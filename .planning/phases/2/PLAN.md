# Phase 2: Testes - Persistência e Execução

**Status:** pending
**Started:** -
**Completed:** -

## Goal
Testar lógica de persistência (versão+CRC) e motor de execução sequencial por lotes sem depender de hardware real (Preferences/NVS, timing físico).

## Tasks

### Task 2.1: Testar validação de versão+CRC do banco de agendas
**Status:** complete

**Deviation:** Novo módulo `schedule_persistence.h/.cpp` (não reaproveitou `schedule_validation`, escopo diferente). `calcularCrc16` era usado em 4 pontos (banco de agendas + cache de execução diária) — todos migrados pra `SchedulePersistence::crc16`, método privado antigo removido (sem duplicação). Checagem de versão do cache de execução diária, que antes era um `if` separado, foi absorvida por `bancoValido` (mesmo comportamento, menos duplicação).

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

### Task 2.2: Testar motor de execução sequencial por lotes (escopo parcial)
**Status:** complete

**Deviation:** Escopo original era grande demais pra uma task só (misturava 3 preocupações distintas). Repartido, com aprovação do usuário, em:
- Task 2.2 (esta): seleção de setores pro lote atual (respeita `MAX_SETOR_SIMULTANEOS_AGENDA`) + regra de conflito de duração (mantém a maior) — extraído pra `schedule_execution.h/.cpp`, ligado em `IrrigacaoInteligente.ino` (`iniciarProximoLoteAgenda`, `enfileirarDisparosAgenda`)
- Task 2.3 (nova): retomada de janela ativa com tempo remanescente — adiada, ver abaixo

Build ESP32 real não compilado neste ambiente (sem arduino-cli/core ESP32) — verificado por revisão manual.

**Acceptance Criteria:**
- [x] Limite de setores simultâneos respeitado no teste
- [x] Conflito de duração no mesmo setor mantém a maior
- [x] `pio test -e native` passa com os novos testes

---

### Task 2.3: Testar retomada de janela ativa com tempo remanescente
**Status:** pending

**Description:**
`ScheduleManager::avaliarDisparos` (schedule_manager.cpp, ~150 linhas) calcula em que lote uma agenda deveria estar a partir do horário de início, incluindo retomada após reboot com tempo remanescente do lote em execução. Usa `DateTime` (RTClib) pra `agora` e aritmética de época em segundos/minutos. Extrair a decisão pura (dado `agoraEpoch`, `inicioEpoch`, `duracaoMin`, `totalSetores`, `limiteSimultaneo`, `intervaloLoteMs` → qual lote está ativo, quanto tempo falta, se a janela já encerrou) pra função testável, convertendo `DateTime` pra epoch inteiro na borda (quem chama already tem `agora.unixtime()`).

**TDD Approach:**
1. Write test: dentro do primeiro lote (tempo restante correto); no intervalo entre lotes (aponta pro próximo lote); dentro de um lote intermediário após reboot (retoma com tempo remanescente); depois do fim da janela total (não dispara mais); antes do horário de início (não dispara ainda)
2. Run test (should FAIL - RED): função ainda não extraída/testável
3. Implement: extrair função pura de janela/lote, operando só com inteiros (segundos/minutos), sem `DateTime`/`Preferences`
4. Run test (should PASS - GREEN): casos cobertos
5. Refactor: `schedule_manager.cpp::avaliarDisparos` usa a função extraída, convertendo `DateTime` pra epoch antes de chamar

**Files to create/modify:**
- `schedule_execution.h` / `.cpp`: nova função de decisão de janela/lote
- `schedule_manager.cpp`: `avaliarDisparos` usa a função extraída
- `test/test_execution/test_execution.cpp`: novos casos de janela/retomada

**Acceptance Criteria:**
- [ ] Caso "dentro do lote atual" retorna tempo remanescente correto
- [ ] Caso "no intervalo entre lotes" aponta pro próximo lote
- [ ] Caso "depois do fim da janela total" não dispara
- [ ] Caso "antes do horário de início" não dispara
- [ ] `pio test -e native` passa com os novos testes
- [ ] Comportamento de `avaliarDisparos` não muda (mesma decisão pro firmware real)

---
