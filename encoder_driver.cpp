// ============================================================
//  encoder_driver.cpp — Implementação do EncoderDriver
// ============================================================

#include "encoder_driver.h"

EncoderDriver::EncoderDriver()
    : _posicaoAnterior(0),
      _estadoAnteriorBtn(HIGH),
      _botaoEvento(false),
      _botaoLongoEvento(false),
      _tempoPressionadoBtn(0),
      _ultimoTempoBtn(0)
{
}

void EncoderDriver::begin()
{
    // Habilita pull-up interno nos pinos do encoder
    ESP32Encoder::useInternalWeakPullResistors = puType::up;

    // attachHalfQuad: detecta borda de subida e descida — mais responsivo
    _encoder.attachHalfQuad(PIN_ENCODER_CLK, PIN_ENCODER_DT);
    _encoder.setCount(0);
    _posicaoAnterior = 0;

    // Botão com pull-up interno
    pinMode(PIN_ENCODER_BTN, INPUT_PULLUP);

    if (DEBUG_SERIAL)
    {
        Serial.println("[Encoder] Inicializado.");
    }
}

void EncoderDriver::atualizar()
{
    // --- Leitura do botão com debounce + clique longo ---
    bool estadoAtual = digitalRead(PIN_ENCODER_BTN);
    unsigned long agora = millis();

    // Borda de descida: início do pressionamento
    if (estadoAtual == LOW && _estadoAnteriorBtn == HIGH)
    {
        if (agora - _ultimoTempoBtn > DEBOUNCE_MS)
        {
            _tempoPressionadoBtn = agora;
            _ultimoTempoBtn = agora;
        }
    }

    // Borda de subida: fim do pressionamento
    if (estadoAtual == HIGH && _estadoAnteriorBtn == LOW)
    {
        if (agora - _ultimoTempoBtn > DEBOUNCE_MS)
        {
            unsigned long duracao = agora - _tempoPressionadoBtn;
            if (duracao >= LONG_PRESS_MS)
            {
                _botaoLongoEvento = true;
            }
            else
            {
                _botaoEvento = true;
            }
            _ultimoTempoBtn = agora;
        }
    }

    _estadoAnteriorBtn = estadoAtual;
}

DirecaoEncoder EncoderDriver::lerDirecao()
{
    long posicaoAtual = _encoder.getCount();
    long delta = posicaoAtual - _posicaoAnterior;

    if (delta == 0)
        return DirecaoEncoder::NENHUMA;

    // Reseta para evitar acúmulo
    _posicaoAnterior = posicaoAtual;

    if (delta > 0)
        return DirecaoEncoder::HORARIO;
    return DirecaoEncoder::ANTI_HORARIO;
}

bool EncoderDriver::botaoPressionado()
{
    if (_botaoEvento)
    {
        _botaoEvento = false; // consome o evento — só retorna true uma vez
        return true;
    }
    return false;
}

bool EncoderDriver::botaoLongoPressionado()
{
    if (_botaoLongoEvento)
    {
        _botaoLongoEvento = false; // consome o evento
        return true;
    }
    return false;
}
