# Graph Report - .  (2026-07-16)

## Corpus Check
- 41 files · ~69,041 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 647 nodes · 1140 edges · 32 communities (26 shown, 6 thin omitted)
- Extraction: 88% EXTRACTED · 12% INFERRED · 0% AMBIGUOUS · INFERRED: 136 edges (avg confidence: 0.8)
- Token cost: 59,708 input · 0 output

## Community Hubs (Navigation)
- Menu Controller e Navegacao
- Dashboard Web - Agendas
- Display Manager e Telas
- Driver DHT11 e Config
- Arquitetura e Requisitos
- WebServer - Estado e Eventos
- Controlador de Irrigacao
- WebServer - AP e Rotas
- Historico de Sensores
- Config Runtime NVS
- Dashboard Web - Status
- Validacao de Agenda
- Executor de Agenda
- Driver OLED U8g2
- Arquitetura de Modulos
- Estados de Programacao
- Hardware e Fiacao
- Fluxo do Loop Principal
- Driver RTC DS3231
- WebServer - JSON Helpers
- Dashboard Web - Eventos
- Redesign Menu Principal
- Sensor DHT11 Conceito
- Roadmap Fase 5
- Roadmap Fase 6
- Persistencia README
- DateTime Schedule Manager
- Preferences Schedule Manager

## God Nodes (most connected - your core abstractions)
1. `MenuController` - 83 edges
2. `WebApManager` - 72 edges
3. `ScheduleManager` - 40 edges
4. `DisplayManager` - 29 edges
5. `IrrigationController` - 29 edges
6. `RuntimeConfigManager` - 21 edges
7. `Dht11Driver` - 19 edges
8. `AgendaSetor` - 19 edges
9. `figura2_arquitetura.png (Architecture Diagram)` - 18 edges
10. `AgendaExecutor` - 18 edges

## Surprising Connections (you probably didn't know these)
- `Network Status Footer (AP IrrigacaoESP32 192.168.4.1, STA CasaWiFi 192.168.0.42, mDNS irrigacaoesp32.local, WebSocket porta 81)` --shares_data_with--> `WebApManager`  [INFERRED]
  figuras/figura5b_dashboard_agendas.png → web_ap_manager.h
- `Agendas (Schedules) Dashboard View Screenshot` --shares_data_with--> `ScheduleManager`  [INFERRED]
  figuras/figura5b_dashboard_agendas.png → schedule_manager.h
- `Agenda Slot 01: Seg/Qua/Sex 06:00, 20 min, Setores S1-S4` --shares_data_with--> `AgendaSetor`  [INFERRED]
  figuras/figura5b_dashboard_agendas.png → schedule_manager.h
- `Agenda Slot 02: Ter/Qui/Sab 18:30, 15 min, Setores S5-S6` --shares_data_with--> `AgendaSetor`  [INFERRED]
  figuras/figura5b_dashboard_agendas.png → schedule_manager.h
