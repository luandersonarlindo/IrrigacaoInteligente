# Phase 1 Summary: Fundação de Testes

**Status:** complete
**Started:** 2026-07-15
**Completed:** 2026-07-15

## What Was Built
- Harness de teste em host via PlatformIO (`platformio.ini`, ambiente `native`) + Unity, sem alterar o build Arduino IDE/ESP32
- Módulo `schedule_validation.h/.cpp`: lógica pura de validação de agenda (hora, minuto, duração, dias, setores, duplicidade), extraída de `schedule_manager.cpp` e sem dependência de hardware
- `schedule_manager.cpp` atualizado pra delegar `agendaValida`/`duplicada` ao módulo puro, sem duplicar lógica

## Test Coverage
- 2 arquivos de teste: `test/test_smoke/test_smoke.cpp`, `test/test_schedule/test_validation.cpp`
- 10 casos de teste
- Todos passando (`pio test -e native`)

## Key Decisions During Implementation
- PlatformIO + Unity escolhido como test_framework (rodável em host, sem depender de hardware ESP32)
- Nenhum toolchain C++ existia no sistema — instalado PlatformIO (via pip) e MinGW-w64 UCRT (via winget) como pré-requisito de infraestrutura
- Fontes do firmware ficam na raiz do projeto (padrão Arduino IDE), não em `src/`; `platformio.ini` usa `[platformio] src_dir = .` + `build_src_filter` restrito só ao(s) arquivo(s) de lógica pura, pra não puxar `.ino`/drivers de hardware pro build nativo
- Struct própria `CampoAgenda` no módulo de validação, em vez de reusar `AgendaSetor` (que vive em um header com dependência de Arduino/Preferences/RTClib)

## Deviations from Plan
- Build ESP32 real (Arduino IDE) não foi compilado neste ambiente — sem arduino-cli/core ESP32 instalado, fora do escopo desta fase. Verificação foi por revisão manual: a extração preserva exatamente a ordem de checagem e as mensagens de erro do código original.
- Um caso de teste (`test_duplicata_ignora_slot_atual`) tinha bug de setup (duas agendas idênticas no array, ignorar uma ainda batia na outra) — corrigido no teste, não no código de produção, seguindo a regra TDD de não alterar produção pra fazer teste errado passar.

## Files Created/Modified
- `platformio.ini` (novo)
- `test/test_smoke/test_smoke.cpp` (novo)
- `test/test_schedule/test_validation.cpp` (novo)
- `schedule_validation.h` (novo)
- `schedule_validation.cpp` (novo)
- `schedule_manager.cpp` (modificado: `agendaValida`/`duplicada` delegam ao módulo puro)
- `.gitignore` (adicionado `.pio/`)

## Learnings
- Projeto Arduino IDE sem pasta `src/` exige `[platformio] src_dir = .` (não é opção de env) + `build_src_filter` explícito, e `test_build_src = true` pra `pio test` de fato linkar o código de src junto do binário de teste
- Ambiente Windows não tinha nenhum toolchain C++ nem PlatformIO — ambos precisaram ser instalados (pip + winget) antes de qualquer teste nativo rodar
