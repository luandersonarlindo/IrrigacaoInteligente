#pragma once

// ============================================================
//  irrigation_controller.h — Controlador de Irrigação
//
//  Fase 2: GPIO reais, trigger HIGH, timeout de segurança.
//
//  Responsabilidade: acionar/desacionar relés e monitorar
//  tempo de segurança no modo manual.
//  NÃO conhece menu, NÃO conhece display.
// ============================================================

#include <Arduino.h>
#include "Config.h"
#include "runtime_config_manager.h"

enum class EstadoValvula
{
    FECHADA,
    ABERTA
};

class IrrigationController
{
public:
    explicit IrrigationController(RuntimeConfigManager &config);

    void begin();

    // Toggle: se fechada abre, se aberta fecha
    // Retorna o novo estado após o toggle
    EstadoValvula toggleValvula(int indice);

    // Aciona por agendamento por um periodo em minutos.
    // Se a valvula ja estiver aberta, preserva o maior tempo restante.
    void iniciarAgendamento(int indice, uint16_t duracaoMin);

    // Fecha uma válvula específica
    void fecharValvula(int indice);

    // Fecha todas — segurança garantida
    void fecharTodas();

    // Retorna o estado atual de uma válvula
    EstadoValvula estadoValvula(int indice) const;

    // Retorna true se alguma válvula está aberta
    bool algumAbertas() const;

    // Deve ser chamado no loop() — verifica timeout de segurança
    // Retorna indice da válvula que foi fechada por timeout, -1 se nenhuma
    int atualizar();

private:
    RuntimeConfigManager &_config;
    EstadoValvula _estados[NUM_VALVULAS];
    unsigned long _tempoAbertura[NUM_VALVULAS]; // millis() de quando abriu
    unsigned long _deadlineFechamento[NUM_VALVULAS];
    bool _origemManual[NUM_VALVULAS];

    // Mapa de pinos — índice 0 = canal 1
    static const uint8_t _pinos[NUM_VALVULAS];

    void abrirValvula(int indice);
    void atualizarDeadline(int indice, unsigned long novoDeadline, bool origemManual);
    bool indiceValido(int indice) const;
};
