# 🌱 IrrigacaoInteligente

Sistema de irrigacao inteligente com ESP32, display LCD 16x2 I2C, 3 botoes de navegacao, RTC DS3231 e controle de ate 8 valvulas por rele.

Este README descreve o estado atual implementado no firmware.

## 👀 Visao rapida

- ✅ Controle manual de setores no menu.
- 🗓️ Agendamento automatico semanal com execucao sequencial por lotes.
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
9. 🔒 Interface local
10. 💾 Persistencia
11. 🗂️ Estrutura do projeto
12. 🛠️ Build e gravacao
13. 🩺 Troubleshooting rapido
14. ✅ Validacao recomendada
15. 🛣️ Limites atuais e roadmap

## 1. 🎯 Escopo

O firmware entrega:

- Irrigacao manual por setor com feedback no LCD 16x2.
- Irrigacao automatica por agenda semanal.
- Controle fisico de reles (trigger HIGH).
- Persistencia de agendas e configuracoes runtime apos reboot.
- Regras de seguranca e validacao para evitar configuracoes invalidas.

Documentos complementares:

- GUIA_DIDATICO_PROJETO.md
- FASE5_CONTRATO_TECNICO.md

## 2. 🧰 Hardware

Plataforma:

- ESP32
- 3 botoes de navegacao (Cima, Baixo, Selecionar)
- LCD 16x2 com modulo I2C (PCF8574)
- RTC DS3231 I2C
- 2 modulos de rele 4 canais (8 canais no total)

Mapeamento de pinos (Config.h):

| Recurso           | Pino |
| ----------------- | ---- |
| Botao Cima        | 19   |
| Botao Baixo       | 18   |
| Botao Selecionar  | 4    |
| I2C SDA (LCD/RTC) | 21   |
| I2C SCL (LCD/RTC) | 22   |
| Rele 1            | 23   |
| Rele 2            | 25   |
| Rele 3            | 26   |
| Rele 4            | 27   |
| Rele 5            | 32   |
| Rele 6            | 33   |
| Rele 7            | 13   |
| Rele 8            | 14   |

Observacao: LCD e DS3231 compartilham o barramento I2C.

## 3. ⚡ Configuracao rapida

Passo a passo curto para colocar o sistema para rodar:

1. Monte os componentes conforme os pinos definidos em Config.h.
2. Instale as bibliotecas na IDE Arduino:
  - LiquidCrystal_I2C
  - RTClib
3. Selecione a placa ESP32 e a porta serial.
4. Compile e grave o firmware.
5. Abra o monitor serial em 115200 para acompanhar logs.
6. Use os botoes para abrir o menu e testar irrigacao manual.
7. Ajuste agendas e configuracoes diretamente pela interface local (LCD + botoes).

## 4. ⚙️ Configuracoes principais

Constantes relevantes em Config.h:

- NUM_VALVULAS = 8
- MAX_AGENDAS_TOTAIS = 4
- MAX_AGENDAS_POR_SETOR = 4 (referencia contratual)
- DURACAO_PADRAO_MIN = 10
- TIMEOUT_MANUAL_MS = 600000 (10 minutos)
- MAX_SETOR_SIMULTANEOS_AGENDA = 2
- INTERVALO_LOTE_AGENDA_MS = 10000
- BUTTON_DEBOUNCE_MS = 50
- BUTTON_LONG_PRESS_MS = 1200
- BAUD_RATE = 115200
- DEBUG_SERIAL = false

## 5. 🧱 Arquitetura

Separacao adotada:

- Drivers: acesso ao hardware.
- Managers/Controllers: regras de negocio e estado.
- Arquivo .ino: orquestracao do ciclo principal.

Modulos:

- input_driver.*: leitura dos 3 botoes, debounce, clique curto e clique longo para voltar.
- display_driver_lcd16x2.*: escrita de linhas no LCD 16x2.
- rtc_driver_ds3231.*: leitura/ajuste de data e hora.
- runtime_config_manager.*: configuracoes runtime (timeout manual e duracao padrao) persistidas em NVS.
- menu_controller.*: maquina de estados do menu, programacao e configuracoes.
- display_manager.*: composicao das telas, incluindo status de agenda sequencial.
- irrigation_controller.*: acionamento de reles, origem manual/agenda e deadlines.
- schedule_manager.*: CRUD de agenda, validacao, NVS, proxima execucao e disparo por janela de lote.
- IrrigacaoInteligente.ino: setup e loop.

