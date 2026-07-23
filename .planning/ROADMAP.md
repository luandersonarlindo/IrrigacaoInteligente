# Roadmap

## Architecture
**Approach:** A - Testes-primeiro (safety net)
**Rationale:** Sistema já roda em campo (hardware real). Toda mudança de código exige teste antes (regra TDD do claude-stack). Construir rede de testes sobre o comportamento atual primeiro reduz risco de regressão antes de mexer no modelo de agenda (que envolve migração de schema NVS) e na UX de configurações.
**Key decisions:**
- Testes rodam em host (sem hardware) via PlatformIO + Unity, ambiente `native`, sem alterar o build Arduino IDE existente
- Lógica de negócio pura (validação, parsing, motor de execução) é extraída para funções testáveis, isolada de hardware (Preferences/NVS, WiFi, Wire)
- Requisitos que dependem de hardware real (sensores, timing físico, RTC físico, WebSocket real) não são unit-testáveis em host — cobertos pelo checklist de homologação de campo (Fase 7) em vez de teste automatizado

## Requirements Coverage Map
| Requisito | Cobertura |
|-----------|-----------|
| R1 (irrigação manual) | Campo (Fase 7) — depende de relé físico e timeout real |
| R2 (agendamento) | Automatizado (Fase 1) + revisão de modelo (Fase 5) |
| R3 (execução por lotes) | Automatizado (Fase 2) |
| R4 (dashboard web) | Parcial automatizado (Fase 3, API) + Campo (Fase 7, UI) |
| R5 (DHT11) | Campo (Fase 7) — depende de sensor físico |
| R6 (persistência NVS) | Automatizado (Fase 2) |
| R7 (API REST) | Automatizado (Fase 3) |
| R8 (histórico eventos) | Campo (Fase 7) — depende de execução real ao longo do tempo |
| R9 (botão emergência) | Automatizado (Fase 3, endpoint off-all) + Campo (Fase 7, UI) |
| R10 (servidor web assíncrono) | Fase 4 (migração) — validação manual no hardware (Task 4.4) |
| NF1 (recuperação corrompida) | Automatizado (Fase 2) |
| NF2 (clamp de segurança) | Automatizado (Fase 1) |
| NF3 (dashboard standalone) | Campo (Fase 7) |
| NF4 (push tempo real) | Campo (Fase 7) — depende de WebSocket real |
| NF5 (resiliência sem RTC) | Campo (Fase 7) |
| NF6 (sem travamento no acesso web) | Fase 4 (migração) — validação manual no hardware (Task 4.4) |

## Phase Overview

| Phase | Name | Goal | Tasks | Dependencies | Status |
|-------|------|------|-------|--------------|--------|
| 1 | Fundação de Testes | Criar harness de teste em host, cobrir validação de agenda | 2 | - | pending |
| 2 | Testes - Persistência e Execução | Cobrir CRC/versão e motor de execução por lotes | 3 | Phase 1 | in-progress |
| 3 | Testes - API Web | Cobrir parsing/contrato da API REST | 1 | Phase 1 | pending |
| 4 | Migração ESPAsyncWebServer (bug crítico) | Eliminar travamento do firmware ao acessar a página web | 4 | - | pending |
| 5 | Modelo de Agenda | Definir e migrar modelo final (resolve gap com contrato ABNT) | 2 | Phase 1, Phase 2 | pending |
| 6 | UX de Configurações | Reduzir profundidade do menu de configuração | 2 | Phase 1 | pending |
| 7 | Homologação de Campo | Checklist e validação manual no hardware real | 2 | Phase 1-6 | pending |

**Nota de prioridade:** Fase 4 é bug crítico em produção (travamento reprodutível), inserida fora de ordem — não depende das Fases 1-3 e pode/deve ser executada antes delas se o usuário priorizar estabilidade sobre a rede de testes em andamento.

---

## Phase 1: Fundação de Testes

**Goal:** Criar infraestrutura de teste automatizado rodável em host (sem hardware), isolando regras de validação de agenda.

**Success Criteria:**
- [ ] `pio test -e native` roda e passa localmente sem hardware
- [ ] Regras de validação de agenda (hora, minuto, duração, dias, setores, duplicidade) cobertas por teste

**Requirements:** R2, NF2

### Tasks