- `RuntimeConfigManager::RuntimeConfigManager()` --calls--> `inicializarPadrao`  [INFERRED]
  runtime_config_manager.cpp → runtime_config_manager.h

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Phase 1-2 Test Foundation Workflow** — tdd_approach, platformio_native_testing, code_schedule_validation, code_schedule_manager [INFERRED 0.85]
- **Hardware I2C Bus Shared Components** — hardware_esp32, hardware_oled_display, hardware_rtc_ds3231 [EXTRACTED 1.00]
- **Schedule Management Pipeline** — code_schedule_validation, code_schedule_persistence, code_schedule_execution, code_schedule_manager [INFERRED 0.85]
- **I2C Bus linking ESP32, OLED Display and RTC DS3231** — figuras_figura1_hardware_esp32, figuras_figura1_hardware_display_oled_ssd1306, figuras_figura1_hardware_rtc_ds3231, figuras_figura1_hardware_barramento_i2c [EXTRACTED 1.00]
- **Irrigation Control Chain: ESP32 drives relay modules to actuate solenoid valves** — figuras_figura1_hardware_esp32, figuras_figura1_hardware_modulo_rele_1, figuras_figura1_hardware_modulo_rele_2, figuras_figura1_hardware_valvulas_solenoides [INFERRED 0.75]
- **Orchestration Layer Drives Managers/Controllers Layer** — irrigacaointeligente_ino, menu_controller, display_manager, irrigation_controller, schedule_manager, runtime_config_manager, web_ap_manager [EXTRACTED 1.00]
- **Managers/Controllers Layer (Business Rules and State)** — menu_controller, display_manager, irrigation_controller, schedule_manager, runtime_config_manager, web_ap_manager [EXTRACTED 1.00]
- **Drivers Layer (Hardware Access)** — encoder_driver, display_driver_oled, rtc_driver_ds3231, dht11_driver [EXTRACTED 1.00]
- **ESP32 Hardware and Resources Layer** — figuras_figura2_arquitetura_encoder, figuras_figura2_arquitetura_oled, figuras_figura2_arquitetura_rtc, figuras_figura2_arquitetura_dht11, figuras_figura2_arquitetura_reles, figuras_figura2_arquitetura_nvs, figuras_figura2_arquitetura_radio_wifi [EXTRACTED 1.00]
- **ESP32 Irrigation loop() Execution Cycle** — figuras_figura3_fluxograma_inicio_loop, figuras_figura3_fluxograma_ler_encoder, figuras_figura3_fluxograma_processar_menu, figuras_figura3_fluxograma_executar_comandos_manuais, figuras_figura3_fluxograma_rotina_seguranca, figuras_figura3_fluxograma_atualizar_irrigacao, figuras_figura3_fluxograma_rtc_ativo, figuras_figura3_fluxograma_avaliar_disparos_minuto, figuras_figura3_fluxograma_processar_lotes_sequenciais, figuras_figura3_fluxograma_atender_servidor_web, figuras_figura3_fluxograma_atualizar_display_oled [EXTRACTED 1.00]
- **RTC-dependent Irrigation Scheduling Branch** — figuras_figura3_fluxograma_rtc_ativo, figuras_figura3_fluxograma_avaliar_disparos_minuto, figuras_figura3_fluxograma_processar_lotes_sequenciais [INFERRED 0.85]
- **Máquina de Estados do Submenu de Agenda (edição de horários de irrigação)** — figuras_figura4_estados_selecionar_agenda, figuras_figura4_estados_submenu_agenda, figuras_figura4_estados_edit_hora, figuras_figura4_estados_edit_minuto, figuras_figura4_estados_edit_duracao, figuras_figura4_estados_edit_dias, figuras_figura4_estados_edit_setores, figuras_figura4_estados_voltar, figuras_figura4_estados_salvar, figuras_figura4_estados_confirmar_exclusao [INFERRED 0.85]
- **Dashboard Summary Cards Group (Válvulas Abertas, Próxima Agenda, Ação Rápida)** — figuras_figura5a_dashboard_status_valvulas_abertas_card, figuras_figura5a_dashboard_status_proxima_agenda_card, figuras_figura5a_dashboard_status_acao_rapida_card [EXTRACTED 1.00]
- **8-Sector Valve Control Grid** — figuras_figura5a_dashboard_status_setor01, figuras_figura5a_dashboard_status_setor02, figuras_figura5a_dashboard_status_setor03, figuras_figura5a_dashboard_status_setor04, figuras_figura5a_dashboard_status_setor05, figuras_figura5a_dashboard_status_setor06, figuras_figura5a_dashboard_status_setor07, figuras_figura5a_dashboard_status_setor08 [EXTRACTED 1.00]
- **Relay Modules Implement Control for the 8 Setores** — figuras_figura5a_dashboard_status_relay_modules, figuras_figura5a_dashboard_status_setor01, figuras_figura5a_dashboard_status_setor02, figuras_figura5a_dashboard_status_setor03, figuras_figura5a_dashboard_status_setor04, figuras_figura5a_dashboard_status_setor05, figuras_figura5a_dashboard_status_setor06, figuras_figura5a_dashboard_status_setor07, figuras_figura5a_dashboard_status_setor08 [INFERRED 0.85]
- **Agendas UI rendering flow: ScheduleManager storage -> enviarListaAgendas JSON endpoint -> renderAgendas JS -> dashboard slot cards** — figuras_figura5b_dashboard_agendas, schedule_manager_schedulemanager, web_ap_manager_enviarlistaagendas, web_ap_manager_renderagendas [INFERRED 0.85]
- **Today's Event Log Entries (HOJE) shown in the history panel** — figuras_figura5c_dashboard_eventos_concept_sector_activation_event, figuras_figura5c_dashboard_eventos_concept_schedule_save_event, figuras_figura5c_dashboard_eventos_concept_wifi_connection_event, figuras_figura5c_dashboard_eventos_concept_web_panel_start_event [EXTRACTED 1.00]

