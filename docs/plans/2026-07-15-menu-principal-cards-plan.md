# Menu Principal — Cards Empilhados Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Substituir o carrossel horizontal de ícones do Menu Principal por um layout de 2 cards empilhados (selecionado em destaque + próximo item dimmed), com navegação circular via encoder.

**Architecture:** A lógica pura de "qual índice mostrar em cada linha, dado o índice selecionado e o total de itens, com wrap circular" é extraída para uma função testável em `menu_cards.h/.cpp` (sem dependência de hardware, seguindo o padrão já usado por `schedule_execution`/`sensor_historico`). `DisplayManager::desenharMenuPrincipal()` chama essa função e desenha os dois cards usando as primitivas já existentes do `DisplayDriverOled`. `MenuController` não muda.

**Tech Stack:** C++ (Arduino/ESP32), U8g2 (via `DisplayDriverOled`), PlatformIO + Unity para testes nativos (host).

## Global Constraints

- Tela: 128x64 px, monocromático (`OLED_LARGURA=128`, `OLED_ALTURA=64` em `Config.h`).
- Wrap circular obrigatório nas pontas (spec: `docs/brainstorms/2026-07-15-menu-principal-cards.md`).
- Troca instantânea, sem animação de frames.
- Card inferior (próximo item) não desenha ícone — só texto (decisão registrada na spec).
- `MenuController` não muda: usa apenas `itemSelecionado()`, `totalItens()`, `nomeItem(int)` já existentes.
- Testes de lógica pura rodam via: `g++ -std=gnu++17 -I .pio/libdeps/native/Unity/src -I . .pio/libdeps/native/Unity/src/unity.c menu_cards.cpp test/test_menu/test_menu_cards.cpp -o t.exe && ./t.exe`
- `platformio.ini`'s `build_src_filter` deve incluir `menu_cards.cpp` para o build nativo de teste.

---

### Task 1: Lógica pura de seleção de cards (com wrap circular)

**Files:**
- Create: `menu_cards.h`
- Create: `menu_cards.cpp`
- Test: `test/test_menu/test_menu_cards.cpp`
- Modify: `platformio.ini:13`

**Interfaces:**
- Consumes: nada (função pura, sem dependências externas).
- Produces: `MenuCards::proximoIndice(int atual, int total)` — usado pela Task 2 (`DisplayManager::desenharMenuPrincipal`) para saber qual índice desenhar na linha inferior (dimmed).

- [ ] **Step 1: Criar o diretório de teste e escrever o teste que falha**

Crie o arquivo `test/test_menu/test_menu_cards.cpp`:

```cpp
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
```

- [ ] **Step 2: Criar o header vazio para o teste compilar (mas falhar por falta de implementação)**

Crie `menu_cards.h`:

```cpp
#pragma once

// Lógica pura do layout de cards do Menu Principal (seleção + próximo item
// com wrap circular). Sem dependência de hardware — testável em host.
namespace MenuCards
{

// Retorna o índice do item a mostrar na linha inferior (dimmed), dado o
// índice atualmente selecionado e o total de itens do menu. Faz wrap
// circular: se `atual` for o último item, retorna 0.
int proximoIndice(int atual, int total);

} // namespace MenuCards
```

Crie `menu_cards.cpp` vazio (só o include, sem implementação ainda):

```cpp
#include "menu_cards.h"

namespace MenuCards
{

} // namespace MenuCards
```

- [ ] **Step 3: Adicionar `menu_cards.cpp` ao build nativo**

Edite `platformio.ini`, linha 13 — adicione `+<menu_cards.cpp>` à lista existente:

```ini
build_src_filter = -<*> +<schedule_validation.cpp> +<schedule_persistence.cpp> +<schedule_execution.cpp> +<sensor_historico.cpp> +<menu_cards.cpp>
```

- [ ] **Step 4: Rodar o teste e confirmar que falha (RED)**

Run: `g++ -std=gnu++17 -I .pio/libdeps/native/Unity/src -I . .pio/libdeps/native/Unity/src/unity.c menu_cards.cpp test/test_menu/test_menu_cards.cpp -o test_menu_cards.exe`

Expected: erro de linkagem — `undefined reference to MenuCards::proximoIndice(int, int)` (a função ainda não existe, só a declaração).

- [ ] **Step 5: Implementar `proximoIndice`**

