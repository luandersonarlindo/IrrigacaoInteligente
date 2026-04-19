#include "schedule_manager.h"

ScheduleManager::ScheduleManager(RuntimeConfigManager &config)
    : _config(config),
      _ultimaChaveMinuto(-1)
{
    inicializarBancoPadrao();
}

bool ScheduleManager::begin()
{
    if (!_prefs.begin(NAMESPACE_NVS, false))
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Agenda] ERRO: falha ao abrir NVS.");
        }
        return false;
    }

    bool bancoReiniciado = false;
    if (!carregarBanco())
    {
        inicializarBancoPadrao();
        if (!salvarBanco())
        {
            return false;
        }
        bancoReiniciado = true;
    }

    if (bancoReiniciado)
    {
        // Evita reaproveitar cache de execução diária quando o banco foi recriado.
        limparCacheExecucaoDiaMemoria();
        salvarCacheExecucaoDia();
    }
    else if (!carregarCacheExecucaoDia())
    {
        limparCacheExecucaoDiaMemoria();
        salvarCacheExecucaoDia();
    }

    if (DEBUG_SERIAL)
    {
        Serial.println("[Agenda] ScheduleManager inicializado.");
    }

    return true;
}

bool ScheduleManager::obterAgenda(int slot, AgendaSetor &agenda) const
{
    if (!indiceSlotValido(slot))
    {
        return false;
    }

    agenda = _banco.agendas[slot];
    return true;
}

bool ScheduleManager::salvarAgenda(int slot, const AgendaSetor &agenda, String &erro)
{
    if (!indiceSlotValido(slot))
    {
        erro = "indice invalido";
        return false;
    }

    if (!agendaValida(agenda, erro))
    {
        return false;
    }

    if (duplicada(slot, agenda))
    {
        erro = "agenda duplicada";
        return false;
    }

    _banco.agendas[slot] = agenda;

    if (!salvarBanco())
    {
        erro = "falha ao persistir";
        return false;
    }

    // Força nova avaliação no loop mesmo dentro do mesmo minuto.
    _ultimaChaveMinuto = -1;
    // Permite novo disparo do slot no mesmo dia após edição/salvamento.
    _ultimaExecucaoDiaPorSlot[slot] = -1;
    salvarCacheExecucaoDia();

    erro = "";
    return true;
}

bool ScheduleManager::removerAgenda(int slot)
{
    if (!indiceSlotValido(slot))
    {
        return false;
    }

    AgendaSetor limpa = {};
    limpa.ativa = false;
    limpa.hora = 0;
    limpa.minuto = 0;
    limpa.duracaoMin = _config.duracaoPadraoMin();
    limpa.diasMask = 0;
    limpa.setoresMask = 0;

    _banco.agendas[slot] = limpa;
    bool ok = salvarBanco();
    if (ok)
    {
        // Evita cache do minuto anterior após alteração de agenda.
        _ultimaChaveMinuto = -1;
        // Libera o slot para nova execução no dia atual.
        _ultimaExecucaoDiaPorSlot[slot] = -1;
        salvarCacheExecucaoDia();
    }
    return ok;
}

bool ScheduleManager::limparTodasAgendas()
{
    for (int slot = 0; slot < MAX_AGENDAS_TOTAIS; slot++)
    {
        _banco.agendas[slot].ativa = false;
        _banco.agendas[slot].hora = 6;
        _banco.agendas[slot].minuto = 0;
        _banco.agendas[slot].duracaoMin = _config.duracaoPadraoMin();
        _banco.agendas[slot].diasMask = 0;
        _banco.agendas[slot].setoresMask = 0;
    }

    bool ok = salvarBanco();
    if (ok)
    {
        _ultimaChaveMinuto = -1;
        limparCacheExecucaoDiaMemoria();
        salvarCacheExecucaoDia();
    }
    return ok;
}

int ScheduleManager::totalAtivas() const
{
    int total = 0;
    for (int i = 0; i < MAX_AGENDAS_TOTAIS; i++)
    {
        if (_banco.agendas[i].ativa)
        {
            total++;
        }
    }
    return total;
}

