# Phase 1 Context

## What this phase is about
Firmware v1 já funciona em campo, mas não tem nenhum teste automatizado. Esta fase cria a primeira rede de segurança: um harness de teste em host (PlatformIO + Unity, ambiente `native`) e cobre as regras de validação de agenda, que são a base pras próximas fases (persistência, execução, migração de modelo).

## Key decisions made
- Abordagem A (testes-primeiro) escolhida no brainstorm de arquitetura do `/s:plan`: construir rede de testes antes de mexer em modelo de agenda ou UX
- Testes rodam em host via PlatformIO `native`, sem alterar o build Arduino IDE/ESP32 existente
- Lógica de negócio pura é extraída para funções testáveis, isolada de hardware (NVS, WiFi, Wire)

## Dependencies
- Nenhuma — primeira fase do roadmap

## Reference
- Architecture approach: Testes-primeiro (safety net) — ver ROADMAP.md
- Requirements: R2, NF2
