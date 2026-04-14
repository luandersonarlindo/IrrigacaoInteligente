#pragma once

// ============================================================
//  encoder_driver.h — Driver de navegação local (4 botões)
//
//  Responsabilidade: ler direção, seleção e voltar.
//  NÃO conhece menu, NÃO conhece display.
//  Só reporta o que o hardware fez.
// ============================================================

#include <Arduino.h>
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

    // Inicializa os pinos dos botões
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
    struct EstadoBotao
    {
        bool estadoAnterior;
        unsigned long ultimoEventoMs;
        unsigned long tempoPressionadoMs;
    };

    DirecaoEncoder _direcaoEvento;
    bool _botaoEvento;
    bool _botaoLongoEvento;

    EstadoBotao _btnUp;
    EstadoBotao _btnDown;
    EstadoBotao _btnSelect;
    EstadoBotao _btnBack;

    static const unsigned long DEBOUNCE_MS = BUTTON_DEBOUNCE_MS;
    static const unsigned long LONG_PRESS_MS = BUTTON_LONG_PRESS_MS;

    void atualizarBotaoDirecao(uint8_t pin, EstadoBotao &estado, DirecaoEncoder direcaoEvento);
    void atualizarBotaoAcao(uint8_t pin, EstadoBotao &estado, bool emitirComoVoltar);
};