uint16_t ScheduleManager::duracaoPadraoMin() const
{
    return _config.duracaoPadraoMin();
}

bool ScheduleManager::obterProximaExecucao(const DateTime &agora, DateTime &proximaDataHora, AgendaSetor &agenda, int &slot) const
{
    bool encontrou = false;
    uint32_t menorEpoch = 0;
    AgendaSetor melhorAgenda = {};
    int melhorSlot = -1;

    for (int i = 0; i < MAX_AGENDAS_TOTAIS; i++)
    {
        const AgendaSetor &atual = _banco.agendas[i];
        if (!atual.ativa)
        {
            continue;
        }

        if ((atual.diasMask & 0x7F) == 0)
        {
            continue;
        }

        for (int deltaDia = 0; deltaDia <= 7; deltaDia++)
        {
            DateTime dataBase = agora + TimeSpan(deltaDia, 0, 0, 0);
            uint8_t bitDia = bitDiaPorIndice(dataBase.dayOfTheWeek());
            if ((atual.diasMask & bitDia) == 0)
            {
                continue;
            }

            DateTime candidata(dataBase.year(), dataBase.month(), dataBase.day(), atual.hora, atual.minuto, 0);
            uint32_t epoch = candidata.unixtime();

            if (deltaDia == 0)
            {
                bool minutoJaPassou = (agora.hour() > atual.hora) ||
                                      (agora.hour() == atual.hora && agora.minute() > atual.minuto);
                if (minutoJaPassou)
                {
                    continue;
                }
            }

            if (!encontrou || epoch < menorEpoch)
            {
                encontrou = true;
                menorEpoch = epoch;
                melhorAgenda = atual;
                melhorSlot = i;
            }

            break;
        }
    }

    if (!encontrou)
    {
        return false;
    }

    proximaDataHora = DateTime(menorEpoch);
    agenda = melhorAgenda;
    slot = melhorSlot;
    return true;
}

