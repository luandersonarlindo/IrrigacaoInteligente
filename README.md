# 🌱 IrrigacaoInteligente

Sistema de irrigacao inteligente com ESP32, display OLED, encoder rotativo, RTC DS3231 e controle de ate 8 valvulas por rele.

Este README descreve o estado atual implementado no firmware.

## 👀 Visao rapida

- ✅ Controle manual de setores no menu.
- 🗓️ Agendamento automatico semanal com execucao sequencial por lotes.
- 🌐 Dashboard web local (AP Wi-Fi) para monitorar e controlar o sistema.
- 💾 Persistencia de agendas e configuracoes runtime na flash (NVS), com versao e CRC.
- 🕒 Operacao mesmo sem RTC (sem hora real).
- 🛡️ Timeout de seguranca no modo manual.

## 🧭 Sumario

1. 🎯 Escopo
2. 🧰 Hardware
3. ⚡ Configuracao rapida
4. ⚙️ Configuracoes principais
5. 🧱 Arquitetura
6. 🔄 Fluxo do firmware
7. 🕹️ Operacao da interface
8. 📅 Agendamento
9. 🌐 Dashboard web
10. 💾 Persistencia
11. 🗂️ Estrutura do projeto
12. 🛠️ Build e gravacao
13. 🩺 Troubleshooting rapido
14. ✅ Validacao recomendada
15. 🛣️ Limites atuais e roadmap

## 1. 🎯 Escopo

O firmware entrega:

- Irrigacao manual por setor com feedback no OLED.
- Irrigacao automatica por agenda semanal.
- Controle fisico de reles (trigger HIGH).
- Dashboard web com AP dedicado e tentativas de conexao STA.
- Persistencia de agendas e configuracoes runtime apos reboot.
- Regras de seguranca e validacao para evitar configuracoes invalidas.

Documentos complementares:

- GUIA_DIDATICO_PROJETO.md
- FASE5_CONTRATO_TECNICO.md

## 2. 🧰 Hardware

Plataforma:

- ESP32
- Encoder rotativo com botao (HW-040 ou equivalente)
- OLED SSD1306 128x64 I2C
- RTC DS3231 I2C
- 2 modulos de rele 4 canais (8 canais no total)

Mapeamento de pinos (Config.h):

| Recurso     | Pino |
| ----------- | ---- |
| Encoder CLK | 19   |
| Encoder DT  | 18   |
| Encoder BTN | 4    |
| OLED SDA    | 21   |
| OLED SCL    | 22   |
| Rele 1      | 23   |
| Rele 2      | 25   |
| Rele 3      | 26   |
| Rele 4      | 27   |
| Rele 5      | 32   |
| Rele 6      | 33   |
| Rele 7      | 13   |
| Rele 8      | 14   |

Observacao: OLED e DS3231 compartilham o barramento I2C.

## 3. ⚡ Configuracao rapida

Passo a passo curto para colocar o sistema para rodar:

1. Monte os componentes conforme os pinos definidos em Config.h.
2. Instale as bibliotecas na IDE Arduino:
   - U8g2
   - RTClib
   - ESP32Encoder
3. Selecione a placa ESP32 e a porta serial.
4. Compile e grave o firmware.
5. Abra o monitor serial em 115200 para acompanhar logs.
6. Gire o encoder para abrir o menu e testar irrigacao manual.
7. Se quiser usar dashboard web, conecte no AP e abra a URL exibida na tela WEBSERVER.

## 4. ⚙️ Configuracoes principais

Constantes relevantes em Config.h:

- NUM_VALVULAS = 8
- MAX_AGENDAS_TOTAIS = 4
- MAX_AGENDAS_POR_SETOR = 4 (referencia contratual)
- DURACAO_PADRAO_MIN = 10
- TIMEOUT_MANUAL_MS = 600000 (10 minutos)
- MAX_SETOR_SIMULTANEOS_AGENDA = 2
- INTERVALO_LOTE_AGENDA_MS = 10000
- BAUD_RATE = 115200
- DEBUG_SERIAL = false

Rede/AP:

- WIFI_AP_SSID, WIFI_AP_PASSWORD
- WIFI_STA_ENABLED, WIFI_STA_SSID, WIFI_STA_PASSWORD
- WIFI_STA_RETRY_MS