Edite `menu_cards.cpp`:

```cpp
#include "menu_cards.h"

namespace MenuCards
{

int proximoIndice(int atual, int total)
{
    if (total <= 1)
    {
        return atual;
    }
    return (atual + 1) % total;
}

} // namespace MenuCards
```

- [ ] **Step 6: Rodar o teste e confirmar que passa (GREEN)**

Run: `g++ -std=gnu++17 -I .pio/libdeps/native/Unity/src -I . .pio/libdeps/native/Unity/src/unity.c menu_cards.cpp test/test_menu/test_menu_cards.cpp -o test_menu_cards.exe && ./test_menu_cards.exe`

Expected:
```
5 Tests 0 Failures 0 Ignored
OK
```

- [ ] **Step 7: Limpar o binário de teste e commitar**

```bash
rm -f test_menu_cards.exe
git add menu_cards.h menu_cards.cpp test/test_menu/test_menu_cards.cpp platformio.ini
git commit -m "test(menu): add MenuCards::proximoIndice with circular wrap"
```

---

### Task 2: Redesenhar `desenharMenuPrincipal` com o layout de 2 cards

**Files:**
- Modify: `display_manager.cpp:566-599` (função `desenharMenuPrincipal`)
- Modify: `display_manager.h` (nenhuma mudança de assinatura pública necessária — `desenharMenuPrincipal()` já é privado e sem parâmetros)

**Interfaces:**
- Consumes: `MenuCards::proximoIndice(int, int)` (Task 1); `MenuController::itemSelecionado()`, `MenuController::totalItens()`, `MenuController::nomeItem(int)` (já existentes, sem mudança); `DisplayDriverOled::desenharRetangulo`, `desenharTexto`, `desenharTextoGrande`, `larguraTexto` (já existentes).
- Produces: nada consumido por tasks futuras — esta é a última task do plano.

- [ ] **Step 1: Ler a função atual para confirmar o texto exato a substituir**

Abra `display_manager.cpp` e confirme que a função `desenharMenuPrincipal()` (linhas 566-599) é exatamente:

```cpp
void DisplayManager::desenharMenuPrincipal()
{
    const int iconW = 32;
    const int iconGap = 6;
    const int iconY = 8;
    const int centerX = (OLED_LARGURA - iconW) / 2;

    int totalItens = _menu.totalItens();
    int itemAtual = _menu.itemSelecionado();

    int x = centerX - (itemAtual * (iconW + iconGap));
    for (int i = 0; i < totalItens; i++)
    {
        if (x >= 0 && (x + iconW) <= OLED_LARGURA)
        {
            desenharIconeMenuPrincipal(_display, x, iconY, i);
        }
        x += iconW + iconGap;
    }

    // Moldura tripla no item selecionado para reforcar foco
    _display.desenharRetangulo(centerX - 1, iconY - 1, iconW + 2, iconW + 2);
    _display.desenharRetangulo(centerX - 2, iconY - 2, iconW + 4, iconW + 4);
    _display.desenharRetangulo(centerX - 3, iconY - 3, iconW + 6, iconW + 6);

    // Rotulo centralizado no rodape
    const char *rotulo = _menu.nomeItem(itemAtual);
    int larguraRotulo = _display.larguraTexto(rotulo);
    int xRotulo = (OLED_LARGURA - larguraRotulo) / 2;
    if (xRotulo < 0)
        xRotulo = 0;

    _display.desenharTexto(xRotulo, 54, rotulo);
}
```

Se o texto não bater exatamente (por causa de edições anteriores nesta sessão), localize a função pelo nome `DisplayManager::desenharMenuPrincipal` e use o conteúdo atual como base para o Step 2.

- [ ] **Step 2: Substituir pela nova implementação em cards**

Troque o corpo inteiro da função por:

