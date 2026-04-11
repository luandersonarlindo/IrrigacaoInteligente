// ============================================================
//  irrigation_controller.cpp — Fase 2
//  GPIO reais, trigger HIGH, timeout de segurança manual
// ============================================================

#include "irrigation_controller.h"

// Mapa de pinos: índice 0 = setor 1, índice 7 = setor 8
const uint8_t IrrigationController::_pinos[NUM_VALVULAS] = {
    PIN_RELAY_1,
    PIN_RELAY_2,
    PIN_RELAY_3,
    PIN_RELAY_4,
    PIN_RELAY_5,
    PIN_RELAY_6,
    PIN_RELAY_7,
    PIN_RELAY_8};

IrrigationController::IrrigationController(RuntimeConfigManager &config)
    : _config(config)
{
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        _estados[i] = EstadoValvula::FECHADA;
        _tempoAbertura[i] = 0;
        _deadlineFechamento[i] = 0;
        _origemManual[i] = false;
    }
}

void IrrigationController::begin()
{
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        pinMode(_pinos[i], OUTPUT);
        digitalWrite(_pinos[i], LOW); // garante tudo fechado na inicialização
    }

    if (DEBUG_SERIAL)
    {
        Serial.println("[Irrigacao] Fase 2 iniciada. Pinos configurados. Valvulas fechadas.");
    }
}

// --- Toggle público ---

EstadoValvula IrrigationController::toggleValvula(int indice)
{
    if (!indiceValido(indice))
        return EstadoValvula::FECHADA;

    if (_estados[indice] == EstadoValvula::FECHADA)
    {
        abrirValvula(indice);
        atualizarDeadline(indice, millis() + _config.timeoutManualMs(), true);
    }
    else
    {
        fecharValvula(indice);
    }
    return _estados[indice];
}

void IrrigationController::iniciarAgendamento(int indice, uint16_t duracaoMin)
{
    if (!indiceValido(indice) || duracaoMin < 1)
    {
        return;
    }

    unsigned long novoDeadline = millis() + (unsigned long)duracaoMin * 60000UL;

    if (_estados[indice] == EstadoValvula::FECHADA)
    {
        abrirValvula(indice);
        atualizarDeadline(indice, novoDeadline, false);
    }
    else
    {
        // Em conflito de horario, conserva o maior tempo restante.
        if ((long)(novoDeadline - _deadlineFechamento[indice]) > 0)
        {
            atualizarDeadline(indice, novoDeadline, false);
        }
    }

    if (DEBUG_SERIAL)
    {
        Serial.print("[Irrigacao] Agendamento setor ");
        Serial.print(indice + 1);
        Serial.print(" por ");
        Serial.print(duracaoMin);
        Serial.println(" min.");
    }
}

// --- Abrir / Fechar ---

void IrrigationController::abrirValvula(int indice)
{
    if (!indiceValido(indice))
        return;

    digitalWrite(_pinos[indice], HIGH);
    _estados[indice] = EstadoValvula::ABERTA;
    _tempoAbertura[indice] = millis();

    if (DEBUG_SERIAL)
    {
        Serial.print("[Irrigacao] Setor ");
        Serial.print(indice + 1);
        Serial.println(" ABERTO.");
    }
}

void IrrigationController::fecharValvula(int indice)
{
    if (!indiceValido(indice))
        return;

    digitalWrite(_pinos[indice], LOW);
    _estados[indice] = EstadoValvula::FECHADA;
    _tempoAbertura[indice] = 0;
    _deadlineFechamento[indice] = 0;
    _origemManual[indice] = false;

    if (DEBUG_SERIAL)
    {
        Serial.print("[Irrigacao] Setor ");
        Serial.print(indice + 1);
        Serial.println(" FECHADO.");
    }
}

void IrrigationController::fecharTodas()
{
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        fecharValvula(i);
    }
}

// --- Getters ---

EstadoValvula IrrigationController::estadoValvula(int indice) const
{
    if (!indiceValido(indice))
        return EstadoValvula::FECHADA;
    return _estados[indice];
}

bool IrrigationController::algumAbertas() const
{
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        if (_estados[i] == EstadoValvula::ABERTA)
            return true;
    }
    return false;
}

// --- Loop: verifica timeout de segurança ---

int IrrigationController::atualizar()
{
    unsigned long agora = millis();

    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        if (_estados[i] == EstadoValvula::ABERTA)
        {
            if ((long)(agora - _deadlineFechamento[i]) >= 0)
            {
                bool eraManual = _origemManual[i];
                fecharValvula(i);

                if (eraManual)
                {
                    if (DEBUG_SERIAL)
                    {
                        Serial.print("[Irrigacao] TIMEOUT: Setor ");
                        Serial.print(i + 1);
                        Serial.println(" fechado automaticamente por seguranca.");
                    }
                    return i; // aviso de timeout manual
                }

                if (DEBUG_SERIAL)
                {
                    Serial.print("[Irrigacao] Agenda concluida no setor ");
                    Serial.print(i + 1);
                    Serial.println(".");
                }
            }
        }
    }
    return -1; // nenhum timeout
}

void IrrigationController::atualizarDeadline(int indice, unsigned long novoDeadline, bool origemManual)
{
    if (!indiceValido(indice))
    {
        return;
    }

    _deadlineFechamento[indice] = novoDeadline;
    _origemManual[indice] = origemManual;
}

// --- Privado ---

bool IrrigationController::indiceValido(int indice) const
{
    if (indice >= 0 && indice < NUM_VALVULAS)
        return true;

    if (DEBUG_SERIAL)
    {
        Serial.print("[Irrigacao] ERRO: indice invalido: ");
        Serial.println(indice);
    }
    return false;
}
