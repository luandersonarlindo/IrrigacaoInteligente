# Graph Report - .  (2026-07-14)

## Corpus Check
- 29 files · ~58,925 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 506 nodes · 1018 edges · 19 communities (16 shown, 3 thin omitted)
- Extraction: 83% EXTRACTED · 16% INFERRED · 0% AMBIGUOUS · INFERRED: 166 edges (avg confidence: 0.8)
- Token cost: 446,043 input · 0 output

## Community Hubs (Navigation)
- Menu Controller Logic
- Web AP Manager Backend
- Schedule Manager & Agendas UI
- OLED Display Rendering
- README Documentation Concepts
- Header Declarations & Config
- Dashboard Status UI
- Irrigation Controller (Valves)
- Rotary Encoder Driver
- System Architecture Overview
- DHT11 Sensor Driver
- RTC DS3231 Driver & Constructors
- Schedule Edit State Machine (UI)
- Hardware Wiring Diagram
- Main Loop Flowchart
- Dashboard Events/History UI
- Layered Architecture Rationale
- Figures Directory Reference
- Main Sketch Reference

## God Nodes (most connected - your core abstractions)
1. `MenuController` - 85 edges
2. `WebApManager` - 74 edges
3. `ScheduleManager` - 41 edges
4. `DisplayDriverOled` - 32 edges
5. `DisplayManager` - 28 edges
6. `RuntimeConfigManager` - 26 edges
7. `IrrigationController` - 25 edges
8. `AgendaSetor` - 19 edges
9. `figura2_arquitetura.png (Architecture Diagram)` - 18 edges
10. `Dht11Driver` - 16 edges

## Surprising Connections (you probably didn't know these)
- `Network Status Footer (AP IrrigacaoESP32 192.168.4.1, STA CasaWiFi 192.168.0.42, mDNS irrigacaoesp32.local, WebSocket porta 81)` --shares_data_with--> `WebApManager`  [INFERRED]
  figuras/figura5b_dashboard_agendas.png → web_ap_manager.h
- `Agenda Slot 01: Seg/Qua/Sex 06:00, 20 min, Setores S1-S4` --shares_data_with--> `AgendaSetor`  [INFERRED]
  figuras/figura5b_dashboard_agendas.png → schedule_manager.h
- `Agenda Slot 02: Ter/Qui/Sab 18:30, 15 min, Setores S5-S6` --shares_data_with--> `AgendaSetor`  [INFERRED]
  figuras/figura5b_dashboard_agendas.png → schedule_manager.h
- `Agendas (Schedules) Dashboard View Screenshot` --shares_data_with--> `ScheduleManager`  [INFERRED]
  figuras/figura5b_dashboard_agendas.png → schedule_manager.h
- `RuntimeConfigManager::RuntimeConfigManager()` --calls--> `inicializarPadrao`  [INFERRED]
  runtime_config_manager.cpp → runtime_config_manager.h

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Orquestração de setup/loop pelo IrrigacaoInteligente.ino sobre drivers e managers** — readme_irrigacaointeligente_ino, readme_encoder_driver, readme_display_driver_oled, readme_rtc_driver_ds3231, readme_dht11_driver, readme_runtime_config_manager, readme_menu_controller, readme_display_manager, readme_irrigation_controller, readme_schedule_manager, readme_web_ap_manager [EXTRACTED 1.00]
- **Fluxo do dashboard web: AP/STA, mDNS, WebSocket, rotas de API** — readme_web_ap_manager, readme_dashboard_web, readme_api_rotas, readme_websocket_status_push, readme_mdns, readme_wifi_ap_sta [INFERRED 0.85]
- **Fluxo de agendamento e persistência: schedule_manager, modelo semanal, execução em lotes, NVS e divergência de contrato** — readme_schedule_manager, readme_agendamento_semanal, readme_execucao_sequencial_por_lotes, readme_persistencia_nvs, readme_modelo_agenda_atual_vs_contrato [INFERRED 0.80]
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

## Communities (19 total, 3 thin omitted)

