# Brainstorm: Estilo cards nos submenus de Configurações

**Data:** 2026-07-16
**Contexto:** Menu Principal já migrou pro estilo Pirx (cards empilhados, 2 linhas: selecionado + próximo dimmed). Próximo passo: expandir consistência visual pros 3 submenus de Configurações.

## Escopo

Telas afetadas (todas em `desenharTelaConfig`, `display_manager.cpp`):
1. `EtapaConfiguracao::MENU` — 3 opções (Relógio, Sistema, Voltar)
2. `EtapaConfiguracao::SUBMENU_RELOGIO` — 8 opções (Hora, Minuto, Dia, Mês, Ano, Timeout, Salvar, Voltar), com valores dinâmicos no rótulo
3. `EtapaConfiguracao::SUBMENU_SISTEMA` — 6 opções (Duração padrão, Teste válvulas, Limpar agendas, Restaurar padrão, Info sistema, Voltar), 1 com valor dinâmico

Fora de escopo: telas de edição (EDIT_HORA, EDIT_MINUTO etc), confirmações, TESTE_VALVULAS, INFO_SISTEMA — essas já têm UI própria (não são listas de opções).

## Problema identificado

Menu Principal tinha 5 itens fixos, sem scroll. Os 3 submenus de Config têm até 8 itens e hoje usam janela deslizante de 4 linhas (`inicio = selecionada - 2`, clamp). O padrão 2-cards do Menu Principal (só mostra atual + próximo) esconde profundidade da lista — usuário não sabe se está no item 2 de 8 ou 7 de 8.

## Decisão

Reusar padrão 2-cards (`MenuCards::proximoIndice`, já genérico pra qualquer total) **+ indicador de posição** tipo "N/total" pra compensar a perda de visão geral da janela de 4 linhas.

### Layout por tela

Mesma estrutura visual do Menu Principal (`cardSelY=4, cardSelH=34` / `cardProxY=40, cardProxH=12`), com 2 ajustes:

1. **Cabeçalho ganha contador**: `desenharCabecalho` atual mostra só título (ex: "CONFIG > RELOGIO"). Adicionar posição no canto direito do cabeçalho ou como linha auxiliar — ex: "3/8" — sem quebrar título longo já existente.
2. **Rótulo do card selecionado precisa caber valor dinâmico**: "Hora: 14", "Duracao: 30 min" etc — já passa por `truncarParaLargura` se necessário (helper existente).

Ícones: reusar `desenharIconeSubmenu` já mapeado por opção (linhas 1381, 1439) — mesma tabela, só muda o container visual ao redor.

### Reuso de código

- `MenuCards::proximoIndice(atual, total)` — zero mudança, já testado (5 testes)
- Extrair um helper comum de desenho (`desenharCardsComContador` ou similar) usado pelas 3 telas + Menu Principal, evitando 4 cópias quase-idênticas do mesmo bloco de desenho. Avaliar na fase de plano se vale a pena unificar com `desenharMenuPrincipal` ou manter independente pra não acoplar Menu Principal (sem contador) a Config (com contador).

## Fora de escopo (não mexer agora)

- Sensores, Status, Irrigação, WebServer, Programar — não são listas simples de opções, ficam como estão
- Validação em hardware real — ainda pendente desde o Menu Principal

## Próximo passo

`/s:plan` — quebrar em tasks TDD (se houver lógica pura nova, ex. formatação do contador) + task de desenho.
