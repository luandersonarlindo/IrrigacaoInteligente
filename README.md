# 🌱 IrrigacaoInteligente

Sistema de irrigação inteligente com ESP32, display OLED, encoder rotativo, RTC DS3231 e controle de até 8 válvulas por relé.

Este README descreve o comportamento real implementado no firmware.

## 👀 Visão rápida

- ✅ Controle manual de setores no menu local.
- 🗓️ Agendamento automático semanal com execução sequencial por lotes.
- 🌐 Dashboard web local via AP do ESP32, com tentativas periódicas de STA.
- 🚨 API de alertas e histórico operacional em tempo real.
- 💾 Persistência em NVS com versão e CRC para agendas e configurações runtime.
- 🛡️ Timeout de segurança no modo manual.

## 🧭 Sumário

1. 🎯 Escopo
2. 🧰 Hardware
3. ⚡ Configuração rápida
4. ⚙️ Configurações principais
5. 🧱 Arquitetura
6. 🔄 Fluxo do firmware
7. 🕹️ Operação da interface
8. 📅 Agendamento
9. 🌐 Dashboard web
10. 💾 Persistência
11. 🗂️ Estrutura do projeto
12. 🛠️ Build e gravação
13. 🩺 Troubleshooting rápido
14. ✅ Validação recomendada
15. 🛣️ Limites atuais e roadmap

## 1. 🎯 Escopo

O firmware entrega:

- Irrigação manual por setor com feedback no OLED.
- Irrigação automática por agenda semanal.
- Controle físico de relés (trigger HIGH).
- Dashboard web para status, válvulas, agendas, runtime e RTC.
- Persistência de agendas e configurações runtime após reboot.
- Regras de validação para evitar configurações inválidas.

Documentos complementares:

- `GUIA_DIDATICO_PROJETO.md`
- `FASE5_CONTRATO_TECNICO.md`
- `PLANO_AULAS_12_ENCONTROS.md`

## 2. 🧰 Hardware

Plataforma:

- ESP32
- Encoder rotativo com botão (HW-040 ou equivalente)
- OLED SSD1306 128x64 I2C
- RTC DS3231 I2C
- 2 módulos de relé 4 canais (8 canais no total)

Mapeamento de pinos (Config.h):

| Recurso     | Pino |
| ----------- | ---- |
| Encoder CLK | 18   |
| Encoder DT  | 19   |
| Encoder BTN | 4    |
| OLED SDA    | 21   |
| OLED SCL    | 22   |
| Relé 1      | 23   |
| Relé 2      | 25   |
| Relé 3      | 26   |
| Relé 4      | 27   |
| Relé 5      | 32   |
| Relé 6      | 33   |
| Relé 7      | 13   |
| Relé 8      | 14   |

Observação: OLED e DS3231 compartilham o mesmo barramento I2C.

## 3. ⚡ Configuração rápida

1. Monte os componentes conforme os pinos definidos em `Config.h`.
2. Instale as bibliotecas na IDE Arduino:
   - U8g2
   - RTClib
   - ESP32Encoder
3. Selecione a placa ESP32 e a porta serial.
4. Compile e grave o firmware.
5. Abra o monitor serial em 115200.
6. Navegue com o encoder e teste a irrigação manual.
7. Conecte no AP e acesse a URL mostrada na tela WEBSERVER.

## 4. ⚙️ Configurações principais

Constantes relevantes em `Config.h`:

- `NUM_VALVULAS = 8`
- `MAX_AGENDAS_TOTAIS = 4`
- `MAX_AGENDAS_POR_SETOR = 4` (referência contratual)
- `DURACAO_PADRAO_MIN = 10`
- `TIMEOUT_MANUAL_MS = 600000` (10 min)
- `MAX_SETOR_SIMULTANEOS_AGENDA = 2`
- `INTERVALO_LOTE_AGENDA_MS = 10000`
- `MENU_TIMEOUT_MS = 30000`
- `BAUD_RATE = 115200`
- `DEBUG_SERIAL = false`
- `WIFI_MDNS_ENABLED = true`
- `WIFI_MDNS_HOSTNAME = "irrigacaoesp32"`
- `WIFI_WEBSOCKET_ENABLED = true`
- `WIFI_WEBSOCKET_PORT = 81`
- `WIFI_WEBSOCKET_PUSH_STATUS_MS = 2000`

Faixas aplicadas em runtime:

- Timeout manual: 1..120 min
- Duração padrão: 1..240 min
- Duração por agenda na UI: 1..240 min

Comportamentos importantes:

- O timeout de inatividade do menu usa `MENU_TIMEOUT_MS` (fixo no firmware).
- O timeout manual é configuração runtime separada.

Rede e segredos:

- O firmware tenta incluir `Config_privado.h` automaticamente quando o arquivo existe.
- `Config_privado.h` está no `.gitignore` para evitar versionar credenciais.
- Na ausência dele, são usados os valores padrão de `WIFI_STA_SSID` e `WIFI_STA_PASSWORD` definidos em `Config.h`.

