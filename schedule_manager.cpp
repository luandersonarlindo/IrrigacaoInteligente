#include "schedule_manager.h"

ScheduleManager::ScheduleManager()
    : _ultimaChaveMinuto(-1)
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

    if (!carregarBanco())
    {
        inicializarBancoPadrao();
        if (!salvarBanco())
        {
            return false;
        }
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
    limpa.duracaoMin = DURACAO_PADRAO_MIN;
    limpa.diasMask = 0;
    limpa.setoresMask = 0;

    _banco.agendas[slot] = limpa;
    return salvarBanco();
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

void ScheduleManager::avaliarDisparos(const DateTime &agora, uint16_t duracoesMinPorSetor[NUM_VALVULAS])
{
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        duracoesMinPorSetor[i] = 0;
    }

    int chave = chaveMinuto(agora);
    if (chave == _ultimaChaveMinuto)
    {
        return;
    }
    _ultimaChaveMinuto = chave;

    uint8_t bitDia = bitDiaSemana(agora);

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
        if (agenda.hora != agora.hour() || agenda.minuto != agora.minute())
        {
            continue;
        }

        for (int setor = 0; setor < NUM_VALVULAS; setor++)
        {
            uint8_t bitSetor = (1 << setor);
            if ((agenda.setoresMask & bitSetor) == 0)
            {
                continue;
            }

            if (agenda.duracaoMin > duracoesMinPorSetor[setor])
            {
                duracoesMinPorSetor[setor] = agenda.duracaoMin;
            }
        }
    }
}

void ScheduleManager::inicializarBancoPadrao()
{
    _banco.versao = VERSAO_BANCO;
    _banco.crc = 0;

    for (int slot = 0; slot < MAX_AGENDAS_TOTAIS; slot++)
    {
        _banco.agendas[slot].ativa = false;
        _banco.agendas[slot].hora = 6;
        _banco.agendas[slot].minuto = 0;
        _banco.agendas[slot].duracaoMin = DURACAO_PADRAO_MIN;
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

int ScheduleManager::chaveMinuto(const DateTime &agora)
{
    int chave = agora.year();
    chave = (chave * 100) + agora.month();
    chave = (chave * 100) + agora.day();
    chave = (chave * 100) + agora.hour();
    chave = (chave * 100) + agora.minute();
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
