#include "schedule_execution.h"

namespace ScheduleExecution
{

int selecionarLote(bool pendentes[], bool selecionados[], int totalSetores, int limiteSimultaneo)
{
    int iniciados = 0;
    for (int i = 0; i < totalSetores; i++)
    {
        if (!pendentes[i])
        {
            continue;
        }

        pendentes[i] = false;
        selecionados[i] = true;
        iniciados++;

        if (iniciados >= limiteSimultaneo)
        {
            break;
        }
    }
    return iniciados;
}

bool deveAtualizarDuracaoPendente(bool jaPendente, uint16_t duracaoAtual, uint16_t duracaoNova)
{
    return !jaPendente || duracaoNova > duracaoAtual;
}

} // namespace ScheduleExecution