void ScheduleManager::avaliarDisparos(const DateTime &agora, uint16_t duracoesMinPorSetor[NUM_VALVULAS])
{
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        duracoesMinPorSetor[i] = 0;
    }

    int32_t chave = chaveMinuto(agora);
    if (chave == _ultimaChaveMinuto)
    {
        return;
    }
    _ultimaChaveMinuto = chave;

    uint8_t bitDia = bitDiaSemana(agora);
    int chaveDiaAtual = chaveDia(agora);
    uint32_t agoraEpoch = agora.unixtime();
    bool cacheExecucaoAtualizado = false;

    for (int slot = 0; slot < MAX_AGENDAS_TOTAIS; slot++)
    {
        const AgendaSetor &agenda = _banco.agendas[slot];
        if (!agenda.ativa)
        {
            continue;
        }
        if ((agenda.diasMask & bitDia) == 0)
        {
            continue;
        }
        if (_ultimaExecucaoDiaPorSlot[slot] == chaveDiaAtual)
        {
            continue;
        }

        DateTime inicioHoje(agora.year(), agora.month(), agora.day(), agenda.hora, agenda.minuto, 0);
        uint32_t inicioEpoch = inicioHoje.unixtime();

        // Ainda não chegou no horário da agenda de hoje.
        if (agoraEpoch < inicioEpoch)
        {
            continue;
        }

        int totalSetoresAgenda = 0;
        int setoresAgenda[NUM_VALVULAS];
        for (int setor = 0; setor < NUM_VALVULAS; setor++)
        {
            if (agenda.setoresMask & (1 << setor))
            {
                setoresAgenda[totalSetoresAgenda] = setor;
                totalSetoresAgenda++;
            }
        }

        if (totalSetoresAgenda <= 0)
        {
            continue;
        }

        int lotes = (totalSetoresAgenda + MAX_SETOR_SIMULTANEOS_AGENDA - 1) / MAX_SETOR_SIMULTANEOS_AGENDA;
        uint32_t totalMinLotes = (uint32_t)lotes * (uint32_t)agenda.duracaoMin;
        uint32_t totalMinIntervalos = (uint32_t)(((unsigned long)(lotes - 1) * INTERVALO_LOTE_AGENDA_MS + 59999UL) / 60000UL);
        uint32_t janelaTotalMin = totalMinLotes + totalMinIntervalos;
        if (janelaTotalMin == 0)
        {
            janelaTotalMin = 1;
        }

        uint32_t fimJanelaEpoch = inicioEpoch + (janelaTotalMin * 60UL);
        if (agoraEpoch >= fimJanelaEpoch)
        {
            continue;
        }

        // Descobre em qual lote a agenda deveria estar no momento atual.
        uint32_t duracaoLoteSec = (uint32_t)agenda.duracaoMin * 60UL;
        uint32_t intervaloSec = (INTERVALO_LOTE_AGENDA_MS + 999UL) / 1000UL;
        uint32_t elapsedSec = agoraEpoch - inicioEpoch;

        int loteAlvo = -1;
        bool loteAlvoEmExecucao = false;
        uint32_t restanteLoteAlvoSec = duracaoLoteSec;

        for (int lote = 0; lote < lotes; lote++)
        {
            uint32_t inicioLoteSec = (uint32_t)lote * (duracaoLoteSec + intervaloSec);
            uint32_t fimLoteSec = inicioLoteSec + duracaoLoteSec;

            if (elapsedSec < fimLoteSec)
            {
                loteAlvo = lote;
                loteAlvoEmExecucao = true;
                restanteLoteAlvoSec = fimLoteSec - elapsedSec;
                if (restanteLoteAlvoSec == 0)
                {
                    restanteLoteAlvoSec = 1;
                }
                break;
            }

            // Intervalo entre lotes: retoma no proximo lote.
            if (lote < (lotes - 1))
            {
                uint32_t fimIntervaloSec = fimLoteSec + intervaloSec;
                if (elapsedSec < fimIntervaloSec)
                {
                    loteAlvo = lote + 1;
                    loteAlvoEmExecucao = false;
                    restanteLoteAlvoSec = duracaoLoteSec;
                    break;
                }
            }
        }

        if (loteAlvo < 0 || loteAlvo >= lotes)
        {
            continue;
        }

        if (DEBUG_SERIAL)
        {
            Serial.print("[Agenda] Disparo slot ");
            Serial.print(slot + 1);
            Serial.print(" em janela ativa (inicio ");
            Serial.print(agenda.hora);
            Serial.print(":");
            Serial.print(agenda.minuto);
            Serial.println(").");
        }

        for (int lote = loteAlvo; lote < lotes; lote++)
        {
            int inicioIdx = lote * MAX_SETOR_SIMULTANEOS_AGENDA;
            int fimIdx = inicioIdx + MAX_SETOR_SIMULTANEOS_AGENDA;
            if (fimIdx > totalSetoresAgenda)
                fimIdx = totalSetoresAgenda;

            uint16_t duracaoParaLoteMin = agenda.duracaoMin;
            if (lote == loteAlvo && loteAlvoEmExecucao)
            {
                uint32_t restoMin = (restanteLoteAlvoSec + 59UL) / 60UL;
                if (restoMin < 1UL)
                    restoMin = 1UL;
                if (restoMin > agenda.duracaoMin)
                    restoMin = agenda.duracaoMin;
                duracaoParaLoteMin = (uint16_t)restoMin;
            }

            for (int idx = inicioIdx; idx < fimIdx; idx++)
            {
                int setor = setoresAgenda[idx];
                if (duracaoParaLoteMin > duracoesMinPorSetor[setor])
                {
                    duracoesMinPorSetor[setor] = duracaoParaLoteMin;
                }
            }
        }

        _ultimaExecucaoDiaPorSlot[slot] = chaveDiaAtual;
        cacheExecucaoAtualizado = true;
    }

    if (cacheExecucaoAtualizado)
    {
        salvarCacheExecucaoDia();
    }
}

