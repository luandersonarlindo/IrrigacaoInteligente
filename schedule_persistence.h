#pragma once

#include <cstdint>
#include <cstddef>

// CRC16 e checagem de versão/CRC do banco de agendas (e de outros blocos
// persistidos em NVS), extraídos de ScheduleManager, sem dependência de
// hardware (Preferences), para permitir teste em host.
namespace SchedulePersistence
{

// CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF). Mesmo algoritmo usado
// para o banco de agendas e para o cache de execução diária.
uint16_t crc16(const uint8_t *dados, size_t tamanho);

// Decide se um bloco lido da NVS é válido: versão bate com a esperada
// e o CRC armazenado bate com o CRC recalculado sobre os bytes lidos
// (com o campo crc zerado antes do cálculo, responsabilidade do chamador).
bool bancoValido(uint16_t versaoLida, uint16_t versaoEsperada,
                  uint16_t crcArmazenado, uint16_t crcRecalculado);

} // namespace SchedulePersistence
