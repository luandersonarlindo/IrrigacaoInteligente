#include "schedule_persistence.h"

namespace SchedulePersistence
{

uint16_t crc16(const uint8_t *dados, size_t tamanho)
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

bool bancoValido(uint16_t versaoLida, uint16_t versaoEsperada,
                  uint16_t crcArmazenado, uint16_t crcRecalculado)
{
    if (versaoLida != versaoEsperada)
    {
        return false;
    }
    return crcArmazenado == crcRecalculado;
}

} // namespace SchedulePersistence