#### Task 1.1: Configurar ambiente PlatformIO nativo para testes
- **What:** Adicionar `platformio.ini` com ambiente `native` + Unity, sem alterar o build Arduino IDE/ESP32 existente
- **Test first:** Teste trivial (`assert true`) para validar o harness
- **Files:** `platformio.ini`, `test/test_smoke/test_smoke.cpp`
- **Acceptance:** `pio test -e native` executa e passa
- **Estimate:** M

#### Task 1.2: Isolar e testar regras de validação de agenda
- **What:** Extrair validação (hora 0-23, minuto 0-59, duração >=1, ao menos 1 dia, ao menos 1 setor, sem duplicata exata) de `schedule_manager.cpp` para função pura testável
- **Test first:** Casos hora inválida (24, -1), minuto inválido (60), duração 0, sem dia, sem setor, duplicata exata
- **Files:** `schedule_manager.cpp`, `schedule_manager.h`, `test/test_schedule/test_validation.cpp`
- **Acceptance:** Todos os casos de validação cobertos e passando
- **Estimate:** M

---

## Phase 2: Testes - Persistência e Execução

**Goal:** Testar lógica de persistência (versão+CRC) e motor de execução sequencial por lotes sem depender de hardware real.

**Success Criteria:**
- [ ] Casos válido/corrompido de versão+CRC cobertos
- [ ] Fila de lotes, limite de setores simultâneos e conflito de duração cobertos

**Requirements:** R3, R6, NF1

### Tasks

#### Task 2.1: Testar validação de versão+CRC do banco de agendas
- **What:** Extrair checagem de versão/CRC para função testável isolada de `Preferences`
- **Test first:** CRC válido aceita; CRC inválido reinicializa banco padrão seguro
- **Files:** `schedule_manager.cpp`, `schedule_manager.h`, `test/test_schedule/test_persistence.cpp`
- **Acceptance:** Casos válido e corrompido cobertos e passando
- **Estimate:** M

#### Task 2.2: Testar motor de execução sequencial por lotes (escopo parcial)
- **What:** Testar seleção de setores pro lote atual (`MAX_SETOR_SIMULTANEOS_AGENDA`) e conflito de setor (mantém maior duração). Repartido em 2.2/2.3 durante o build — escopo original era grande demais pra uma task.
- **Test first:** Fila com mais setores que o limite simultâneo; conflito de duração no mesmo setor
- **Files:** `schedule_execution.h/.cpp`, `IrrigacaoInteligente.ino`, `test/test_execution/test_execution.cpp`
- **Acceptance:** Casos cobertos e passando
- **Estimate:** M
- **Status:** complete

#### Task 2.3: Testar retomada de janela ativa com tempo remanescente
- **What:** Extrair e testar a decisão de qual lote está ativo, com tempo remanescente, incluindo retomada após reboot (`ScheduleManager::avaliarDisparos`)
- **Test first:** Dentro do lote atual; no intervalo entre lotes; retomada após reboot com tempo remanescente; depois do fim da janela; antes do início
- **Files:** `schedule_execution.h/.cpp`, `schedule_manager.cpp`, `test/test_execution/test_execution.cpp`
- **Acceptance:** Casos cobertos e passando, comportamento de `avaliarDisparos` não muda
- **Estimate:** L
- **Status:** pending

---

## Phase 3: Testes - API Web

**Goal:** Testar parsing e contrato da API REST sem precisar de rede/HTTP real.

**Success Criteria:**
- [ ] Parsing de `index`/`slot` (base 0 e base 1) coberto
- [ ] Parsing de `state` (`1/0`, `true/false`, `on/off`, `ligado/desligado`) coberto
- [ ] Resposta de erro JSON com `ok=false` coberta

**Requirements:** R7, R9

### Tasks

#### Task 3.1: Extrair e testar parsing de parâmetros da API
- **What:** Isolar parsing de `index`/`slot`/`state` em funções puras testáveis fora do `WebServer`
- **Test first:** Casos base 0/base 1, todas as variantes de `state`, entrada inválida retorna `ok=false`
- **Files:** `web_ap_manager.cpp`, `web_ap_manager.h`, `test/test_web/test_api_parsing.cpp`
- **Acceptance:** Todos os casos de contrato cobertos e passando
- **Estimate:** M

---

## Phase 4: Migração ESPAsyncWebServer (bug crítico)

