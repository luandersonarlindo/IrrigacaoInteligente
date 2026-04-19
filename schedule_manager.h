#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <RTClib.h>
#include "Config.h"
#include "runtime_config_manager.h"

enum DiaSemanaBit
{
    DIA_DOM = 1 << 0,
    DIA_SEG = 1 << 1,
    DIA_TER = 1 << 2,
    DIA_QUA = 1 << 3,
    DIA_QUI = 1 << 4,
    DIA_SEX = 1 << 5,
    DIA_SAB = 1 << 6
};

struct AgendaSetor
{
    bool ativa;
    uint8_t hora;
    uint8_t minuto;
    uint16_t duracaoMin;
    uint8_t diasMask;
    uint8_t setoresMask;
};

struct BancoAgendas
{
    uint16_t versao;
    uint16_t crc;
    AgendaSetor agendas[MAX_AGENDAS_TOTAIS];
};

class ScheduleManager
{
public:
    explicit ScheduleManager(RuntimeConfigManager &config);

    bool begin();

    bool obterAgenda(int slot, AgendaSetor &agenda) const;
    bool salvarAgenda(int slot, const AgendaSetor &agenda, String &erro);
    bool removerAgenda(int slot);
    bool limparTodasAgendas();

    int totalAtivas() const;
    uint16_t duracaoPadraoMin() const;

    // Localiza a próxima execução real considerando dia/horário das agendas.
    // Retorna false quando não houver nenhuma agenda ativa válida.
    bool obterProximaExecucao(const DateTime &agora, DateTime &proximaDataHora, AgendaSetor &agenda, int &slot) const;

    // Preenche duracoesMinPorSetor para disparos do minuto atual.
    // Se nenhum disparo no minuto, o vetor sai com zeros.
    void avaliarDisparos(const DateTime &agora, uint16_t duracoesMinPorSetor[NUM_VALVULAS]);

private:
    RuntimeConfigManager &_config;
    static const uint16_t VERSAO_BANCO = 1;
    static const uint16_t VERSAO_CACHE_EXECUCAO = 1;
    static constexpr const char *NAMESPACE_NVS = "irrig_sched";
    static constexpr const char *KEY_BANCO = "bank";
    static constexpr const char *KEY_CACHE_EXECUCAO = "exec_cache";

    struct CacheExecucaoDiaria
    {
        uint16_t versao;
        uint16_t crc;
        int32_t ultimaExecucaoDiaPorSlot[MAX_AGENDAS_TOTAIS];
    };

    BancoAgendas _banco;
    Preferences _prefs;

    int32_t _ultimaChaveMinuto;
    int _ultimaExecucaoDiaPorSlot[MAX_AGENDAS_TOTAIS];

    void inicializarBancoPadrao();
    bool carregarBanco();
    bool salvarBanco();
    void limparCacheExecucaoDiaMemoria();
    bool carregarCacheExecucaoDia();
    bool salvarCacheExecucaoDia();

    bool indiceSlotValido(int slot) const;

    bool agendaValida(const AgendaSetor &agenda, String &erro) const;
    bool duplicada(int slotIgnorado, const AgendaSetor &agenda) const;

    static uint16_t calcularCrc16(const uint8_t *dados, size_t tamanho);
    static int chaveDia(const DateTime &agora);
    static int32_t chaveMinuto(const DateTime &agora);
    static uint8_t bitDiaSemana(const DateTime &agora);
    static uint8_t bitDiaPorIndice(int indiceDiaSemana);
};