## 5. 🧱 Arquitetura

Separacao adotada:

- Drivers: acesso ao hardware.
- Managers/Controllers: regras de negocio e estado.
- Arquivo .ino: orquestracao do ciclo principal.

Modulos:

- encoder_driver.*: leitura do encoder, debounce, clique curto e longo.
- display_driver_oled.*: primitivas de desenho no OLED.
- rtc_driver_ds3231.*: leitura/ajuste de data e hora.
- runtime_config_manager.*: configuracoes runtime (timeout manual e duracao padrao) persistidas em NVS.
- menu_controller.*: maquina de estados do menu, programacao e configuracoes.
- display_manager.*: composicao das telas, incluindo status de agenda sequencial e tela WEBSERVER.
- irrigation_controller.*: acionamento de reles, origem manual/agenda e deadlines.
- schedule_manager.*: CRUD de agenda, validacao, NVS, proxima execucao e disparo por janela de lote.
- web_ap_manager.*: AP Wi-Fi, servidor HTTP e API do dashboard.
- IrrigacaoInteligente.ino: setup e loop.

## 6. 🔄 Fluxo do firmware

### 6.1 🚀 Setup

1. Inicializa Serial (se debug ativo).
2. Inicializa I2C (Wire.begin).
3. Inicializa encoder e display.
4. Inicializa RTC (se indisponivel, continua sem hora real).
5. Inicializa runtime config e schedule manager.
6. Inicializa irrigacao, web AP manager e display manager.

### 6.2 🔁 Loop

1. Atualiza encoder.
2. Le direcao e botoes.
3. Processa menu.
4. Em irrigacao manual, clique curto faz toggle do setor selecionado.
5. Em teste de valvulas (Configuracoes), clique curto faz toggle do setor em teste.
6. Se houver exclusao de agenda, cancela execucao automatica em andamento.
7. Atualiza irrigacao (timeouts e fechamento por deadline).
8. Se RTC ativo, avalia disparos de agenda do minuto atual e enfileira setores.
9. Processa execucao sequencial por lotes (limite de simultaneos + intervalo entre lotes).
10. Publica estado da agenda sequencial para o display.
11. Mantem servidor HTTP responsivo.
12. Atualiza display.

## 7. 🕹️ Operacao da interface

Menu principal:

- Irrigar Agora
- Programar
- WEBSERVER
- Configuracoes

Encoder:

- Giro: navega itens e campos.
- Clique curto: seleciona/edita.
- Clique longo: voltar/cancelar conforme a tela.

Irrigacao manual:

- Seleciona setor de 1 a 8.
- Existe item Voltar na navegacao (apos o setor 8).
- Clique curto abre/fecha rele do setor.
- Timeout manual fecha automaticamente apos o tempo configurado.

Programar:

- 4 slots globais de agenda.
- Submenu com editar hora/minuto/duracao/dias/setores, salvar, excluir e voltar.

Configuracoes:

- Submenu Relogio: hora, minuto, dia, mes, ano e timeout manual.
- Submenu Sistema: duracao padrao, teste de valvulas, limpar agendas, restaurar padrao e info sistema.

## 8. 📅 Agendamento

Modelo implementado atualmente:

- Ate 4 agendas globais (MAX_AGENDAS_TOTAIS = 4).
- Cada agenda possui diasMask (DOM..SAB) e setoresMask (setores 1..8).
- Cada agenda pode acionar mais de um setor no mesmo horario.

Validacoes:

- Hora: 0..23
- Minuto: 0..59
- Duracao: >= 1
- Minimo 1 dia selecionado
- Minimo 1 setor selecionado
- Sem duplicidade exata (hora + minuto + dias + setores)

Motor de execucao:

- Verificacao por minuto (evita repeticao no mesmo minuto).
- Identifica lote atual considerando horario de inicio, duracao e intervalo entre lotes.
- Respeita limite de simultaneos (MAX_SETOR_SIMULTANEOS_AGENDA).
- Em conflito no mesmo setor/minuto, aplica maior duracao.
- Exclusao de agenda cancela execucao automatica em andamento.

## 9. 🌐 Dashboard web

Comportamento:

