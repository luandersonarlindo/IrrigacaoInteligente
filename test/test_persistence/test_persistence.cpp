#include <unity.h>
#include <cstring>
#include "schedule_persistence.h"

void setUp(void) {}
void tearDown(void) {}

void test_crc16_mesmo_dado_gera_mesmo_crc(void)
{
    const uint8_t dados[] = {1, 2, 3, 4, 5};
    uint16_t crcA = SchedulePersistence::crc16(dados, sizeof(dados));
    uint16_t crcB = SchedulePersistence::crc16(dados, sizeof(dados));
    TEST_ASSERT_EQUAL_UINT16(crcA, crcB);
}

void test_crc16_dado_diferente_gera_crc_diferente(void)
{
    const uint8_t dadosA[] = {1, 2, 3, 4, 5};
    const uint8_t dadosB[] = {1, 2, 3, 4, 6};
    uint16_t crcA = SchedulePersistence::crc16(dadosA, sizeof(dadosA));
    uint16_t crcB = SchedulePersistence::crc16(dadosB, sizeof(dadosB));
    TEST_ASSERT_NOT_EQUAL(crcA, crcB);
}

void test_banco_valido_com_versao_e_crc_corretos(void)
{
    const uint8_t dados[] = {10, 20, 30};
    uint16_t crc = SchedulePersistence::crc16(dados, sizeof(dados));
    TEST_ASSERT_TRUE(SchedulePersistence::bancoValido(/*versaoLida=*/1, /*versaoEsperada=*/1, crc, crc));
}

void test_banco_invalido_quando_crc_nao_bate(void)
{
    TEST_ASSERT_FALSE(SchedulePersistence::bancoValido(/*versaoLida=*/1, /*versaoEsperada=*/1,
                                                         /*crcArmazenado=*/0x1234, /*crcRecalculado=*/0x5678));
}

void test_banco_invalido_quando_versao_diverge(void)
{
    const uint8_t dados[] = {1};
    uint16_t crc = SchedulePersistence::crc16(dados, sizeof(dados));
    TEST_ASSERT_FALSE(SchedulePersistence::bancoValido(/*versaoLida=*/2, /*versaoEsperada=*/1, crc, crc));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_crc16_mesmo_dado_gera_mesmo_crc);
    RUN_TEST(test_crc16_dado_diferente_gera_crc_diferente);
    RUN_TEST(test_banco_valido_com_versao_e_crc_corretos);
    RUN_TEST(test_banco_invalido_quando_crc_nao_bate);
    RUN_TEST(test_banco_invalido_quando_versao_diverge);
    return UNITY_END();
}