## Communities (32 total, 6 thin omitted)

### Community 0 - "Menu Controller e Navegacao"
Cohesion: 0.05
Nodes (85): DateTime, DirecaoEncoder, EstadoMenu, EtapaConfiguracao, EtapaProgramacao, FeedbackProgramacao, EstadoMenu, EtapaConfiguracao (+77 more)

### Community 1 - "Dashboard Web - Agendas"
Cohesion: 0.07
Nodes (57): DateTime, Agendas (Schedules) Dashboard View Screenshot, Agenda Slot 01: Seg/Qua/Sex 06:00, 20 min, Setores S1-S4, Agenda Slot 02: Ter/Qui/Sab 18:30, 15 min, Setores S5-S6, Batch Execution Info Banner (4 setores, limite de 2 simultâneos, 2 lotes, 10s de intervalo), Limpar todas as agendas (Clear All Schedules) Button, Empty Agenda Slots 03/04 (Slot vazio - Toque para configurar), Dashboard Header (Sistema de Irrigação, ESP32 hostname, Tempo Real badge, notification bell) (+49 more)

### Community 2 - "Display Manager e Telas"
Cohesion: 0.10
Nodes (47): adicionarTextoQuebrado(), DisplayDriverOled, MenuController, RtcDriverDs3231, String, WebApManager, desenharAnimacaoCheck(), desenharAnimacaoCheck48() (+39 more)

### Community 3 - "Driver DHT11 e Config"
Cohesion: 0.06
Nodes (34): Dht11Driver, atualizar, backendNome, begin, _historico, idadeLeituraMs, leituraValida, statusTexto (+26 more)

### Community 4 - "Arquitetura e Requisitos"
Cohesion: 0.08
Nodes (31): schedule_execution module, schedule_manager module, schedule_persistence module, schedule_validation module, sensor_historico module, Gap: 4 Global Schedules vs ABNT Contract, ESP32 Hardware Platform, OLED SSD1306 Display (+23 more)

### Community 5 - "WebServer - Estado e Eventos"
Cohesion: 0.07
Nodes (30): EventoSistema, WebApManager, _agendaAguardandoIntervalo, _agendaPendenteMask, atualizarHistoricoEstado, begin, _historicoCount, _historicoEventos (+22 more)

### Community 6 - "Controlador de Irrigacao"
Cohesion: 0.14
Nodes (29): EstadoValvula, RuntimeConfigManager, EstadoValvula, RuntimeConfigManager, IrrigationController, abrirValvula, algumAbertas, atualizar (+21 more)

### Community 7 - "WebServer - AP e Rotas"
Cohesion: 0.07
Nodes (26): ativo, atualizar, atualizarEstadoAgendaSequencial, atualizarLeituraClima, atualizarMdns, configurarRotas, contarValvulasAutomaticasAbertas, contarValvulasManuaisAbertas (+18 more)

