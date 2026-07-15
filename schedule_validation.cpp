#include "schedule_validation.h"

namespace ScheduleValidation
{

bool validar(const CampoAgenda &agenda, const char *&erro)
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

bool duplicada(const CampoAgenda &candidata, const CampoAgenda *existentes,
               const bool *ativas, int total, int slotIgnorado)
{
    for (int i = 0; i < total; i++)
    {
        if (i == slotIgnorado)
        {
            continue;
        }
        if (!ativas[i])
        {
            continue;
        }

        const CampoAgenda &atual = existentes[i];
        if (atual.hora == candidata.hora &&
            atual.minuto == candidata.minuto &&
            atual.diasMask == candidata.diasMask &&
            atual.setoresMask == candidata.setoresMask)
        {
            return true;
        }
    }
    return false;
}

} // namespace ScheduleValidation
