#pragma once

// ============================================================
//  encoder_driver.h — Driver do Encoder Rotativo HW-040
//
//  Responsabilidade: ler rotação e botão do encoder.
//  NÃO conhece menu, NÃO conhece display.
//  Só reporta o que o hardware fez.
// ============================================================

#include <Arduino.h>
#include <ESP32Encoder.h>
#include "Config.h"

// Direções possíveis de rotação
enum class DirecaoEncoder
{
    NENHUMA,
    HORARIO,
    ANTI_HORARIO
};

class EncoderDriver
{
public:
    EncoderDriver();

    // Inicializa os pinos e a biblioteca
    void begin();

    // Deve ser chamado no loop() — atualiza estado interno
    void atualizar();

    // Retorna a direção do último movimento (e reseta para NENHUMA)
    DirecaoEncoder lerDirecao();

    // Retorna true se o botão foi pressionado (com debounce)
    bool botaoPressionado();

    // Retorna true se ocorreu clique longo no botão
    bool botaoLongoPressionado();

private:
    ESP32Encoder _encoder;
    long _posicaoAnterior;

    // Botão
    bool _estadoAnteriorBtn;
    bool _botaoEvento;
    bool _botaoLongoEvento;
    unsigned long _tempoPressionadoBtn;
    unsigned long _ultimoTempoBtn;

    static const unsigned long DEBOUNCE_MS = 50;
    static const unsigned long LONG_PRESS_MS = 1200;
};
