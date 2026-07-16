#include <unity.h>
#include "sensor_historico.h"

using SensorHistorico::Buffer;

void setUp(void) {}
void tearDown(void) {}

void test_buffer_novo_esta_vazio(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);
    TEST_ASSERT_EQUAL_INT(0, SensorHistorico::total(buf));
}

void test_inserir_um_ponto_incrementa_total(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);
    SensorHistorico::inserir(buf, 25.5f, 60.0f);
    TEST_ASSERT_EQUAL_INT(1, SensorHistorico::total(buf));
}

void test_obter_retorna_valores_inseridos_na_ordem(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);
    SensorHistorico::inserir(buf, 20.0f, 50.0f);
    SensorHistorico::inserir(buf, 21.0f, 51.0f);

    float temp, umid;
    SensorHistorico::obter(buf, 0, temp, umid);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, temp);
    TEST_ASSERT_EQUAL_FLOAT(50.0f, umid);

    SensorHistorico::obter(buf, 1, temp, umid);
    TEST_ASSERT_EQUAL_FLOAT(21.0f, temp);
    TEST_ASSERT_EQUAL_FLOAT(51.0f, umid);
}

void test_buffer_nao_ultrapassa_capacidade_maxima(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);
    for (int i = 0; i < SensorHistorico::CAPACIDADE + 5; i++)
    {
        SensorHistorico::inserir(buf, (float)i, (float)i);
    }
    TEST_ASSERT_EQUAL_INT(SensorHistorico::CAPACIDADE, SensorHistorico::total(buf));
}

void test_buffer_cheio_descarta_ponto_mais_antigo(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);
    for (int i = 0; i < SensorHistorico::CAPACIDADE; i++)
    {
        SensorHistorico::inserir(buf, (float)i, 0.0f);
    }
    // Insere mais um: o ponto 0 (mais antigo) deve sair, ponto 1 vira o mais antigo.
    SensorHistorico::inserir(buf, 999.0f, 0.0f);

    float temp, umid;
    SensorHistorico::obter(buf, 0, temp, umid);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, temp);

    SensorHistorico::obter(buf, SensorHistorico::CAPACIDADE - 1, temp, umid);
    TEST_ASSERT_EQUAL_FLOAT(999.0f, temp);
}

void test_minimo_maximo_temperatura_no_buffer(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);
    SensorHistorico::inserir(buf, 18.0f, 40.0f);
    SensorHistorico::inserir(buf, 30.0f, 70.0f);
    SensorHistorico::inserir(buf, 22.0f, 55.0f);

    float minTemp, maxTemp, minUmid, maxUmid;
    SensorHistorico::faixaTemperatura(buf, minTemp, maxTemp);
    SensorHistorico::faixaUmidade(buf, minUmid, maxUmid);

    TEST_ASSERT_EQUAL_FLOAT(18.0f, minTemp);
    TEST_ASSERT_EQUAL_FLOAT(30.0f, maxTemp);
    TEST_ASSERT_EQUAL_FLOAT(40.0f, minUmid);
    TEST_ASSERT_EQUAL_FLOAT(70.0f, maxUmid);
}

void test_faixa_com_buffer_vazio_retorna_min_igual_max_zero(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);

    float minTemp, maxTemp;
    SensorHistorico::faixaTemperatura(buf, minTemp, maxTemp);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, minTemp);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, maxTemp);
}

void test_obter_com_indice_negativo_retorna_ponto_mais_antigo(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);
    SensorHistorico::inserir(buf, 20.0f, 50.0f);
    SensorHistorico::inserir(buf, 21.0f, 51.0f);

    float temp, umid;
    SensorHistorico::obter(buf, -1, temp, umid);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, temp);
}

void test_obter_com_indice_alem_do_total_retorna_ponto_mais_recente(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);
    SensorHistorico::inserir(buf, 20.0f, 50.0f);
    SensorHistorico::inserir(buf, 21.0f, 51.0f);

    float temp, umid;
    SensorHistorico::obter(buf, 99, temp, umid);
    TEST_ASSERT_EQUAL_FLOAT(21.0f, temp);
}

void test_obter_em_buffer_vazio_nao_le_lixo(void)
{
    Buffer buf;
    SensorHistorico::iniciar(buf);

    float temp = -999.0f, umid = -999.0f;
    SensorHistorico::obter(buf, 0, temp, umid);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, temp);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, umid);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_buffer_novo_esta_vazio);
    RUN_TEST(test_inserir_um_ponto_incrementa_total);
    RUN_TEST(test_obter_retorna_valores_inseridos_na_ordem);
    RUN_TEST(test_buffer_nao_ultrapassa_capacidade_maxima);
    RUN_TEST(test_buffer_cheio_descarta_ponto_mais_antigo);
    RUN_TEST(test_minimo_maximo_temperatura_no_buffer);
    RUN_TEST(test_faixa_com_buffer_vazio_retorna_min_igual_max_zero);
    RUN_TEST(test_obter_com_indice_negativo_retorna_ponto_mais_antigo);
    RUN_TEST(test_obter_com_indice_alem_do_total_retorna_ponto_mais_recente);
    RUN_TEST(test_obter_em_buffer_vazio_nao_le_lixo);
    return UNITY_END();
}
