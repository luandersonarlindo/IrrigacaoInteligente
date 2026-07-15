# Project State

**Status:** phase-1-complete
**Active Phase:** 2
**Last Updated:** 2026-07-15

## Current Focus
Phase 2: Testes - Persistência e Execução - 2/3 tasks done (Task 2.2 repartida, gerou Task 2.3). Próxima: Task 2.3 (retomada de janela ativa).

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

## Blockers
None.