## 5. 🧱 Arquitetura

Separação adotada:

- Drivers: acesso ao hardware.
- Managers/Controllers: regras de negócio e estado.
- Arquivo `.ino`: orquestração do setup e loop.

Módulos:

- `encoder_driver.*`: leitura do encoder, debounce e clique longo.
- `display_driver_oled.*`: primitivas de desenho no OLED.
- `rtc_driver_ds3231.*`: leitura e ajuste de data/hora.
- `runtime_config_manager.*`: persistência de timeout manual e duração padrão.
- `menu_controller.*`: estados do menu, programação e configurações.
- `display_manager.*`: composição das telas e página WEBSERVER no OLED.
- `irrigation_controller.*`: acionamento de relés, origem manual/agenda e deadlines.
- `schedule_manager.*`: CRUD de agenda, validação, NVS, cache diário e disparo.
- `web_ap_manager.*`: AP/STA, servidor HTTP e API do dashboard.
- `IrrigacaoInteligente.ino`: setup e loop principal.

## 6. 🔄 Fluxo do firmware

### 6.1 🚀 Setup

1. Inicializa Serial (se debug ativo).
2. Inicializa I2C (`Wire.begin`).
3. Inicializa encoder e display.
4. Inicializa RTC (se ausente, continua sem hora real).
5. Inicializa runtime config e schedule manager.
6. Inicializa irrigação, web AP manager e display manager.

### 6.2 🔁 Loop

1. Atualiza encoder.
2. Lê direção e botões.
3. Processa menu.
4. Em irrigação manual, clique curto faz toggle do setor selecionado.
5. Em teste de válvulas, clique curto faz toggle do setor em teste.
6. Se houver exclusão de agenda, cancela execução automática em andamento.
7. Ao sair de `TESTE_VALVULAS`, fecha todas as válvulas por segurança.
8. Atualiza irrigação (timeout/deadline).
9. Se RTC ativo, avalia disparos do minuto atual e enfileira setores.
10. Processa execução sequencial por lotes.
11. Publica estado da agenda sequencial para o display.
12. Mantém servidor HTTP responsivo.
13. Atualiza display.

## 7. 🕹️ Operação da interface

Menu principal:

- Irrigar Agora
- Programar
- WEBSERVER
- Configurações

Encoder:

- Giro: navega itens e campos.
- Clique curto: seleciona, confirma ou alterna campo.
- Clique longo: atalho implementado na etapa `EDIT_DIAS` (retorna ao submenu da agenda).

Irrigação manual:

- Seleção de setor 1..8.
- Existe item `Voltar` na navegação.
- Clique curto abre/fecha relé do setor.
- Timeout manual fecha automaticamente após o tempo configurado.

Programar:

- 4 slots globais de agenda.
- Submenu: editar hora/minuto/duração/dias/setores, salvar, excluir e voltar.

Configurações:

- Submenu Relógio: hora, minuto, dia, mês, ano e timeout manual.
- Submenu Sistema: duração padrão, teste de válvulas, limpar agendas, restaurar padrão e informações.

## 8. 📅 Agendamento

Modelo atual:

- Até 4 agendas globais (`MAX_AGENDAS_TOTAIS = 4`).
- Cada agenda possui `diasMask` (DOM..SAB) e `setoresMask` (setores 1..8).
- Cada agenda pode acionar múltiplos setores no mesmo horário.

Validações:

- Hora: 0..23
- Minuto: 0..59
- Duração: >= 1
- Pelo menos 1 dia selecionado
- Pelo menos 1 setor selecionado
- Sem duplicidade exata (hora + minuto + diasMask + setoresMask)

Motor de execução:

- Verificação por minuto (evita repetição no mesmo minuto).
- Retomada de janela ativa no lote correto, com tempo remanescente.
- Respeita `MAX_SETOR_SIMULTANEOS_AGENDA` e `INTERVALO_LOTE_AGENDA_MS`.
- Em conflito no mesmo setor, mantém a maior duração.
- Marca execução diária por slot e persiste cache para evitar repetição indevida após reboot.

## 9. 🌐 Dashboard web

Comportamento:

- Inicializa AP Wi-Fi no boot.
- Se STA estiver habilitada/configurada, tenta reconectar periodicamente sem bloquear o AP.
- Com STA conectada, ativa mDNS para acesso por `http://<hostname>.local/`.
- Publica status em tempo real por WebSocket (porta 81) com fallback para polling HTTP no dashboard.
- Exibe dashboard para status, válvulas, agendas e runtime.
- Exibe alertas ativos e histórico operacional (rede, irrigação, agenda, configuração e RTC).
- Histórico usa buffer circular com até 40 registros.

Observação sobre biblioteca:

