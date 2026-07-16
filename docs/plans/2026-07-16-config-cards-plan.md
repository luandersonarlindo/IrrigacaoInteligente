# Plano: Cards nos submenus de Configurações

**Brainstorm:** `docs/brainstorms/2026-07-16-config-cards.md`
**Escopo:** `EtapaConfiguracao::MENU`, `SUBMENU_RELOGIO`, `SUBMENU_SISTEMA` em `desenharTelaConfig` (`display_manager.cpp`)

## Abordagem

Sem lógica pura nova — `MenuCards::proximoIndice` já cobre qualquer total, já testado (5 testes). Isto é trabalho de desenho, mesmo padrão da Task 2 do redesign do Menu Principal. Não precisa worktree/subagent (mudança contida em 1 método, arquivo já familiar).

## Task única: extrair helper `desenharCardsOpcao` + aplicar nas 3 etapas

**O quê:**
1. Criar helper privado em `display_manager.cpp` (anonymous namespace ou método `DisplayManager`):
   ```cpp
   void desenharCardsOpcao(const char *rotuloAtual, const char *rotuloProximo,
                            int iconeAtual, int iconeProximo,
                            int posicaoAtual, int totalOpcoes);
   ```
   Reusa geometria do Menu Principal (`cardSelY=4, cardSelH=34, cardProxY=40, cardProxH=12`). Diferença: rodapé mostra `"N/total"` junto da dica, ex: `desenharRodapeDica` recebe string já formatada tipo `"3/8 OK entra"`. Ícone usa `desenharIconeSubmenu` (8x8, já existente) posicionado no canto do card — NÃO redesenhar em 32x32 (fora de escopo, decidido em brainstorm follow-up).

2. Aplicar nas 3 etapas, substituindo o bloco de loop com janela de 4 linhas:
   - `MENU`: 3 opções (Relógio, Sistema, Voltar), ícones já mapeados (0, 2, seta-voltar)
   - `SUBMENU_RELOGIO`: 8 opções, rótulos dinâmicos (Hora/Minuto/Dia/Mês/Ano/Timeout já formatados) — manter o `snprintf` existente por índice, só trocar o desenho
   - `SUBMENU_SISTEMA`: 6 opções, 1 rótulo dinâmico (Duração), ícones via `iconesSubmenuSistema`

3. Ícone do "próximo" item: usar mesma tabela de ícones já existente (`desenharIconeSubmenu` / `iconesSubmenuSistema`), buscando pelo índice de `itemProximo` calculado via `MenuCards::proximoIndice`.

**Onde:** `display_manager.cpp`, dentro de `desenharTelaConfig` (linhas ~1291–1456 na versão atual).

**Testes:** Nenhum teste novo de lógica pura (nada extraído). Validação:
- Compilar nativo não se aplica (código é desenho puro, sem module isolável) — validar por leitura + `g++` só se algo for extraído.
- Rodar suite `test_menu/test_menu_cards.cpp` existente pra garantir `MenuCards::proximoIndice` intocado.
- Checagem manual de overflow de texto (repetir o cuidado do fix `f03fb8a`): rótulos com valor dinâmico (ex: "Ano: 2026") passam por `truncarParaLargura` se necessário.

**Feito quando:**
- [x] Helper `desenharCardsOpcao` criado e usado nas 3 etapas
- [x] Contador "N/total" visível no rodapé de cada etapa
- [x] Ícone do próximo item correto (usa a mesma tabela da etapa, não hardcoded) — nota: ícone do "próximo" não é desenhado (card pequeno é só texto, igual ao Menu Principal); ponto revisado e aprovado
- [x] Nenhuma label estoura os 128px (checagem visual/aritmética como no fix anterior) — `truncarParaLargura` cobre todos os casos, verificado por subagent revisor
- [x] Suite MenuCards passando (5/5)
- [x] Sem duplicação: as 3 etapas + Menu Principal compartilham o máximo de código de desenho razoável

## Revisão

Revisão leve (1 passada, não precisa 8-ângulos como a revisão estrutural anterior) — focar em: overflow de texto, ícone do "próximo" errado, off-by-one no contador N/total.