## 6. 🔄 Fluxo do firmware

### 6.1 🚀 Setup

1. Inicializa Serial (se debug ativo).
2. Inicializa I2C (Wire.begin).
3. Inicializa entrada local (3 botoes) e display.
4. Inicializa RTC (se indisponivel, continua sem hora real).
5. Inicializa runtime config e schedule manager.
6. Inicializa irrigacao e display manager.

### 6.2 🔁 Loop

1. Atualiza leitura dos 3 botoes.
2. Le direcao, selecionar e voltar (clique longo no Selecionar).
3. Processa menu.
4. Em irrigacao manual, clique curto faz toggle do setor selecionado.
5. Em teste de valvulas (Configuracoes), clique curto faz toggle do setor em teste.
6. Se houver exclusao de agenda, cancela execucao automatica em andamento.
7. Atualiza irrigacao (timeouts e fechamento por deadline).
8. Se RTC ativo, avalia disparos de agenda do minuto atual e enfileira setores.
9. Processa execucao sequencial por lotes (limite de simultaneos + intervalo entre lotes).
10. Publica estado da agenda sequencial para o display.
11. Atualiza display.

## 7. 🕹️ Operacao da interface

Menu principal:

- Irrigar Agora
- Programar
- Configuracoes

Controles (3 botoes):

- Botao Cima: navega para cima/anterior.
- Botao Baixo: navega para baixo/proximo.
- Botao Selecionar: seleciona/edita (clique curto).
- Clique longo no botao Selecionar: retorna/cancela conforme a tela.

Irrigacao manual:

- Seleciona setor de 1 a 8.
- Existe item Voltar na navegacao (apos o setor 8).
- Clique curto abre/fecha rele do setor.
- Timeout manual fecha automaticamente apos o tempo configurado.

Programar:

- 4 slots globais de agenda.
- Submenu com editar hora/minuto/duracao/dias/setores, salvar, excluir e opcao de voltar.

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

## 9. 🔒 Interface local apenas

Nesta branch simplificada, o firmware opera somente com interface local:

- Navegacao via 3 botoes.
- Feedback e configuracao pelo LCD 16x2.
- Sem rede/AP e sem API remota.

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
- input_driver.h/.cpp - leitura dos 3 botoes de navegacao
- display_driver_lcd16x2.h/.cpp - driver LCD 16x2 I2C
- display_manager.h/.cpp - renderizacao de telas
- rtc_driver_ds3231.h/.cpp - RTC DS3231
- menu_controller.h/.cpp - navegacao, programacao e configuracoes
- schedule_manager.h/.cpp - agenda e persistencia de agenda
- runtime_config_manager.h/.cpp - persistencia de configuracoes runtime
- irrigation_controller.h/.cpp - controle de valvulas/reles
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

- LCD nao liga:
  - confira SDA/SCL (21/22)
  - confira alimentacao e GND
- Botoes nao respondem:
  - confira pinos 19/18/4/16
  - confira ligacao GND dos botoes (INPUT_PULLUP)
- RTC nao encontrado:
  - o sistema continua, mas sem hora real
  - confira conexoes I2C e bateria do DS3231
- Agenda nao dispara:
  - confirme se o RTC esta ativo
  - confirme se o dia atual esta marcado
  - confirme hora/minuto e setoresMask da agenda

## 14. ✅ Validacao recomendada

Teste funcional minimo:

1. Navegar menu e abrir/fechar setores manualmente.
2. Validar timeout manual.
3. Criar agenda valida.
4. Reiniciar e verificar persistencia.
5. Simular horario e validar disparo automatico.
6. Tentar criar duplicata e confirmar bloqueio.
7. Testar comportamento com RTC ausente.

Checklist de aceite rapido:

- Sem travamentos na navegacao.
- Setor acionado corresponde ao selecionado.
- Dados permanecem apos reboot.
- Disparo ocorre apenas no minuto esperado.

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
- Adicionar testes automatizados para agenda e persistencia.
- Criar checklist de homologacao em campo (eletrica, rede e UX).

---

Ultima revisao deste documento: 2026-04-13