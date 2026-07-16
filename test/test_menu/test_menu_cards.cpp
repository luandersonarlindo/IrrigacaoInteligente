#include <unity.h>
#include "menu_cards.h"

void setUp(void) {}
void tearDown(void) {}

void test_proximoIndice_no_meio_da_lista_retorna_indice_seguinte(void)
{
    // 5 itens (0..4), selecionado = 1 -> próximo = 2
    TEST_ASSERT_EQUAL_INT(2, MenuCards::proximoIndice(1, 5));
}

void test_proximoIndice_no_primeiro_item_retorna_segundo(void)
{
    TEST_ASSERT_EQUAL_INT(1, MenuCards::proximoIndice(0, 5));
}

void test_proximoIndice_no_ultimo_item_faz_wrap_para_o_primeiro(void)
{
    // 5 itens (0..4), selecionado = 4 (último) -> próximo = 0 (wrap)
    TEST_ASSERT_EQUAL_INT(0, MenuCards::proximoIndice(4, 5));
}

void test_proximoIndice_com_dois_itens_alterna_entre_os_dois(void)
{
    TEST_ASSERT_EQUAL_INT(1, MenuCards::proximoIndice(0, 2));
    TEST_ASSERT_EQUAL_INT(0, MenuCards::proximoIndice(1, 2));
}

void test_proximoIndice_com_um_item_retorna_o_mesmo_item(void)
{
    TEST_ASSERT_EQUAL_INT(0, MenuCards::proximoIndice(0, 1));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_proximoIndice_no_meio_da_lista_retorna_indice_seguinte);
    RUN_TEST(test_proximoIndice_no_primeiro_item_retorna_segundo);
    RUN_TEST(test_proximoIndice_no_ultimo_item_faz_wrap_para_o_primeiro);
    RUN_TEST(test_proximoIndice_com_dois_itens_alterna_entre_os_dois);
    RUN_TEST(test_proximoIndice_com_um_item_retorna_o_mesmo_item);
    return UNITY_END();
}
