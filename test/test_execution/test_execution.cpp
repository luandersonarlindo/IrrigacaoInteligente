#include <unity.h>
#include "schedule_execution.h"

void setUp(void) {}
void tearDown(void) {}

void test_selecionarLote_menos_pendentes_que_limite_seleciona_todos(void)
{
    bool pendentes[4] = {true, true, false, false};
    bool selecionados[4] = {false, false, false, false};

    int qtd = ScheduleExecution::selecionarLote(pendentes, selecionados, 4, /*limiteSimultaneo=*/2);

    TEST_ASSERT_EQUAL_INT(2, qtd);
    TEST_ASSERT_TRUE(selecionados[0]);
    TEST_ASSERT_TRUE(selecionados[1]);
    TEST_ASSERT_FALSE(pendentes[0]);
    TEST_ASSERT_FALSE(pendentes[1]);
}

void test_selecionarLote_mais_pendentes_que_limite_respeita_limite(void)
{
    bool pendentes[4] = {true, true, true, true};
    bool selecionados[4] = {false, false, false, false};

    int qtd = ScheduleExecution::selecionarLote(pendentes, selecionados, 4, /*limiteSimultaneo=*/2);

    TEST_ASSERT_EQUAL_INT(2, qtd);
    TEST_ASSERT_TRUE(selecionados[0]);
    TEST_ASSERT_TRUE(selecionados[1]);
    TEST_ASSERT_FALSE(selecionados[2]);
    TEST_ASSERT_FALSE(selecionados[3]);
    // Os que nao entraram no lote continuam pendentes.
    TEST_ASSERT_FALSE(pendentes[0]);
    TEST_ASSERT_FALSE(pendentes[1]);
    TEST_ASSERT_TRUE(pendentes[2]);
    TEST_ASSERT_TRUE(pendentes[3]);
}

void test_selecionarLote_sem_pendentes_seleciona_zero(void)
{
    bool pendentes[4] = {false, false, false, false};
    bool selecionados[4] = {false, false, false, false};

    int qtd = ScheduleExecution::selecionarLote(pendentes, selecionados, 4, /*limiteSimultaneo=*/2);

    TEST_ASSERT_EQUAL_INT(0, qtd);
}

void test_selecionarLote_exatamente_no_limite_seleciona_todos(void)
{
    bool pendentes[2] = {true, true};
    bool selecionados[2] = {false, false};

    int qtd = ScheduleExecution::selecionarLote(pendentes, selecionados, 2, /*limiteSimultaneo=*/2);

    TEST_ASSERT_EQUAL_INT(2, qtd);
}

void test_deveAtualizarDuracaoPendente_ainda_nao_pendente_retorna_verdadeiro(void)
{
    TEST_ASSERT_TRUE(ScheduleExecution::deveAtualizarDuracaoPendente(
        /*jaPendente=*/false, /*duracaoAtual=*/0, /*duracaoNova=*/10));
}

void test_deveAtualizarDuracaoPendente_nova_maior_retorna_verdadeiro(void)
{
    TEST_ASSERT_TRUE(ScheduleExecution::deveAtualizarDuracaoPendente(
        /*jaPendente=*/true, /*duracaoAtual=*/10, /*duracaoNova=*/15));
}

void test_deveAtualizarDuracaoPendente_nova_menor_ou_igual_retorna_falso(void)
{
    TEST_ASSERT_FALSE(ScheduleExecution::deveAtualizarDuracaoPendente(
        /*jaPendente=*/true, /*duracaoAtual=*/10, /*duracaoNova=*/10));
    TEST_ASSERT_FALSE(ScheduleExecution::deveAtualizarDuracaoPendente(
        /*jaPendente=*/true, /*duracaoAtual=*/10, /*duracaoNova=*/5));
}

void test_selecionarLoteMask_menos_pendentes_que_limite_seleciona_todos(void)
{
    uint16_t pendentes = 0b0011;
    uint16_t selecionados = 0;

    int qtd = ScheduleExecution::selecionarLoteMask(pendentes, selecionados, 4, /*limiteSimultaneo=*/2);

    TEST_ASSERT_EQUAL_INT(2, qtd);
    TEST_ASSERT_EQUAL_HEX16(0b0011, selecionados);
    TEST_ASSERT_EQUAL_HEX16(0, pendentes);
}

void test_selecionarLoteMask_mais_pendentes_que_limite_respeita_limite(void)
{
    uint16_t pendentes = 0b1111;
    uint16_t selecionados = 0;

    int qtd = ScheduleExecution::selecionarLoteMask(pendentes, selecionados, 4, /*limiteSimultaneo=*/2);

    TEST_ASSERT_EQUAL_INT(2, qtd);
    TEST_ASSERT_EQUAL_HEX16(0b0011, selecionados);
    // Os que nao entraram no lote continuam pendentes.
    TEST_ASSERT_EQUAL_HEX16(0b1100, pendentes);
}

void test_selecionarLoteMask_sem_pendentes_seleciona_zero(void)
{
    uint16_t pendentes = 0;
    uint16_t selecionados = 0;

    int qtd = ScheduleExecution::selecionarLoteMask(pendentes, selecionados, 4, /*limiteSimultaneo=*/2);

    TEST_ASSERT_EQUAL_INT(0, qtd);
    TEST_ASSERT_EQUAL_HEX16(0, selecionados);
}

void test_selecionarLoteMask_exatamente_no_limite_seleciona_todos(void)
{
    uint16_t pendentes = 0b11;
    uint16_t selecionados = 0;

    int qtd = ScheduleExecution::selecionarLoteMask(pendentes, selecionados, 2, /*limiteSimultaneo=*/2);

    TEST_ASSERT_EQUAL_INT(2, qtd);
    TEST_ASSERT_EQUAL_HEX16(0b11, selecionados);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_selecionarLote_menos_pendentes_que_limite_seleciona_todos);
    RUN_TEST(test_selecionarLote_mais_pendentes_que_limite_respeita_limite);
    RUN_TEST(test_selecionarLote_sem_pendentes_seleciona_zero);
    RUN_TEST(test_selecionarLote_exatamente_no_limite_seleciona_todos);
    RUN_TEST(test_deveAtualizarDuracaoPendente_ainda_nao_pendente_retorna_verdadeiro);
    RUN_TEST(test_deveAtualizarDuracaoPendente_nova_maior_retorna_verdadeiro);
    RUN_TEST(test_deveAtualizarDuracaoPendente_nova_menor_ou_igual_retorna_falso);
    RUN_TEST(test_selecionarLoteMask_menos_pendentes_que_limite_seleciona_todos);
    RUN_TEST(test_selecionarLoteMask_mais_pendentes_que_limite_respeita_limite);
    RUN_TEST(test_selecionarLoteMask_sem_pendentes_seleciona_zero);
    RUN_TEST(test_selecionarLoteMask_exatamente_no_limite_seleciona_todos);
    return UNITY_END();
}