### Community 0 - "Menu Controller Logic"
Cohesion: 0.05
Nodes (85): DateTime, DirecaoEncoder, EstadoMenu, EtapaConfiguracao, EtapaProgramacao, FeedbackProgramacao, EstadoMenu, EtapaConfiguracao (+77 more)

### Community 1 - "Web AP Manager Backend"
Cohesion: 0.07
Nodes (68): EventoSistema, String, WebApManager, _agendaAguardandoIntervalo, _agendaPendenteMask, ativo, atualizar, atualizarEstadoAgendaSequencial (+60 more)

### Community 2 - "Schedule Manager & Agendas UI"
Cohesion: 0.07
Nodes (56): Agendas (Schedules) Dashboard View Screenshot, Agenda Slot 01: Seg/Qua/Sex 06:00, 20 min, Setores S1-S4, Agenda Slot 02: Ter/Qui/Sab 18:30, 15 min, Setores S5-S6, Batch Execution Info Banner (4 setores, limite de 2 simultâneos, 2 lotes, 10s de intervalo), Limpar todas as agendas (Clear All Schedules) Button, Empty Agenda Slots 03/04 (Slot vazio - Toque para configurar), Dashboard Header (Sistema de Irrigação, ESP32 hostname, Tempo Real badge, notification bell), Network Status Footer (AP IrrigacaoESP32 192.168.4.1, STA CasaWiFi 192.168.0.42, mDNS irrigacaoesp32.local, WebSocket porta 81) (+48 more)

### Community 3 - "OLED Display Rendering"
Cohesion: 0.08
Nodes (53): DisplayDriverOled, alturaFonte, begin, desenharIcone32, desenharLinha, desenharRetangulo, desenharRetanguloPreenchido, desenharTexto (+45 more)