void ScheduleManager::inicializarBancoPadrao()
{
    _banco.versao = VERSAO_BANCO;
    _banco.crc = 0;
    limparCacheExecucaoDiaMemoria();

    for (int slot = 0; slot < MAX_AGENDAS_TOTAIS; slot++)
    {
        _banco.agendas[slot].ativa = false;
        _banco.agendas[slot].hora = 6;
        _banco.agendas[slot].minuto = 0;
        _banco.agendas[slot].duracaoMin = _config.duracaoPadraoMin();
        _banco.agendas[slot].diasMask = 0;
        _banco.agendas[slot].setoresMask = 0;
    }
}

bool ScheduleManager::carregarBanco()
{
    size_t esperado = sizeof(BancoAgendas);
    size_t atual = _prefs.getBytesLength(KEY_BANCO);
    if (atual != esperado)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Agenda] Banco ausente/incompativel. Inicializando padrao.");
        }
        return false;
    }

    size_t lidos = _prefs.getBytes(KEY_BANCO, &_banco, sizeof(BancoAgendas));
    if (lidos != sizeof(BancoAgendas))
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Agenda] Falha ao ler banco da NVS.");
        }
        return false;
    }

    if (_banco.versao != VERSAO_BANCO)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Agenda] Versao de banco invalida.");
        }
        return false;
    }

    uint16_t crcSalvo = _banco.crc;
    _banco.crc = 0;
    uint16_t crcCalc = calcularCrc16(reinterpret_cast<const uint8_t *>(&_banco), sizeof(BancoAgendas));
    _banco.crc = crcSalvo;

    if (crcSalvo != crcCalc)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Agenda] CRC invalido. Banco resetado.");
        }
        return false;
    }

    return true;
}

bool ScheduleManager::salvarBanco()
{
    _banco.versao = VERSAO_BANCO;
    _banco.crc = 0;
    _banco.crc = calcularCrc16(reinterpret_cast<const uint8_t *>(&_banco), sizeof(BancoAgendas));

    size_t escritos = _prefs.putBytes(KEY_BANCO, &_banco, sizeof(BancoAgendas));
    bool ok = (escritos == sizeof(BancoAgendas));

    if (!ok && DEBUG_SERIAL)
    {
        Serial.println("[Agenda] ERRO: falha ao salvar banco na NVS.");
    }

    return ok;
}

void ScheduleManager::limparCacheExecucaoDiaMemoria()
{
    for (int slot = 0; slot < MAX_AGENDAS_TOTAIS; slot++)
    {
        _ultimaExecucaoDiaPorSlot[slot] = -1;
    }
}

bool ScheduleManager::carregarCacheExecucaoDia()
{
    size_t esperado = sizeof(CacheExecucaoDiaria);
    size_t atual = _prefs.getBytesLength(KEY_CACHE_EXECUCAO);
    if (atual != esperado)
    {
        return false;
    }

    CacheExecucaoDiaria cache = {};
    size_t lidos = _prefs.getBytes(KEY_CACHE_EXECUCAO, &cache, sizeof(CacheExecucaoDiaria));
    if (lidos != sizeof(CacheExecucaoDiaria))
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Agenda] Falha ao ler cache de execucao diaria.");
        }
        return false;
    }

    if (cache.versao != VERSAO_CACHE_EXECUCAO)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Agenda] Versao do cache de execucao invalida.");
        }
        return false;
    }

    uint16_t crcSalvo = cache.crc;
    cache.crc = 0;
    uint16_t crcCalc = calcularCrc16(reinterpret_cast<const uint8_t *>(&cache), sizeof(CacheExecucaoDiaria));
    if (crcSalvo != crcCalc)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Agenda] CRC invalido no cache de execucao diaria.");
        }
        return false;
    }

    for (int slot = 0; slot < MAX_AGENDAS_TOTAIS; slot++)
    {
        _ultimaExecucaoDiaPorSlot[slot] = cache.ultimaExecucaoDiaPorSlot[slot];
    }

    return true;
}

