#include <unity.h>
#include "schedule_validation.h"

using ScheduleValidation::CampoAgenda;

void setUp(void) {}
void tearDown(void) {}

static CampoAgenda agendaValidaBase()
{
    CampoAgenda a;
    a.hora = 6;
    a.minuto = 30;
    a.duracaoMin = 10;
    a.diasMask = 0b0000001; // domingo
    a.setoresMask = 0b00000001; // setor 1
    return a;
}

void test_hora_invalida_retorna_falso(void)
{
    CampoAgenda a = agendaValidaBase();
    a.hora = 24;
    const char *erro = nullptr;
    TEST_ASSERT_FALSE(ScheduleValidation::validar(a, erro));
    TEST_ASSERT_EQUAL_STRING("hora invalida", erro);
}

void test_minuto_invalido_retorna_falso(void)
{
    CampoAgenda a = agendaValidaBase();
    a.minuto = 60;
    const char *erro = nullptr;
    TEST_ASSERT_FALSE(ScheduleValidation::validar(a, erro));
    TEST_ASSERT_EQUAL_STRING("minuto invalido", erro);
}

void test_duracao_invalida_retorna_falso(void)
{
    CampoAgenda a = agendaValidaBase();
    a.duracaoMin = 0;
    const char *erro = nullptr;
    TEST_ASSERT_FALSE(ScheduleValidation::validar(a, erro));
    TEST_ASSERT_EQUAL_STRING("duracao invalida", erro);
}

void test_sem_dia_selecionado_retorna_falso(void)
{
    CampoAgenda a = agendaValidaBase();
    a.diasMask = 0;
    const char *erro = nullptr;
    TEST_ASSERT_FALSE(ScheduleValidation::validar(a, erro));
    TEST_ASSERT_EQUAL_STRING("selecione ao menos um dia", erro);
}

void test_sem_setor_selecionado_retorna_falso(void)
{
    CampoAgenda a = agendaValidaBase();
    a.setoresMask = 0;
    const char *erro = nullptr;
    TEST_ASSERT_FALSE(ScheduleValidation::validar(a, erro));
    TEST_ASSERT_EQUAL_STRING("selecione ao menos um setor", erro);
}

void test_agenda_valida_retorna_verdadeiro(void)
{
    CampoAgenda a = agendaValidaBase();
    const char *erro = nullptr;
    TEST_ASSERT_TRUE(ScheduleValidation::validar(a, erro));
}

void test_duplicata_exata_retorna_verdadeiro(void)
{
    CampoAgenda candidata = agendaValidaBase();
    CampoAgenda existentes[2] = {agendaValidaBase(), agendaValidaBase()};
    bool ativas[2] = {true, true};

    TEST_ASSERT_TRUE(ScheduleValidation::duplicada(candidata, existentes, ativas, 2, /*slotIgnorado=*/-1));
}

void test_duplicata_ignora_slot_atual(void)
{
    CampoAgenda candidata = agendaValidaBase();
    CampoAgenda outraAgenda = agendaValidaBase();
    outraAgenda.hora = 18; // diferente da candidata, não deve colidir
    CampoAgenda existentes[2] = {agendaValidaBase(), outraAgenda};
    bool ativas[2] = {true, true};

    // Único slot idêntico à candidata é o próprio slot 0 (sendo editado) -> ignorado.
    TEST_ASSERT_FALSE(ScheduleValidation::duplicada(candidata, existentes, ativas, 2, /*slotIgnorado=*/0));
}

void test_duplicata_agenda_inativa_nao_conta(void)
{
    CampoAgenda candidata = agendaValidaBase();
    CampoAgenda existentes[1] = {agendaValidaBase()};
    bool ativas[1] = {false};

    TEST_ASSERT_FALSE(ScheduleValidation::duplicada(candidata, existentes, ativas, 1, /*slotIgnorado=*/-1));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_hora_invalida_retorna_falso);
    RUN_TEST(test_minuto_invalido_retorna_falso);
    RUN_TEST(test_duracao_invalida_retorna_falso);
    RUN_TEST(test_sem_dia_selecionado_retorna_falso);
    RUN_TEST(test_sem_setor_selecionado_retorna_falso);
    RUN_TEST(test_agenda_valida_retorna_verdadeiro);
    RUN_TEST(test_duplicata_exata_retorna_verdadeiro);
    RUN_TEST(test_duplicata_ignora_slot_atual);
    RUN_TEST(test_duplicata_agenda_inativa_nao_conta);
    return UNITY_END();
}