```cpp
void DisplayManager::desenharMenuPrincipal()
{
    int totalItens = _menu.totalItens();
    int itemAtual = _menu.itemSelecionado();
    int itemProximo = MenuCards::proximoIndice(itemAtual, totalItens);

    // --- Card superior: item selecionado (em destaque) ---
    const int cardSelY = 4;
    const int cardSelH = 34;
    const int iconX = 2;
    const int iconY = cardSelY + 1;

    _display.desenharRetangulo(0, cardSelY, OLED_LARGURA, cardSelH);
    _display.desenharRetangulo(1, cardSelY + 1, OLED_LARGURA - 2, cardSelH - 2);

    desenharIconeMenuPrincipal(_display, iconX, iconY, itemAtual);

    const char *rotuloSelecionado = _menu.nomeItem(itemAtual);
    _display.desenharTextoGrande(40, cardSelY + 8, rotuloSelecionado);

    // Seta indicando "OK confirma"
    const int setaX = OLED_LARGURA - 10;
    const int setaY = cardSelY + (cardSelH / 2);
    _display.desenharLinha(setaX, setaY - 4, setaX + 4, setaY);
    _display.desenharLinha(setaX + 4, setaY, setaX, setaY + 4);

    // --- Card inferior: próximo item (dimmed, só texto) ---
    const int cardProxY = cardSelY + cardSelH + 2;
    const int cardProxH = 12;

    _display.desenharRetangulo(0, cardProxY, OLED_LARGURA, cardProxH);

    const char *rotuloProximo = _menu.nomeItem(itemProximo);
    _display.desenharTexto(6, cardProxY + 2, rotuloProximo);

    desenharRodapeDica("Gire | OK confirma");
}
```

- [ ] **Step 3: Adicionar o include de `menu_cards.h` em `display_manager.h`**

Em `display_manager.h`, localize o bloco de includes (linhas 15-22) e adicione a nova linha após `#include "menu_controller.h"`:

```cpp
#include "menu_controller.h"
#include "menu_cards.h"
```

- [ ] **Step 4: Compilar o firmware completo no Arduino IDE**

Abra o projeto no Arduino IDE (ou use a ferramenta de build habitual) e compile. Não há build automatizado para o firmware completo neste ambiente (depende de bibliotecas de hardware — U8g2, RTClib, ESP32Encoder — não disponíveis no build nativo `platformio.ini`).

Expected: compilação sem erros. Se houver erro de símbolo não encontrado para `MenuCards::proximoIndice`, confirme que `menu_cards.cpp` está no mesmo diretório do sketch (raiz do projeto) — o Arduino IDE compila automaticamente todos os `.cpp` da pasta do sketch, não precisa de configuração adicional.

- [ ] **Step 5: Testar no hardware ou revisar visualmente a lógica**

Se tiver o ESP32 conectado: grave o firmware, entre no Menu Principal (gire o encoder a partir da tela de Status) e confira:
- Card superior mostra o item selecionado com ícone, texto grande e seta à direita.
- Card inferior mostra o nome do próximo item, sem ícone, mais discreto (moldura fina).
- Girar horário avança para o próximo item; girar no último item (Configurações) volta para o primeiro (Irrigar Agora) — wrap circular.
- Girar anti-horário no primeiro item (Irrigar Agora) volta para o último (Configurações).

Se não tiver hardware disponível agora: revise o código do Step 2 manualmente contra a spec (`docs/brainstorms/2026-07-15-menu-principal-cards.md`) e confirme que `MenuCards::proximoIndice` (testado na Task 1) cobre a lógica de wrap corretamente — a parte visual (coordenadas, texto) não tem como ser testada fora do hardware real, mas a lógica de qual índice mostrar já está coberta por teste.

- [ ] **Step 6: Commitar**

```bash
git add display_manager.h display_manager.cpp
git commit -m "feat(display): redesign menu principal as stacked cards"
```

---

## Self-Review Notes

- **Spec coverage:** card superior com ícone/texto grande/seta (Task 2 Step 2) ✓; card inferior sem ícone, só texto dimmed (Task 2 Step 2) ✓; wrap circular (Task 1, testado) ✓; troca instantânea sem animação (nenhum frame loop adicionado) ✓; `MenuController` inalterado (nenhuma task o modifica) ✓.
- **Fora de escopo confirmado:** nenhuma mudança nas outras 6 telas; nenhuma animação de slide.
- **Risco conhecido:** o layout exato (posições x/y) do Step 2 da Task 2 não foi validado em hardware real antes deste plano — é a melhor estimativa a partir das dimensões de fonte conhecidas (`desenharTextoGrande` = `u8g2_font_10x20_tf`, 20px de altura). Pequenos ajustes de posição podem ser necessários após ver no display físico; isso é esperado e não invalida a estrutura do código.
