#pragma once

#include <cstdint>
#include "Config.h"

// Regras de validação de agenda extraídas de ScheduleManager, sem dependência
// de hardware (Preferences/RTClib/Arduino), para permitir teste em host.
namespace ScheduleValidation
{

struct CampoAgenda
{
    uint8_t hora;
    uint8_t minuto;
    uint16_t duracaoMin;
    uint8_t diasMask;
    uint8_t setoresMask;
};

// Valida os campos de uma agenda. Se inválida, preenche `erro` com uma
// mensagem curta (ponteiro para string estática, não precisa ser liberado).
bool validar(const CampoAgenda &agenda, const char *&erro);

// Retorna true se `candidata` colide (mesma hora+minuto+diasMask+setoresMask)
// com alguma agenda ativa em `existentes` (tamanho `total`), ignorando o
// índice `slotIgnorado` (use -1 para não ignorar nenhum).
bool duplicada(const CampoAgenda &candidata, const CampoAgenda *existentes,
               const bool *ativas, int total, int slotIgnorado);

} // namespace ScheduleValidation