**Goal:** Eliminar o travamento do firmware inteiro (display, encoder, irrigação) que ocorre ao acessar a página web, causado pelo `WebServer` síncrono do Arduino-ESP32 bloqueando o loop principal durante o envio do HTML (~35-40KB) e agravado por mDNS.

**Success Criteria:**
- [ ] `WebServer` (core) substituído por `ESPAsyncWebServer`; `WebSocketsServer` (Links2004) substituído por `AsyncWebSocket`
- [ ] Página web acessada repetidamente (10-20x, com e sem mDNS) sem travar o firmware
- [ ] Toggle de válvula, salvar/excluir/limpar agenda, ajustar RTC, salvar config runtime e push WebSocket ao vivo continuam funcionando como antes
- [ ] `pio test -e native` continua passando (nenhuma regressão na lógica extraída em Fases 1-2)

**Requirements:** R10, NF6

### Tasks

#### Task 4.1: Instalar ESPAsyncWebServer + AsyncTCP
- **What:** Documentar e instalar manualmente as bibliotecas (não disponíveis no Library Manager padrão) — `ESPAsyncWebServer` (mathieucarbou ou me-no-dev, conferir fork ativo/compatível com o core ESP32 instalado) + `AsyncTCP` (dependência)
- **Files:** guia de instalação (fora do repo ou em `docs/solutions/`), `Documents/Arduino/libraries/`
- **Acceptance:** Sketch de teste mínimo (`AsyncWebServer` respondendo em `/`) compila e roda no hardware
- **Estimate:** S
- **Status:** complete — já instaladas: `ESPAsyncWebServer` (lacamera/esphome fork, v3.1.0) + `AsyncTCP` (dvarrel, v1.1.4), combinação padrão compatível com core ESP32 3.3.10 instalado

#### Task 4.2: Migrar rotas HTTP (`/`, `/api/*`) para ESPAsyncWebServer
- **What:** Reescrever `configurarRotas()` e os handlers (`enviarPaginaPrincipal`, `enviarStatusSistema`, `enviarListaAgendas`, POSTs de válvula/agenda/config/RTC) usando a API assíncrona (`AsyncWebServerRequest *`); tratar leitura de body de POST (JSON ou form) no padrão async (`onBody` ou `request->arg()` conforme o caso)
- **Files:** `web_ap_manager.h`, `web_ap_manager.cpp`
- **Acceptance:** Todas as rotas respondem igual ao comportamento atual (validado manualmente); build ESP32/Arduino IDE compila
- **Estimate:** L
- **Status:** complete — todos os 13 handlers (GET+POST) migrados de `WebServer`/`_server.hasArg`/`arg` para `AsyncWebServerRequest*`/`temArg`/`lerArg` (novos helpers que checam param de POST-form e querystring); `enviarRespostaJson`/`enviarErroJson`/`lerIndiceValvula`/`lerSlotAgenda` viraram estáticos recebendo `request`; `send_P`/`beginResponse` no lugar de `send`/`sendHeader`. Nenhum handler usava JSON body (só form/querystring), então não precisou de `onBody`

#### Task 4.3: Migrar WebSocket para AsyncWebSocket
- **What:** Substituir `WebSocketsServer _webSocket` por `AsyncWebSocket`, adaptar `iniciarWebSocket()`/`atualizarWebSocket()`/`enviarStatusWebSocket()` para a API assíncrona (evento `onEvent` com `AwsEventType`); remover dependência da lib `WebSockets` (Links2004) e a flag `IRRIGACAO_WS_LIB_AVAILABLE`
- **Files:** `web_ap_manager.h`, `web_ap_manager.cpp`, `Config.h`
- **Acceptance:** Dashboard recebe push de status a cada 2s via WebSocket igual a antes; fallback HTTP continua funcionando
- **Estimate:** M
- **Status:** complete — `AsyncWebSocket` embutido no mesmo `AsyncWebServer` (porta 80, path `/ws`, em vez de porta 81 separada); `WIFI_WEBSOCKET_PORT` removido de `Config.h`/`Config_privado.h.example`. JS do dashboard (`WEB_DASHBOARD_HTML`) ajustado: `wsPortaAtual` → `wsPathAtual`, URL de conexão `ws://host/ws`, campo `rede.websocket.porta`/`biblioteca` do JSON → `rede.websocket.path`. **Mudança de contrato**: qualquer cliente externo que dependia da porta 81 direta precisa ser atualizado (só o próprio dashboard embutido depende disso hoje)