### Community 8 - "Historico de Sensores"
Cohesion: 0.10
Nodes (11): Buffer, proximoIndice, temperaturas, total, umidades, faixaTemperatura(), faixaUmidade(), iniciar() (+3 more)

### Community 9 - "Config Runtime NVS"
Cohesion: 0.13
Nodes (23): BancoConfigRuntime, crc, duracaoPadraoMin, timeoutManualMs, versao, Preferences, RuntimeConfigManager, _banco (+15 more)

### Community 10 - "Dashboard Web - Status"
Cohesion: 0.13
Nodes (24): Dashboard Status Screenshot (Sistema de Irrigação), Card: Ação Rápida (Desligar todas as válvulas button), DHT11 Sensor Readings (24.6°C / 58% humidity), Fechada (Closed/Idle) Valve State, GPIO Pin Mapping per Setor (23,25,26,27,32,33,13,14), Dashboard Header (ESP32 · irrigacaoesp32.local), Manual Mode for Valves, mDNS Hostname (irrigacaoesp32.local) (+16 more)

### Community 11 - "Validacao de Agenda"
Cohesion: 0.13
Nodes (18): CampoAgenda, diasMask, duracaoMin, hora, minuto, setoresMask, duplicada(), validar() (+10 more)

### Community 12 - "Executor de Agenda"
Cohesion: 0.18
Nodes (19): AgendaExecutor, AgendaExecutor::AgendaExecutor(), aguardandoIntervalo, ativa, atualizar, cancelar, _duracaoPendenteMin, enfileirarDisparos (+11 more)

### Community 13 - "Driver OLED U8g2"
Cohesion: 0.17
Nodes (17): DisplayDriverOled, alturaFonte, begin, desenharIcone32, desenharLinha, desenharRetangulo, desenharRetanguloPreenchido, desenharTexto (+9 more)

### Community 15 - "Arquitetura de Modulos"
Cohesion: 0.23
Nodes (18): dht11_driver, display_driver_oled, display_manager, encoder_driver, figura2_arquitetura.png (Architecture Diagram), DHT11 (Hardware), Encoder (Hardware), NVS (Hardware/Storage) (+10 more)

### Community 16 - "Estados de Programacao"
Cohesion: 0.35
Nodes (13): Figura 4 - Diagrama de Estados (Submenu Agenda), Nota: atalho de clique longo em EDIT_DIAS retorna direto ao SUBMENU_AGENDA, CONFIRMAR_EXCLUSAO (remove o slot), EDIT_DIAS (clique alterna DOM..SAB), EDIT_DURACAO (giro ajusta 1..240 min), EDIT_HORA (giro ajusta 0..23), EDIT_MINUTO (giro ajusta 0..59), EDIT_SETORES (clique alterna setores 1..8) (+5 more)

### Community 19 - "Hardware e Fiacao"
Cohesion: 0.27
Nodes (11): Barramento I2C (SDA: GPIO21, SCL: GPIO22), Display OLED SSD1306, Dispositivo do Usuário, Encoder Rotativo (HW-040), ESP32 (DevKit), Fonte de Alimentação, Módulo Relé 1 (4 canais), Módulo Relé 2 (4 canais) (+3 more)

### Community 20 - "Fluxo do Loop Principal"
Cohesion: 0.27
Nodes (11): 8. Atender servidor web (rotas HTTP da API e push por WebSocket), 9. Atualizar display OLED (tela corrente do menu ou status), 5. Atualizar irrigação (timeout manual e deadlines por setor), 6. Avaliar disparos do minuto (agendas ativas do dia no horário atual), 3. Executar comandos manuais (clique curto abre/fecha o setor selecionado), Início do loop(), 1. Ler encoder (giro, clique curto e clique longo), 7. Processar lotes sequenciais (máx. 2 setores simultâneos + intervalo entre lotes) (+3 more)