### Community 4 - "README Documentation Concepts"
Cohesion: 0.07
Nodes (39): Agendamento automático semanal (4 agendas globais, diasMask/setoresMask), Alertas ativos no dashboard, Rotas da API do dashboard (/, /status, /api/status, /api/schedules, /api/events, /api/valve/*, /api/schedule/*, /api/config/runtime, /api/rtc/set), Config.h, Config_privado.h (segredos, não versionado), Dashboard web local (status, válvulas, agendas, runtime, RTC), Sensor DHT11 (temperatura e umidade do ar), dht11_driver.h/.cpp (+31 more)

### Community 5 - "Header Declarations & Config"
Cohesion: 0.11
Nodes (25): IrrigationController::IrrigationController(), BancoConfigRuntime, crc, duracaoPadraoMin, timeoutManualMs, versao, Preferences, RuntimeConfigManager (+17 more)

### Community 6 - "Dashboard Status UI"
Cohesion: 0.13
Nodes (24): Dashboard Status Screenshot (Sistema de Irrigação), Card: Ação Rápida (Desligar todas as válvulas button), DHT11 Sensor Readings (24.6°C / 58% humidity), Fechada (Closed/Idle) Valve State, GPIO Pin Mapping per Setor (23,25,26,27,32,33,13,14), Dashboard Header (ESP32 · irrigacaoesp32.local), Manual Mode for Valves, mDNS Hostname (irrigacaoesp32.local) (+16 more)

### Community 7 - "Irrigation Controller (Valves)"
Cohesion: 0.21
Nodes (21): EstadoValvula, EstadoValvula, IrrigationController, abrirValvula, algumAbertas, atualizar, atualizarDeadline, begin (+13 more)

### Community 8 - "Rotary Encoder Driver"
Cohesion: 0.13
Nodes (17): DirecaoEncoder, EncoderDriver, atualizar, begin, _botaoEvento, _botaoLongoEvento, botaoLongoPressionado, botaoPressionado (+9 more)

### Community 9 - "System Architecture Overview"
Cohesion: 0.22
Nodes (19): dht11_driver, display_driver_oled, display_manager, encoder_driver, figura2_arquitetura.png (Architecture Diagram), DHT11 (Hardware), Encoder (Hardware), NVS (Hardware/Storage) (+11 more)

### Community 10 - "DHT11 Sensor Driver"
Cohesion: 0.19
Nodes (14): Dht11Driver, atualizar, backendNome, begin, idadeLeituraMs, leituraValida, statusTexto, _temLeitura (+6 more)

### Community 11 - "RTC DS3231 Driver & Constructors"
Cohesion: 0.23
Nodes (11): DisplayManager::DisplayManager(), MenuController::MenuController(), DateTime, RtcDriverDs3231, agora, ajustarHora, begin, perdeuEnergia (+3 more)

### Community 12 - "Schedule Edit State Machine (UI)"
Cohesion: 0.35
Nodes (13): Figura 4 - Diagrama de Estados (Submenu Agenda), Nota: atalho de clique longo em EDIT_DIAS retorna direto ao SUBMENU_AGENDA, CONFIRMAR_EXCLUSAO (remove o slot), EDIT_DIAS (clique alterna DOM..SAB), EDIT_DURACAO (giro ajusta 1..240 min), EDIT_HORA (giro ajusta 0..23), EDIT_MINUTO (giro ajusta 0..59), EDIT_SETORES (clique alterna setores 1..8) (+5 more)

### Community 13 - "Hardware Wiring Diagram"
Cohesion: 0.27
Nodes (11): Barramento I2C (SDA: GPIO21, SCL: GPIO22), Display OLED SSD1306, Dispositivo do Usuário, Encoder Rotativo (HW-040), ESP32 (DevKit), Fonte de Alimentação, Módulo Relé 1 (4 canais), Módulo Relé 2 (4 canais) (+3 more)

### Community 14 - "Main Loop Flowchart"
Cohesion: 0.27
Nodes (11): 8. Atender servidor web (rotas HTTP da API e push por WebSocket), 9. Atualizar display OLED (tela corrente do menu ou status), 5. Atualizar irrigação (timeout manual e deadlines por setor), 6. Avaliar disparos do minuto (agendas ativas do dia no horário atual), 3. Executar comandos manuais (clique curto abre/fecha o setor selecionado), Início do loop(), 1. Ler encoder (giro, clique curto e clique longo), 7. Processar lotes sequenciais (máx. 2 setores simultâneos + intervalo entre lotes) (+3 more)

### Community 15 - "Dashboard Events/History UI"
Cohesion: 0.33
Nodes (10): Dashboard - Histórico de Eventos (Notificações), Alert Severity Levels (ATENÇÃO / INFORMAÇÃO), Irrigation Sector / Zone (Setor), Mark All As Read Action ("Marcar tudo como lido"), Notification/Event History View (Avisos / Histórico), Real-time Status Indicator ("Tempo real" badge), Schedule Save Event ("Agenda 1 foi salva"), Sector Activation Event ("Setor N foi ligado") (+2 more)

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
- **113 isolated node(s):** `_temLeitura`, `_ultimaLeituraMs`, `_ultimoOkMs`, `_ultimoStatus`, `_ultimoStatusTexto` (+108 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **3 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

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
- **Why does `MenuController` connect `Menu Controller Logic` to `RTC DS3231 Driver & Constructors`, `Schedule Manager & Agendas UI`, `OLED Display Rendering`, `Header Declarations & Config`?**
  _High betweenness centrality (0.233) - this node is a cross-community bridge._
- **Why does `WebApManager` connect `Web AP Manager Backend` to `Schedule Manager & Agendas UI`, `OLED Display Rendering`, `Header Declarations & Config`, `Irrigation Controller (Valves)`, `RTC DS3231 Driver & Constructors`?**
  _High betweenness centrality (0.194) - this node is a cross-community bridge._
- **Why does `DisplayManager` connect `OLED Display Rendering` to `Menu Controller Logic`, `Web AP Manager Backend`, `Header Declarations & Config`, `Irrigation Controller (Valves)`, `DHT11 Sensor Driver`, `RTC DS3231 Driver & Constructors`?**
  _High betweenness centrality (0.124) - this node is a cross-community bridge._