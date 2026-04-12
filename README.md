# IrrigacaoInteligente

Sistema de irrigacao inteligente com ESP32, display OLED, encoder rotativo, RTC DS3231 e controle de ate 8 valvulas por rele.

Este README descreve o estado atual implementado no firmware.

## Visao rapida

- Controle manual de setores no menu.
- Agendamento automatico semanal com dias, hora, minuto, duracao e setores.
- Persistencia de agendas na flash (NVS), com versao e CRC.
- Operacao mesmo sem RTC (sem hora real).
- Timeout de seguranca no modo manual.

## Sumario

1. Escopo
2. Hardware
3. Configuracao rapida
4. Configuracoes principais
5. Arquitetura
6. Fluxo do firmware
7. Operacao da interface
8. Agendamento
9. Persistencia
10. Estrutura do projeto
11. Build e gravacao
12. Troubleshooting rapido
13. Validacao recomendada
14. Limites atuais e roadmap

## 1. Escopo

O firmware entrega:

- Irrigacao manual por setor com feedback no OLED.
- Irrigacao automatica por agenda semanal.
- Controle fisico de reles (trigger HIGH).
- Persistencia de agendas apos reboot.
- Regras de seguranca e validacao para evitar configuracoes invalidas.

Documentos complementares:

- GUIA_DIDATICO_PROJETO.md
- FASE5_CONTRATO_TECNICO.md

## 2. Hardware

Plataforma:

- ESP32
- Encoder rotativo com botao (HW-040 ou equivalente)
- OLED SSD1306 128x64 I2C
- RTC DS3231 I2C
- 2 modulos de rele 4 canais (8 canais no total)

Mapeamento de pinos (Config.h):

| Recurso     | Pino |
| ----------- | ---- |
| Encoder CLK | 18   |
| Encoder DT  | 19   |
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

## 3. Configuracao rapida

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

## 4. Configuracoes principais

Constantes relevantes em Config.h:

- NUM_VALVULAS = 8
- MAX_AGENDAS_TOTAIS = 4
- MAX_AGENDAS_POR_SETOR = 4 (referencia contratual)
- DURACAO_PADRAO_MIN = 10
- TIMEOUT_MANUAL_MS = 600000 (10 minutos)
- BAUD_RATE = 115200
- DEBUG_SERIAL = true

## 5. Arquitetura

Separacao adotada:

- Drivers: acesso ao hardware.
- Managers/Controllers: regras de negocio e estado.
- Arquivo .ino: orquestracao do ciclo principal.

Modulos:

- encoder_driver.*: leitura do encoder, debounce, clique curto e longo.
- display_driver_oled.*: primitivas de desenho no OLED.
- rtc_driver_ds3231.*: leitura/ajuste de data e hora.
- menu_controller.*: maquina de estados do menu e programacao.
- display_manager.*: composicao das telas.
- irrigation_controller.*: acionamento de reles e deadlines.
- schedule_manager.*: CRUD de agenda, validacao, NVS e disparo por minuto.
- IrrigacaoInteligente.ino: setup e loop.

## 6. Fluxo do firmware

### 6.1 Setup

1. Inicializa Serial (se debug ativo).
2. Inicializa I2C (Wire.begin).
3. Inicializa encoder e display.
4. Inicializa RTC (se indisponivel, continua sem hora real).
5. Inicializa menu, schedule manager, irrigacao e display manager.

### 6.2 Loop

1. Atualiza encoder.
2. Le direcao e botoes.
3. Processa menu.
4. Em irrigacao manual, clique curto faz toggle do setor selecionado.
5. Atualiza irrigacao (timeouts e fechamento por deadline).
6. Se RTC ativo, avalia disparos de agenda do minuto atual.
7. Aciona setores disparados por agendamento.
8. Atualiza display.

## 7. Operacao da interface

Menu principal:

- Irrigar Agora
- Programar
- Configuracoes

Encoder:

- Giro: navega itens e campos.
- Clique curto: seleciona/edita.
- Clique longo:
  - em irrigacao manual: volta ao STATUS.
  - em programacao: confirma em etapas especificas.

Irrigacao manual:

- Seleciona setor de 1 a 8.
- Clique curto abre/fecha rele do setor.
- Timeout manual fecha automaticamente apos 10 min.

## 8. Agendamento

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

Disparo:

- Avaliacao por minuto (evita repeticao no mesmo minuto).
- Em conflito no mesmo setor/minuto, aplica maior duracao.

## 9. Persistencia

Persistencia com Preferences (NVS):

- Namespace: irrig_sched
- Chave: bank
- Conteudo: versao + crc + vetor de agendas

No boot:

- Se versao/CRC/leitura estiver invalida, reinicializa banco padrao seguro.

## 10. Estrutura do projeto

- IrrigacaoInteligente.ino - entrada do firmware
- Config.h - configuracoes globais
- encoder_driver.h/.cpp - leitura do encoder
- display_driver_oled.h/.cpp - driver OLED
- display_manager.h/.cpp - renderizacao de telas
- rtc_driver_ds3231.h/.cpp - RTC DS3231
- menu_controller.h/.cpp - navegacao e programacao
- schedule_manager.h/.cpp - agenda e persistencia
- irrigation_controller.h/.cpp - controle de valvulas/reles
- GUIA_DIDATICO_PROJETO.md - guia pedagogico
- FASE5_CONTRATO_TECNICO.md - contrato tecnico

## 11. Build e gravacao

1. Abra o projeto na IDE Arduino.
2. Selecione a placa ESP32 correta.
3. Verifique se as bibliotecas estao instaladas.
4. Compile o projeto.
5. Grave no ESP32.
6. Abra o monitor serial em 115200.

## 12. Troubleshooting rapido

Problemas comuns e verificacoes:

- OLED nao liga:
  - confira SDA/SCL (21/22)
  - confira alimentacao e GND
- Encoder nao responde:
  - confira pinos 18/19/4
  - confira pull-up do botao
- RTC nao encontrado:
  - o sistema continua, mas sem hora real
  - confira conexoes I2C e bateria do DS3231
- Agenda nao dispara:
  - confirme se o RTC esta ativo
  - confirme se o dia atual esta marcado
  - confirme hora/minuto e setoresMask da agenda

## 13. Validacao recomendada

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

## 14. Limites atuais e roadmap

Estado atual:

- Sistema funcional com 4 agendas globais por mascara de setores.
- Tela de configuracoes ainda basica.

Ponto de alinhamento com contrato:

- Contrato da fase descreve 4 setores x 4 agendas por setor (16 agendas).
- Codigo atual adota 4 agendas totais globais.

Proximos passos sugeridos:

- Definir oficialmente o modelo final de agenda.
- Evoluir tela de configuracoes.
- Adicionar testes automatizados para agenda e persistencia.
- Criar checklist de homologacao em campo.

---

Ultima revisao deste documento: 2026-04-12
