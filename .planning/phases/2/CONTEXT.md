# Phase 2 Context

## What this phase is about
Fase 1 criou o harness de teste e cobriu validação de campo único de agenda. Fase 2 estende a rede de segurança pra duas áreas mais arriscadas: persistência (versão+CRC, o que acontece com dado corrompido) e o motor de execução por lotes (fila, conflito de setor, retomada após reboot). Ambas envolvem estado/tempo, então a extração pra função pura exige mais cuidado que a Fase 1.

## Key decisions made
- Mesma abordagem da Fase 1: extrair lógica de decisão pura, sem tocar em `Preferences`/`millis()`/hardware, pra rodar no ambiente `native`
- Se a lógica de persistência não couber bem em `schedule_validation`, criar módulo novo dedicado (nome sugerido: `schedule_persistence`) — decisão final durante a Task 2.1
- Lógica de execução por lotes pode ficar em módulo novo `schedule_execution`, recebendo tempo/estado como parâmetro em vez de ler `millis()` direto

## Dependencies
- Fase 1 completa: harness `pio test -e native` funcionando, padrão de extração de lógica pura já estabelecido em `schedule_validation.h/.cpp`

## Reference
- Architecture approach: Testes-primeiro (safety net) — ver ROADMAP.md
- Requirements: R3, R6, NF1
