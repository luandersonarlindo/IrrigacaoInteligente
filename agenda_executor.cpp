#include "agenda_executor.h"
#include "schedule_execution.h"

AgendaExecutor::AgendaExecutor(IrrigationController &irrigacao)
    : _irrigacao(irrigacao),
      _ativa(false),
      _aguardandoIntervalo(false),
      _proximoLoteMs(0),
      _maskPendente(0),
      _maskNoLote(0)
{
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        _duracaoPendenteMin[i] = 0;
    }
}

void AgendaExecutor::enfileirarDisparos(const uint16_t duracoesMinPorSetor[NUM_VALVULAS])
{
    bool recebeuNovoDisparo = false;

    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        uint16_t duracao = duracoesMinPorSetor[i];
        if (duracao == 0)
        {
            continue;
        }

        recebeuNovoDisparo = true;
        uint16_t bit = (uint16_t)(1U << i);

        if ((_maskNoLote & bit) && _irrigacao.estadoValvula(i) == EstadoValvula::ABERTA)
        {
            // Se o setor já está em execução, estende apenas se necessário.
            _irrigacao.iniciarAgendamento(i, duracao);
            continue;
        }

        bool jaPendente = (_maskPendente & bit) != 0;
        if (ScheduleExecution::deveAtualizarDuracaoPendente(jaPendente, _duracaoPendenteMin[i], duracao))
        {
            _maskPendente |= bit;
            _duracaoPendenteMin[i] = duracao;
        }
    }

    if (recebeuNovoDisparo)
    {
        _ativa = true;
    }
}

void AgendaExecutor::atualizar()
{
    if (!_ativa)
    {
        return;
    }

    bool tinhaLoteAtual = (_maskNoLote != 0);

    if (loteAtualAindaAberto())
    {
        return;
    }

    if (tinhaLoteAtual)
    {
        limparLoteAtual();

        if (_maskPendente == 0)
        {
            _ativa = false;
            _aguardandoIntervalo = false;
            return;
        }

        _aguardandoIntervalo = true;
        _proximoLoteMs = millis() + INTERVALO_LOTE_AGENDA_MS;
        return;
    }

    if (_maskPendente == 0)
    {
        _ativa = false;
        _aguardandoIntervalo = false;
        return;
    }

    if (!_aguardandoIntervalo)
    {
        iniciarProximoLote();
        return;
    }

    if ((long)(millis() - _proximoLoteMs) >= 0)
    {
        iniciarProximoLote();
    }
}

void AgendaExecutor::cancelar()
{
    _ativa = false;
    _aguardandoIntervalo = false;
    _proximoLoteMs = 0;
    _maskPendente = 0;
    _maskNoLote = 0;

    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        _duracaoPendenteMin[i] = 0;

        // Fecha apenas setores em origem automatica; manual permanece.
        if (_irrigacao.valvulaEmAgendamento(i))
        {
            _irrigacao.fecharValvula(i);
        }
    }

    if (DEBUG_SERIAL)
    {
        Serial.println("[Agenda] Execucao automatica cancelada por exclusao de agenda.");
    }
}

bool AgendaExecutor::ativa() const { return _ativa; }
bool AgendaExecutor::aguardandoIntervalo() const { return _aguardandoIntervalo; }
uint8_t AgendaExecutor::setoresEmLote() const { return (uint8_t)contarBitsSetor(_maskNoLote); }
uint8_t AgendaExecutor::setoresPendentes() const { return (uint8_t)contarBitsSetor(_maskPendente); }
uint16_t AgendaExecutor::maskSetoresEmLote() const { return _maskNoLote; }
uint16_t AgendaExecutor::maskSetoresPendentes() const { return _maskPendente; }

bool AgendaExecutor::loteAtualAindaAberto() const
{
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        if ((_maskNoLote & (1U << i)) && _irrigacao.estadoValvula(i) == EstadoValvula::ABERTA)
        {
            return true;
        }
    }
    return false;
}

void AgendaExecutor::limparLoteAtual()
{
    _maskNoLote = 0;
}

void AgendaExecutor::iniciarProximoLote()
{
    uint16_t selecionados = 0;
    int iniciados = ScheduleExecution::selecionarLoteMask(_maskPendente, selecionados,
                                                            NUM_VALVULAS, MAX_SETOR_SIMULTANEOS_AGENDA);

    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        if ((selecionados & (1U << i)) == 0)
        {
            continue;
        }
        _irrigacao.iniciarAgendamento(i, _duracaoPendenteMin[i]);
        _maskNoLote |= (uint16_t)(1U << i);
        _duracaoPendenteMin[i] = 0;
    }

    _ativa = (iniciados > 0) || (_maskPendente != 0);
    _aguardandoIntervalo = false;
}
