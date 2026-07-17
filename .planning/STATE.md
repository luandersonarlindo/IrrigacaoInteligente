# Project State

**Status:** phase-1-complete
**Active Phase:** 2
**Last Updated:** 2026-07-16

## Current Focus
Phase 2: Testes - Persistência e Execução - 2/3 tasks done (Task 2.2 repartida, gerou Task 2.3). Próxima: Task 2.3 (retomada de janela ativa).

Trabalho paralelo (fora do roadmap, 2026-07-15/16): histórico de sensores + gráfico na tela Sensores, correções da revisão estrutural, redesign do Menu Principal em cards empilhados, expansão do estilo cards pros 3 submenus de Configurações, e ajuste pós-validação em hardware real (ícones Lucide 28x28, cabeçalho removido dos cards) — tudo mergeado ou pronto pra commit no main. Pendente: validação visual do ajuste de ícones em hardware real (ainda não visto fisicamente).

## Decisions
| Date | Decision | Rationale |
|------|----------|-----------|
| 2026-07-15 | Project initialized | Starting new project with claude-stack |
| 2026-07-15 | Requirements defined | 9 functional + 5 non-functional requirements, documenting existing v1 firmware (no code change) |
| 2026-07-15 | Architecture: Testes-primeiro (safety net) | TDD é regra do plugin; rede de testes antes de mexer em modelo de agenda (schema NVS) ou UX reduz risco de regressão em sistema já em campo |
| 2026-07-15 | Roadmap created | 6 phases, 11 total tasks |
| 2026-07-15 | Task 1.1 complete | PlatformIO `native` + Unity instalado (via pip) e MinGW-w64 UCRT (via winget) instalado no sistema — nenhum toolchain C++ existia antes; harness roda 1 teste, build ESP32/Arduino IDE intocado |
| 2026-07-15 | Task 1.2 complete | Validação de agenda extraída pra `schedule_validation.h/.cpp`, 9 testes novos; `schedule_manager.cpp` delega pra lá |
| 2026-07-15 | Phase 1 complete | 2 tasks, 10 tests, todos passando |
| 2026-07-15 | Task 2.1 complete | CRC16 + checagem versão extraída pra `schedule_persistence.h/.cpp`, 5 testes novos; removeu duplicação (`calcularCrc16` usado em 4 pontos, agora 1 só) |
| 2026-07-15 | Task 2.2 repartida em 2.2+2.3 | Escopo original (motor de lotes completo) misturava 3 preocupações; usuário aprovou fazer seleção de lote + conflito de duração agora, adiar retomada de janela (maior, ~150 linhas de aritmética de época, sem toolchain ESP32 pra validar) pra Task 2.3 |
| 2026-07-15 | Task 2.2 complete (escopo parcial) | Seleção de lote + conflito de duração extraídos pra `schedule_execution.h/.cpp`, 7 testes novos; `IrrigacaoInteligente.ino` delega pra lá |
| 2026-07-15 | Histórico de sensores + gráfico | `sensor_historico.h/.cpp` (buffer circular 24 pontos, 10 testes) integrado ao Dht11Driver; mini-gráfico de temperatura (~4h) na tela Sensores |
| 2026-07-15 | Revisão estrutural completa aplicada | 8 ângulos paralelos + verificação: bug do eixo X do gráfico (CONFIRMED), staleness em obterProximaExecucao, bounds check, contarBitsSetor/contarAbertas* compartilhados, AgendaExecutor extraído do main.ino (bitmask), rodapé unificado |
| 2026-07-16 | Menu Principal em cards empilhados | Layout B (2 linhas: selecionado grande + próximo dimmed), wrap circular via `MenuCards::proximoIndice` (5 testes TDD); spec em docs/brainstorms, plano em docs/plans; fix de overflow de label (fonte 6px) pego na revisão final |
| 2026-07-16 | Cards nos submenus de Configurações | Helper `DisplayManager::desenharCardsOpcao` reusa `MenuCards::proximoIndice` + geometria do Menu Principal, com contador "N/total" no rodapé (submenus têm até 8 opções, sem scroll de janela); ícones mantidos em 8x8 (não redesenhados em 32x32, fora de escopo); revisão leve aprovada sem findings |
| 2026-07-16 | Ajuste pós-hardware: ícones Lucide 28x28 + sem cabeçalho | Fotos do display físico mostraram cabeçalho sobrepondo card e ícone 8x8 ilegível; usuário indicou lucide.dev. Baixados 13 SVGs reais via unpkg (lucide-static), rasterizados 1bpp 28x28 com @resvg/resvg-js (script fora do repo); `desenharCardsOpcao` passou a receber `const uint8_t*` (bitmap) em vez de índice int; `desenharIconeOuVoltar` removida (ícone "Voltar" agora é bitmap corner-up-left); cabeçalho removido só nos 3 blocos de opções (telas de animação de feedback mantidas intactas) |
| 2026-07-16 | Extensão: SUBMENU_AGENDA em cards + animações viram ícone estático + rename Menu Principal | `EtapaProgramacao::SUBMENU_AGENDA` (Programar > Editar Agenda) migrado pro mesmo padrão `desenharCardsOpcao`, reusando ícones Lucide já existentes (clock/timer/hourglass/calendar-days/droplets/save/trash-2/corner-up-left); `desenharIconeSubmenu` (8x8, só usada ali) removida. Animações `desenharAnimacaoCheck48`/`Lixeira48`/`desenharSegmentoParcial` (~110 linhas, frames por tempo) substituídas por helper único `desenharFeedbackIcone` (ícone Lucide 28x28 estático, 1200ms fixo) usado nos 4 pontos de feedback (Programar salvo/excluído, Config salvo/limpo/restaurado) — reduz flash e simplifica lógica. `menu_controller.cpp`: "Irrigar Agora"→"Irrigar", "WEBSERVER"→"Web Server" no Menu Principal (cabeçalho da tela WebServer intocado) |
| 2026-07-16 | Rename "Programar"→"Agendas" no Menu Principal | 1 linha em `menu_controller.cpp`; cabeçalhos internos ("PROGRAMAR", "EDITAR AGENDA") ficam como estão |
| 2026-07-16 | Redesign dashboard Status: remove ON+ícone irrigação redundante, adiciona status WiFi | Bloco "ON N/N" + ícone torneira (canto sup. direito) removido — já repetido em palavras na linha abaixo. Substituído por ícone WiFi (Lucide `wifi`/`wifi-off` 12x12, via `_webAp.staConectada()`, já existia — sem API nova). Ícones relógio e próxima-agenda trocados de custom 10x10 pra Lucide 14x14 (`clock`, `calendar-clock`); `desenharIconeStatusRelogio/Irrigacao/Agenda` removidas (órfãs). Escopo confirmado com usuário: sem RSSI/força de sinal (só 2 estados), sem ícone de irrigação separado (texto já cobre) |
| 2026-07-16 | Ajustes finos pós-hardware no dashboard Status | Fotos do display físico confirmaram funcionamento; 3 ajustes: (1) ícone WiFi trocado de `wifi` (4 arcos, poluído em 12px) pra `wifi-high` (3 arcos, mais legível); (2) removida contagem "S%d/%d" das linhas MANUAL/AUTO/ATIVO (só texto, sem número de setores — já redundante com dashboard web); (3) rodapé "Gire para abrir menu" removido, bloco "Próxima Agenda" expandido de 20px pra 35px de altura (y=29 até y=64, borda exata do display), ícone+textos reposicionados/centralizados com mais respiro |
| 2026-07-16 | Reverte ícone WiFi + adiciona screensaver do dashboard | Usuário viu no hardware que `wifi-high` (3 arcos) tinha ficado invertido com a intenção — revertido pro `wifi` original (4 arcos), byte-a-byte idêntico ao commit 342527e. Novo recurso: display apaga (via `setPowerSave` do U8g2, comando I2C, não corta alimentação) após 60s sem interação, só quando parado no dashboard (`EstadoMenu::STATUS && !menuAtivo()` — não afeta navegação em outras telas). Novo getter `MenuController::msDesdeUltimaInteracao()` reusa o `_ultimoEventoMenuMs` que já existia pro timeout de 30s do menu; religa assim que detecta qualquer interação |

## Blockers
None.