- O WebSocket usa a biblioteca `WebSocketsServer` (Links2004/WebSockets).
- Se a biblioteca não estiver instalada, o firmware continua compilando e o dashboard mantém funcionamento por HTTP (polling).

Rotas principais:

- `GET /`
- `GET /status`
- `GET /api/status`
- `GET /api/schedules`
- `GET /api/events`
- `POST /api/valve/toggle`
- `POST /api/valve/set`
- `POST /api/valves/off-all`
- `POST /api/schedule/save`
- `POST /api/schedule/delete`
- `POST /api/schedule/clear`
- `POST /api/config/runtime`
- `POST /api/rtc/set`

Detalhes de contrato:

- `index` (válvula) e `slot` (agenda) aceitam base 1 e base 0.
- `state` em `/api/valve/set` aceita `1/0`, `true/false`, `on/off`, `ligado/desligado`.
- Erros em `/api/*` retornam JSON com `ok=false`.
- Rotas não API inexistentes retornam texto 404.

## 10. 💾 Persistência

Persistência com `Preferences` (NVS):

- Agenda:
  - namespace: `irrig_sched`
  - chave: `bank`
  - conteúdo: versão + CRC + vetor de agendas
- Cache de execução diária:
  - namespace: `irrig_sched`
  - chave: `exec_cache`
  - conteúdo: versão + CRC + última execução por slot
- Config runtime:
  - namespace: `irrig_cfg`
  - chave: `cfg`
  - conteúdo: versão + CRC + timeoutManualMs + duracaoPadraoMin

No boot:

- Se versão/CRC estiver inválido, reinicializa banco padrão seguro.
- Runtime config aplica clamp de segurança ao salvar timeout/duração.

## 11. 🗂️ Estrutura do projeto

- `IrrigacaoInteligente.ino` - entrada do firmware
- `Config.h` - configurações globais
- `Config_privado.h` - segredos locais (não versionado)
- `encoder_driver.h/.cpp`
- `display_driver_oled.h/.cpp`
- `display_manager.h/.cpp`
- `rtc_driver_ds3231.h/.cpp`
- `runtime_config_manager.h/.cpp`
- `menu_controller.h/.cpp`
- `schedule_manager.h/.cpp`
- `irrigation_controller.h/.cpp`
- `web_ap_manager.h/.cpp`
- `README.md`
- `GUIA_DIDATICO_PROJETO.md`
- `FASE5_CONTRATO_TECNICO.md`
- `PLANO_AULAS_12_ENCONTROS.md`

## 12. 🛠️ Build e gravação

1. Abra o projeto na IDE Arduino.
2. Selecione a placa ESP32 correta.
3. Verifique as bibliotecas instaladas.
4. Compile o projeto.
5. Grave no ESP32.
6. Abra o monitor serial em 115200.

## 13. 🩺 Troubleshooting rápido

Problemas comuns:

- OLED não liga:
  - confira SDA/SCL (21/22)
  - confira alimentação e GND
- Encoder não responde:
  - confira pinos 18/19/4
  - confira pull-up do botão
- RTC não encontrado:
  - o sistema continua, mas sem hora real
  - confira I2C e bateria do DS3231
- Agenda não dispara:
  - confirme RTC ativo
  - confirme dia marcado na agenda
  - confirme hora/minuto e `setoresMask`
- Dashboard não abre:
  - confira SSID/senha do AP
  - confirme o IP exibido na tela WEBSERVER
  - se estiver na mesma rede do roteador, teste `http://irrigacaoesp32.local/`

## 14. ✅ Validação recomendada

Teste funcional mínimo:

1. Navegar no menu e abrir/fechar setores manualmente.
2. Validar timeout manual.
3. Criar agenda válida.
4. Reiniciar e verificar persistência.
5. Simular horário e validar disparo automático.
6. Tentar criar duplicata e confirmar bloqueio.
7. Testar comportamento com RTC ausente.
8. Validar dashboard web (status, válvula, agenda, runtime e RTC).

Checklist rápido:

- Sem travamentos na navegação.
- Setor acionado corresponde ao selecionado.
- Dados persistem após reboot.
- Disparo ocorre no minuto esperado.
- Rotas principais respondem sem erro.

## 15. 🛣️ Limites atuais e roadmap

Estado atual:

- Sistema funcional com 4 agendas globais por máscara de setores.
- Fluxo de configurações ainda profundo no OLED.

Ponto de alinhamento com contrato:

- Contrato original descrevia 4 setores x 4 agendas por setor (16 agendas).
- Código atual adota 4 agendas totais globais.

Próximos passos sugeridos:

- Definir oficialmente o modelo final de agenda.
- Evoluir UX de configurações para reduzir profundidade de navegação.
- Adicionar testes automatizados para agenda, persistência e API web.
- Criar checklist de homologação de campo (elétrica, rede e UX).

---

Última revisão deste documento: 2026-04-19