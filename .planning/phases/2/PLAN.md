# Phase 2: Testes - Persistência e Execução

**Status:** pending
**Started:** -
**Completed:** -

## Goal
Testar lógica de persistência (versão+CRC) e motor de execução sequencial por lotes sem depender de hardware real (Preferences/NVS, timing físico).

## Tasks

### Task 2.1: Testar validação de versão+CRC do banco de agendas
**Status:** pending

**Description:**
Extrair a checagem de versão/CRC do banco de agendas para uma função pura testável, isolada de `Preferences`. No boot, se a versão ou o CRC forem inválidos, o banco é reinicializado com o padrão seguro — essa decisão (aceitar vs. reinicializar) é o que deve virar lógica pura testável.

**TDD Approach:**
1. Write test: CRC calculado bate com dado válido -> aceita; CRC não bate (dado corrompido) -> decide reinicializar; versão diferente da esperada -> decide reinicializar
2. Run test (should FAIL - RED): função ainda não existe/testável
3. Implement: extrair função de decisão (ex.: `bancoValido(versaoLida, versaoEsperada, crcLido, crcCalculado)`) sem dependência de `Preferences`
4. Run test (should PASS - GREEN): casos cobertos
5. Refactor: `schedule_manager.cpp` usa a função extraída em `carregarBanco()`

**Files to create/modify:**
- `schedule_validation.h` / `.cpp` (ou novo módulo `schedule_persistence.h/.cpp` se a lógica não couber bem no módulo de validação): função pura de decisão versão/CRC
- `schedule_manager.cpp`: usa a função extraída
- `test/test_schedule/test_persistence.cpp`: novo, casos válido/corrompido/versão divergente

**Acceptance Criteria:**
- [ ] Caso versão+CRC válidos: função indica banco válido
- [ ] Caso CRC inválido: função indica reinicializar
- [ ] Caso versão diferente da esperada: função indica reinicializar
- [ ] `pio test -e native` passa com os novos testes

---

### Task 2.2: Testar motor de execução sequencial por lotes
**Status:** pending

**Description:**
Testar a lógica de fila/lote do motor de execução: respeita `MAX_SETOR_SIMULTANEOS_AGENDA`, em conflito no mesmo setor mantém a maior duração, e retoma janela ativa com tempo remanescente após reboot. Essa lógica vive hoje em `irrigation_controller`/`schedule_manager` misturada com estado de hardware (relés, `millis()`); extrair a parte de decisão pura (dado um conjunto de setores disparados e o estado atual da fila, o que deve acontecer) para função testável.

**TDD Approach:**
1. Write test: fila com mais setores que `MAX_SETOR_SIMULTANEOS_AGENDA` -> só os N primeiros entram no lote atual, resto fica pendente; dois disparos no mesmo setor com durações diferentes -> mantém a maior; retomada com tempo remanescente conhecido -> não reinicia do zero
2. Run test (should FAIL - RED): função ainda não extraída/testável
3. Implement: extrair função pura de decisão de lote (sem `millis()`/hardware, recebendo tempo/estado como parâmetro)
4. Run test (should PASS - GREEN): casos cobertos
5. Refactor: código de produção usa a função extraída

**Files to create/modify:**
- Novo módulo de lógica pura (ex.: `schedule_execution.h/.cpp`)
- `irrigation_controller.cpp` e/ou `schedule_manager.cpp`: usa a função extraída
- `test/test_schedule/test_execution.cpp`: novo, casos de fila/conflito/retomada

**Acceptance Criteria:**
- [ ] Limite de setores simultâneos respeitado no teste
- [ ] Conflito de duração no mesmo setor mantém a maior
- [ ] Retomada de janela ativa com tempo remanescente coberta
- [ ] `pio test -e native` passa com os novos testes

---
