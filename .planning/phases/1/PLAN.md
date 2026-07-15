# Phase 1: Fundação de Testes

**Status:** pending
**Started:** -
**Completed:** -

## Goal
Criar infraestrutura de teste automatizado rodável em host (sem hardware ESP32), sem alterar o build Arduino IDE existente, e cobrir as regras de validação de agenda com testes.

## Tasks

### Task 1.1: Configurar ambiente PlatformIO nativo para testes
**Status:** complete

**Description:**
Adicionar um ambiente `native` via PlatformIO com o framework Unity, rodando em host (Windows), separado do build ESP32 via Arduino IDE. O `.ino` e os drivers de hardware continuam intocados; o ambiente `native` serve só pra rodar testes de lógica pura.

**TDD Approach:**
1. Write test: `test/test_smoke/test_smoke.cpp` com um `TEST_ASSERT_TRUE(true)`
2. Run test (should FAIL - RED): harness ainda não existe, comando falha
3. Implement: criar `platformio.ini` com `[env:native]`, `platform = native`, `test_framework = unity`
4. Run test (should PASS - GREEN): `pio test -e native` passa
5. Refactor: nenhum necessário nesta task

**Files to create/modify:**
- `platformio.ini`: novo, ambiente `native` com Unity
- `test/test_smoke/test_smoke.cpp`: novo, teste trivial de sanity check

**Acceptance Criteria:**
- [ ] `pio test -e native` executa sem erro
- [ ] Build ESP32 original (Arduino IDE) não foi alterado

---

### Task 1.2: Isolar e testar regras de validação de agenda
**Status:** complete

**Deviation:** `schedule_manager.h` inclui `Arduino.h`/`Preferences.h`/`RTClib.h`, então a lógica pura foi extraída para um módulo novo (`schedule_validation.h/.cpp`) com struct própria (`CampoAgenda`), em vez de reusar `AgendaSetor` diretamente — evita puxar dependências de hardware pro teste nativo. `schedule_manager.cpp` agora delega pra esse módulo. Build ESP32 real não foi compilado neste ambiente (sem arduino-cli/core ESP32 instalado) — verificado por revisão manual, extração é mecânica e preserva ordem de checagem e mensagens de erro originais.

**Description:**
Extrair a lógica de validação de agenda de `schedule_manager.cpp` (hora 0-23, minuto 0-59, duração >=1, ao menos 1 dia selecionado, ao menos 1 setor selecionado, sem duplicata exata de hora+minuto+diasMask+setoresMask) para uma função pura, sem dependência de `Preferences`/NVS ou hardware, testável em host.

**TDD Approach:**
1. Write test: casos hora inválida (24, -1), minuto inválido (60), duração 0, nenhum dia selecionado, nenhum setor selecionado, duplicata exata de agenda
2. Run test (should FAIL - RED): função ainda não extraída/testável
3. Implement: extrair função `validarAgenda(...)` (ou equivalente) sem dependências de hardware, chamada pelo código existente
4. Run test (should PASS - GREEN): todos os casos passam
5. Refactor: garantir que `schedule_manager.cpp` chama a função extraída em vez de duplicar a lógica

**Files to create/modify:**
- `schedule_manager.h`: declarar função de validação extraída
- `schedule_manager.cpp`: extrair lógica, usar a função extraída no fluxo existente
- `test/test_schedule/test_validation.cpp`: novo, casos de validação

**Acceptance Criteria:**
- [ ] Todos os casos de validação listados têm teste cobrindo
- [ ] `pio test -e native` passa com os novos testes
- [ ] Comportamento do firmware ESP32 real não muda (mesma validação, código reorganizado)

---