bool ScheduleManager::salvarCacheExecucaoDia()
{
    CacheExecucaoDiaria cache = {};
    cache.versao = VERSAO_CACHE_EXECUCAO;
    cache.crc = 0;

    for (int slot = 0; slot < MAX_AGENDAS_TOTAIS; slot++)
    {
        cache.ultimaExecucaoDiaPorSlot[slot] = _ultimaExecucaoDiaPorSlot[slot];
    }

    cache.crc = calcularCrc16(reinterpret_cast<const uint8_t *>(&cache), sizeof(CacheExecucaoDiaria));

    size_t escritos = _prefs.putBytes(KEY_CACHE_EXECUCAO, &cache, sizeof(CacheExecucaoDiaria));
    bool ok = (escritos == sizeof(CacheExecucaoDiaria));

    if (!ok && DEBUG_SERIAL)
    {
        Serial.println("[Agenda] ERRO: falha ao salvar cache de execucao diaria.");
    }

    return ok;
}

bool ScheduleManager::indiceSlotValido(int slot) const
{
    return slot >= 0 && slot < MAX_AGENDAS_TOTAIS;
}

bool ScheduleManager::agendaValida(const AgendaSetor &agenda, String &erro) const
{
    if (agenda.hora > 23)
    {
        erro = "hora invalida";
        return false;
    }
    if (agenda.minuto > 59)
    {
        erro = "minuto invalido";
        return false;
    }
    if (agenda.duracaoMin < 1)
    {
        erro = "duracao invalida";
        return false;
    }
    if ((agenda.diasMask & 0x7F) == 0)
    {
        erro = "selecione ao menos um dia";
        return false;
    }
    if ((agenda.setoresMask & ((1 << NUM_VALVULAS) - 1)) == 0)
    {
        erro = "selecione ao menos um setor";
        return false;
    }
    return true;
}

bool ScheduleManager::duplicada(int slotIgnorado, const AgendaSetor &agenda) const
{
    for (int i = 0; i < MAX_AGENDAS_TOTAIS; i++)
    {
        if (i == slotIgnorado)
        {
            continue;
        }

        const AgendaSetor &atual = _banco.agendas[i];
        if (!atual.ativa)
        {
            continue;
        }

        if (atual.hora == agenda.hora &&
            atual.minuto == agenda.minuto &&
            atual.diasMask == agenda.diasMask &&
            atual.setoresMask == agenda.setoresMask)
        {
            return true;
        }
    }
    return false;
}

uint16_t ScheduleManager::calcularCrc16(const uint8_t *dados, size_t tamanho)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < tamanho; i++)
    {
        crc ^= static_cast<uint16_t>(dados[i]) << 8;
        for (uint8_t b = 0; b < 8; b++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ 0x1021;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

int32_t ScheduleManager::chaveMinuto(const DateTime &agora)
{
    // Chave monotonica por minuto, sem risco de overflow em concatenacao decimal.
    return (int32_t)(agora.unixtime() / 60UL);
}

int ScheduleManager::chaveDia(const DateTime &agora)
{
    int chave = agora.year();
    chave = (chave * 100) + agora.month();
    chave = (chave * 100) + agora.day();
    return chave;
}

uint8_t ScheduleManager::bitDiaSemana(const DateTime &agora)
{
    switch (agora.dayOfTheWeek())
    {
    case 0:
        return DIA_DOM;
    case 1:
        return DIA_SEG;
    case 2:
        return DIA_TER;
    case 3:
        return DIA_QUA;
    case 4:
        return DIA_QUI;
    case 5:
        return DIA_SEX;
    default:
        return DIA_SAB;
    }
}

uint8_t ScheduleManager::bitDiaPorIndice(int indiceDiaSemana)
{
    switch (indiceDiaSemana)
    {
    case 0:
        return DIA_DOM;
    case 1:
        return DIA_SEG;
    case 2:
        return DIA_TER;
    case 3:
        return DIA_QUA;
    case 4:
        return DIA_QUI;
    case 5:
        return DIA_SEX;
    default:
        return DIA_SAB;
    }
}