#### Task 4.4: Validar no hardware e fechar o bug
- **What:** Rodar critério de sucesso completo no ESP32 físico — acesso repetido à página (com/sem mDNS), todas as ações web, WebSocket ao vivo; registrar resultado
- **Files:** `docs/solutions/fix-travamento-webserver-2026-07.md`
- **Acceptance:** NF6 satisfeito; nenhum travamento em 10-20 acessos consecutivos
- **Estimate:** M
- **Status:** complete — usuário validou no hardware: acesso repetido por IP e por mDNS sem travar, toggle de válvula/agenda/RTC/config runtime ok, WebSocket conectando ("tempo real"). NF6 satisfeito, bug fechado

---

## Phase 5: Modelo de Agenda

**Goal:** Definir oficialmente o modelo de agenda (mantém 4 global ou migra para 4×setor) e migrar dados com segurança, resolvendo o gap com o contrato ABNT.

**Success Criteria:**
- [ ] Decisão documentada em `PROJECT.md`/`REQUIREMENTS.md` com rationale
- [ ] Se houver migração de schema, versão do NVS incrementada e testada (dado antigo não quebra)

**Requirements:** R2 (revisão)

### Tasks

#### Task 4.1: Definir modelo final com o usuário e documentar decisão
- **What:** Apresentar trade-offs (4 global vs 4×setor), decidir com o usuário, atualizar `REQUIREMENTS.md`/`PROJECT.md`
- **Test first:** N/A (decisão) — testes de Fase 1/2 servem de rede de regressão
- **Files:** `.planning/PROJECT.md`, `.planning/REQUIREMENTS.md`
- **Acceptance:** Decisão registrada com rationale
- **Estimate:** S

#### Task 4.2: Migrar schema/lógica se o modelo mudar
- **What:** Implementar mudança de schema com bump de versão NVS, migrando ou resetando dado antigo com segurança
- **Test first:** Teste de upgrade de versão antiga para nova
- **Files:** `schedule_manager.cpp`, `schedule_manager.h`, `test/test_schedule/test_migration.cpp`
- **Acceptance:** Dado antigo migra ou reseta com segurança; testes passam
- **Estimate:** L

---

## Phase 6: UX de Configurações

**Goal:** Reduzir a profundidade de navegação do menu OLED de Configurações.

**Success Criteria:**
- [ ] Novo fluxo tem menos níveis de submenu que o atual
- [ ] Validado manualmente no hardware (navegação sem travamento)

**Requirements:** Melhoria de UX (seção "Limites atuais e roadmap" do README)

### Tasks

#### Task 5.1: Mapear fluxo atual e propor fluxo simplificado
- **What:** Desenhar novo fluxo de estados do `menu_controller` reduzindo níveis de navegação
- **Test first:** N/A (design)
- **Files:** `docs/brainstorms/ux-config-flow.md`
- **Acceptance:** Usuário aprova o novo fluxo
- **Estimate:** S

#### Task 5.2: Implementar novo fluxo de menu
- **What:** Refatorar `menu_controller.cpp/h` para o fluxo aprovado
- **Test first:** Testes de transição de estado extraíveis, se a lógica de estado for isolável
- **Files:** `menu_controller.cpp`, `menu_controller.h`
- **Acceptance:** Navegação manual no hardware confirma menos cliques até salvar configuração
- **Estimate:** L

---

## Phase 7: Homologação de Campo

**Goal:** Formalizar checklist de validação em campo (elétrica, rede, UX) cobrindo os requisitos não testáveis em host.

**Success Criteria:**
- [ ] Checklist documentado e executável
- [ ] Primeira execução completa registrada com resultado por item

**Requirements:** R1, R4, R5, R8, R9, NF3, NF4, NF5

### Tasks

#### Task 6.1: Criar checklist de homologação de campo
- **What:** Documento com passos de validação elétrica, rede (AP/STA/mDNS) e UX, baseado na seção "Validação recomendada" do README
- **Files:** `docs/plans/checklist-homologacao-campo.md`
- **Acceptance:** Checklist revisado e aprovado pelo usuário
- **Estimate:** S

#### Task 6.2: Executar primeira homologação e registrar resultado
- **What:** Rodar checklist no hardware físico, registrar resultado (pass/fail) por item
- **Files:** `docs/solutions/homologacao-2026-07-15.md`
- **Acceptance:** Checklist executado, resultados documentados
- **Estimate:** M