### Community 21 - "Driver RTC DS3231"
Cohesion: 0.29
Nodes (9): MenuController::MenuController(), DateTime, RtcDriverDs3231, agora, ajustarHora, begin, perdeuEnergia, _rtc (+1 more)

### Community 22 - "WebServer - JSON Helpers"
Cohesion: 0.18
Nodes (11): String, enviarRespostaJson, escaparJson, ipStaTexto, ipTexto, montarJsonStatusSistema, registrarEvento, textoParaBool (+3 more)

### Community 23 - "Dashboard Web - Eventos"
Cohesion: 0.33
Nodes (10): Dashboard - Histórico de Eventos (Notificações), Alert Severity Levels (ATENÇÃO / INFORMAÇÃO), Irrigation Sector / Zone (Setor), Mark All As Read Action ("Marcar tudo como lido"), Notification/Event History View (Avisos / Histórico), Real-time Status Indicator ("Tempo real" badge), Schedule Save Event ("Agenda 1 foi salva"), Sector Activation Event ("Setor N foi ligado") (+2 more)

### Community 24 - "Redesign Menu Principal"
Cohesion: 0.40
Nodes (5): Menu Cards Layout B - 2 Lines Stacked, menu_cards module, Task 1: Menu Card Selection Logic with Wrap, Task 2: Redraw Menu Principal with Card Layout, Menu Principal Redesign in Stacked Cards

## Ambiguous Edges - Review These
- `display_manager` → `dht11_driver`  [AMBIGUOUS]
  figuras/figura2_arquitetura.png · relation: conceptually_related_to
- `irrigation_controller` → `dht11_driver`  [AMBIGUOUS]
  figuras/figura2_arquitetura.png · relation: conceptually_related_to
- `EDIT_SETORES (clique alterna setores 1..8)` → `Nota: atalho de clique longo em EDIT_DIAS retorna direto ao SUBMENU_AGENDA`  [AMBIGUOUS]
  figuras/figura4_estados.png · relation: rationale_for
- `Agenda Slot 01: Seg/Qua/Sex 06:00, 20 min, Setores S1-S4` → `Empty Agenda Slots 03/04 (Slot vazio - Toque para configurar)`  [AMBIGUOUS]
  figuras/figura5b_dashboard_agendas.png · relation: conceptually_related_to

## Knowledge Gaps
- **120 isolated node(s):** `_u8g2`, `_encoder`, `_posicaoAnterior`, `_estadoAnteriorBtn`, `_botaoEvento` (+115 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **6 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `display_manager` and `dht11_driver`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `irrigation_controller` and `dht11_driver`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `EDIT_SETORES (clique alterna setores 1..8)` and `Nota: atalho de clique longo em EDIT_DIAS retorna direto ao SUBMENU_AGENDA`?**
  _Edge tagged AMBIGUOUS (relation: rationale_for) - confidence is low._
- **What is the exact relationship between `Agenda Slot 01: Seg/Qua/Sex 06:00, 20 min, Setores S1-S4` and `Empty Agenda Slots 03/04 (Slot vazio - Toque para configurar)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `WebApManager` connect `WebServer - Estado e Eventos` to `Dashboard Web - Agendas`, `Driver DHT11 e Config`, `Controlador de Irrigacao`, `WebServer - AP e Rotas`, `Config Runtime NVS`, `Driver RTC DS3231`, `WebServer - JSON Helpers`?**
  _High betweenness centrality (0.214) - this node is a cross-community bridge._
- **Why does `IrrigationController` connect `Controlador de Irrigacao` to `Display Manager e Telas`, `Driver DHT11 e Config`, `Executor de Agenda`, `WebServer - Estado e Eventos`?**
  _High betweenness centrality (0.166) - this node is a cross-community bridge._
- **Why does `MenuController` connect `Menu Controller e Navegacao` to `Config Runtime NVS`, `Driver DHT11 e Config`, `Driver RTC DS3231`, `Dashboard Web - Agendas`?**
  _High betweenness centrality (0.163) - this node is a cross-community bridge._