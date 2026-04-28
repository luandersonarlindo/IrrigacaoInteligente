// ============================================================
//  input_driver.cpp — Implementação do driver de 3 botões
// ============================================================

#include "input_driver.h"

InputDriver::InputDriver()
    : _direcaoEvento(DirecaoNavegacao::NENHUMA),
      _botaoEvento(false),
      _botaoLongoEvento(false),
      _btnUp{HIGH, 0, 0},
      _btnDown{HIGH, 0, 0},
      _btnSelect{HIGH, 0, 0}
{
}

void InputDriver::begin()
{
    pinMode(PIN_BTN_UP, INPUT_PULLUP);
    pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
    pinMode(PIN_BTN_SELECT, INPUT_PULLUP);

    _btnUp.estadoAnterior = digitalRead(PIN_BTN_UP);
    _btnDown.estadoAnterior = digitalRead(PIN_BTN_DOWN);
    _btnSelect.estadoAnterior = digitalRead(PIN_BTN_SELECT);

    if (DEBUG_SERIAL)
    {
        Serial.println("[Input] Driver de 3 botoes inicializado.");
    }
}

void InputDriver::atualizar()
{
    atualizarBotaoDirecao(PIN_BTN_UP, _btnUp, DirecaoNavegacao::ANTI_HORARIO);
    atualizarBotaoDirecao(PIN_BTN_DOWN, _btnDown, DirecaoNavegacao::HORARIO);
    atualizarBotaoAcao(PIN_BTN_SELECT, _btnSelect);
}

DirecaoNavegacao InputDriver::lerDirecao()
{
    DirecaoNavegacao direcao = _direcaoEvento;
    _direcaoEvento = DirecaoNavegacao::NENHUMA;
    return direcao;
}

bool InputDriver::botaoPressionado()
{
    if (_botaoEvento)
    {
        _botaoEvento = false; // consome o evento — só retorna true uma vez
        return true;
    }
    return false;
}

bool InputDriver::botaoLongoPressionado()
{
    if (_botaoLongoEvento)
    {
        _botaoLongoEvento = false; // consome o evento
        return true;
    }
    return false;
}

void InputDriver::atualizarBotaoDirecao(uint8_t pin, EstadoBotao &estado, DirecaoNavegacao direcaoEvento)
{
    bool estadoAtual = digitalRead(pin);
    unsigned long agora = millis();

    if (estadoAtual != estado.estadoAnterior)
    {
        if (agora - estado.ultimoEventoMs > DEBOUNCE_MS)
        {
            estado.ultimoEventoMs = agora;
            estado.estadoAnterior = estadoAtual;

            if (estadoAtual == LOW)
            {
                _direcaoEvento = direcaoEvento;
            }
        }
    }
}

void InputDriver::atualizarBotaoAcao(uint8_t pin, EstadoBotao &estado)
{
    bool estadoAtual = digitalRead(pin);
    unsigned long agora = millis();

    if (estadoAtual == LOW && estado.estadoAnterior == HIGH)
    {
        if (agora - estado.ultimoEventoMs > DEBOUNCE_MS)
        {
            estado.tempoPressionadoMs = agora;
            estado.ultimoEventoMs = agora;
        }
    }

    if (estadoAtual == HIGH && estado.estadoAnterior == LOW)
    {
        if (agora - estado.ultimoEventoMs > DEBOUNCE_MS)
        {
            unsigned long duracao = agora - estado.tempoPressionadoMs;

            if (duracao >= LONG_PRESS_MS)
            {
                _botaoLongoEvento = true;
            }
            else
            {
                _botaoEvento = true;
            }

            estado.ultimoEventoMs = agora;
        }
    }

    estado.estadoAnterior = estadoAtual;
}
