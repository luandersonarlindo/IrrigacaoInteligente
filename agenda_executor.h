#pragma once

// ============================================================
//  agenda_executor.h — Execução sequencial de agenda (lotes + intervalo)
//
//  Responsabilidade: máquina de estados que decide QUANDO abrir o
//  próximo lote de setores agendados, respeitando o limite de setores
//  simultâneos e o intervalo entre lotes.
//
//  Extraído de IrrigacaoInteligente.ino: "main.ino não tem lógica de
//  negócio" — esta classe é o dono dessa lógica, main.ino só a invoca.
// ============================================================

#include <Arduino.h>
#include "Config.h"
#include "irrigation_controller.h"

class AgendaExecutor
{
public:
    explicit AgendaExecutor(IrrigationController &irrigacao);

    // Recebe, para cada setor, a duração (min) disparada neste minuto pela
    // agenda (0 = nenhum disparo). Enfileira os setores pendentes.
    void enfileirarDisparos(const uint16_t duracoesMinPorSetor[NUM_VALVULAS]);

    // Deve ser chamado a cada iteração do loop() — avança a máquina de
    // estados (abre próximo lote quando o atual termina e o intervalo
    // decorre).
    void atualizar();

    // Cancela toda execução automática em andamento (ex.: agenda excluída
    // pelo usuário). Fecha apenas válvulas de origem automática.
    void cancelar();

    // --- Estado, para o dashboard (display/web) ---
    bool ativa() const;
    bool aguardandoIntervalo() const;
    uint8_t setoresEmLote() const;
    uint8_t setoresPendentes() const;
    uint16_t maskSetoresEmLote() const;
    uint16_t maskSetoresPendentes() const;

private:
    IrrigationController &_irrigacao;

    bool _ativa;
    bool _aguardandoIntervalo;
    unsigned long _proximoLoteMs;
    uint16_t _maskPendente;
    uint16_t _maskNoLote;
    uint16_t _duracaoPendenteMin[NUM_VALVULAS];

    bool loteAtualAindaAberto() const;
    void limparLoteAtual();
    void iniciarProximoLote();
};