- Inicializa AP Wi-Fi no boot.
- Se STA estiver habilitada, tenta conexao periodicamente sem bloquear AP.
- Exibe pagina web local para status, valvulas, agendas e configuracoes runtime.

Rotas principais:

- GET /, GET /status
- GET /api/status, GET /api/schedules
- POST /api/valve/toggle, /api/valve/set, /api/valves/off-all
- POST /api/schedule/save, /api/schedule/delete, /api/schedule/clear
- POST /api/config/runtime
- POST /api/rtc/set

## 10. 💾 Persistencia

Persistencia com Preferences (NVS):

- Agenda:
  - namespace: irrig_sched
  - chave: bank
  - conteudo: versao + crc + vetor de agendas
- Config runtime:
  - namespace: irrig_cfg
  - chave: cfg
  - conteudo: versao + crc + timeoutManualMs + duracaoPadraoMin

No boot:

- Se versao/CRC/leitura estiver invalida, reinicializa banco padrao seguro.

## 11. 🗂️ Estrutura do projeto

- IrrigacaoInteligente.ino - entrada do firmware
- Config.h - configuracoes globais
- encoder_driver.h/.cpp - leitura do encoder
- display_driver_oled.h/.cpp - driver OLED
- display_manager.h/.cpp - renderizacao de telas
- rtc_driver_ds3231.h/.cpp - RTC DS3231
- menu_controller.h/.cpp - navegacao, programacao e configuracoes
- schedule_manager.h/.cpp - agenda e persistencia de agenda
- runtime_config_manager.h/.cpp - persistencia de configuracoes runtime
- irrigation_controller.h/.cpp - controle de valvulas/reles
- web_ap_manager.h/.cpp - AP Wi-Fi e dashboard web
- GUIA_DIDATICO_PROJETO.md - guia pedagogico
- FASE5_CONTRATO_TECNICO.md - contrato tecnico

## 12. 🛠️ Build e gravacao

1. Abra o projeto na IDE Arduino.
2. Selecione a placa ESP32 correta.
3. Verifique se as bibliotecas estao instaladas.
4. Compile o projeto.
5. Grave no ESP32.
6. Abra o monitor serial em 115200.

## 13. 🩺 Troubleshooting rapido

Problemas comuns e verificacoes:

- OLED nao liga:
  - confira SDA/SCL (21/22)
  - confira alimentacao e GND
- Encoder nao responde:
  - confira pinos 19/18/4
  - confira pull-up do botao
- RTC nao encontrado:
  - o sistema continua, mas sem hora real
  - confira conexoes I2C e bateria do DS3231
- Agenda nao dispara:
  - confirme se o RTC esta ativo
  - confirme se o dia atual esta marcado
  - confirme hora/minuto e setoresMask da agenda
- Dashboard nao abre:
  - confira SSID/senha do AP em Config.h
  - confirme IP mostrado na tela WEBSERVER

## 14. ✅ Validacao recomendada

Teste funcional minimo:

1. Navegar menu e abrir/fechar setores manualmente.
2. Validar timeout manual.
3. Criar agenda valida.
4. Reiniciar e verificar persistencia.
5. Simular horario e validar disparo automatico.
6. Tentar criar duplicata e confirmar bloqueio.
7. Testar comportamento com RTC ausente.
8. Validar dashboard web (status, toggle de valvula, salvar agenda).

Checklist de aceite rapido:

- Sem travamentos na navegacao.
- Setor acionado corresponde ao selecionado.
- Dados permanecem apos reboot.
- Disparo ocorre apenas no minuto esperado.
- Dashboard responde sem erro nas rotas principais.

## 15. 🛣️ Limites atuais e roadmap

Estado atual:

- Sistema funcional com 4 agendas globais por mascara de setores.
- Tela de configuracoes com multiplas etapas e fluxo profundo.

Ponto de alinhamento com contrato:

- Contrato original da fase descreve 4 setores x 4 agendas por setor (16 agendas).
- Codigo atual adota 4 agendas totais globais.

Proximos passos sugeridos:

- Definir oficialmente o modelo final de agenda.
- Evoluir UX de configuracoes para reduzir profundidade de navegacao.
- Adicionar testes automatizados para agenda, persistencia e API web.
- Criar checklist de homologacao em campo (eletrica, rede e UX).

---

Ultima revisao deste documento: 2026-04-12