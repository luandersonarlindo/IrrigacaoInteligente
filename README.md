# IrrigacaoInteligente

Sistema de irrigacao inteligente com ESP32, OLED, encoder, RTC DS3231 e controle de ate 8 valvulas por rele.

Documento tecnico do projeto (estado atual implementado).

## Sumario

1. Escopo do projeto
2. Requisitos de hardware
3. Parametros de configuracao
4. Arquitetura de software
5. Fluxo de execucao
6. Interface do usuario (menu e encoder)
7. Agendamento automatico
8. Persistencia de dados
9. Estrutura de arquivos
10. Como montar e executar
11. Validacao e testes recomendados
12. Limites atuais e roadmap

## 1. Escopo do projeto

O firmware entrega:

- Irrigacao manual por setor com feedback visual no OLED.
- Programacao semanal com hora, minuto, duracao, dias e setores.
- Disparo automatico por minuto com base no RTC.
- Persistencia de agendas em flash do ESP32 (NVS).
- Mecanismo de seguranca para desligamento automatico de valvula em modo manual.

## 2. Requisitos de hardware

Plataforma principal:

- ESP32
- Encoder rotativo com botao (HW-040 ou equivalente)
- Display OLED SSD1306 128x64 via I2C
- Modulo RTC DS3231 via I2C
- 2 modulos de rele 4 canais (total 8 canais), acionamento trigger HIGH

Mapeamento de pinos (em Config.h):

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

Observacao: OLED e DS3231 compartilham o mesmo barramento I2C.

## 3. Parametros de configuracao

Constantes relevantes em `Config.h`:

- `NUM_VALVULAS = 8`
- `MAX_AGENDAS_TOTAIS = 4`
- `MAX_AGENDAS_POR_SETOR = 4` (referencia contratual)
- `DURACAO_PADRAO_MIN = 10`
- `TIMEOUT_MANUAL_MS = 600000` (10 minutos)
- `BAUD_RATE = 115200`
- `DEBUG_SERIAL = true`

## 4. Arquitetura de software

### 4.1 Principio de separacao

- Drivers: acessam hardware.
- Controllers/Managers: aplicam regras de negocio e estado.
- Arquivo `.ino`: integra modulos e define ciclo principal.

### 4.2 Modulos e responsabilidades

- `encoder_driver.*`
  - Le direcao de giro.
  - Detecta clique curto e clique longo com debounce.

- `display_driver_oled.*`
  - Inicializa o display (U8g2).
  - Oferece primitivas de desenho (texto, linhas, retangulos, icones).

- `rtc_driver_ds3231.*`
  - Inicializa e le horario atual.
  - Ajusta para hora de compilacao caso RTC tenha perdido energia.

- `menu_controller.*`
  - Maquina de estados da navegacao.
  - Fluxo de irrigacao manual.
  - Fluxo de programacao (edicao e exclusao de agendas).

- `display_manager.*`
  - Decide qual tela desenhar conforme estado atual.
  - Mostra status, menu principal, irrigacao, programacao e mensagens de feedback.

- `schedule_manager.*`
  - CRUD de agendas.
  - Validacao de regras de agenda.
  - Persistencia em NVS com versao e CRC.
  - Avaliacao de disparos por minuto.

- `irrigation_controller.*`
  - Aciona/desaciona pinos de rele.
  - Gerencia timeout manual.
  - Gerencia deadlines de irrigacao por agendamento.

- `IrrigacaoInteligente.ino`
  - Inicializa tudo no `setup()`.
  - Executa ciclo completo no `loop()`.

## 5. Fluxo de execucao

### 5.1 Setup

1. Inicia serial (se debug ativo).
2. Inicia I2C (`Wire.begin`).
3. Inicializa encoder e display.
4. Inicializa RTC (opcional: sistema continua sem RTC).
5. Inicializa menu, agenda, irrigacao e display manager.

### 5.2 Loop principal

1. Atualiza encoder.
2. Le direcao, clique curto e clique longo.
3. Processa menu e navegacao.
4. Na tela de irrigacao manual: clique curto faz toggle da valvula selecionada.
5. Atualiza controlador de irrigacao (timeouts e fechamento por deadline).
6. Se RTC disponivel: verifica disparos de agendas no minuto atual.
7. Aciona irrigacao automatica por setor quando houver disparo.
8. Atualiza o display.

## 6. Interface do usuario (menu e encoder)

Menu principal:

- Irrigar Agora
- Programar
- Configuracoes

Controles:

- Girar encoder: navega itens/campos.
- Clique curto: seleciona/edita/aciona.
- Clique longo:
  - em irrigacao manual: retorna para STATUS.
  - em programacao: confirma salvamento na etapa de dias/setores.

Tela de irrigacao manual:

- Seleciona setor de 1 a 8.
- Clique curto abre/fecha rele do setor selecionado.
- Timeout de seguranca fecha automaticamente apos 10 min quando origem for manual.

## 7. Agendamento automatico

Modelo atual implementado:

- Ate 4 agendas globais (`MAX_AGENDAS_TOTAIS = 4`).
- Cada agenda pode selecionar multiplos setores por mascara de bits (`setoresMask`).
- Dias da semana em mascara de bits (`diasMask`, DOM..SAB).

Regras de validacao atuais:

- Hora valida: `0..23`
- Minuto valido: `0..59`
- Duracao minima: `>= 1`
- Necessario ao menos 1 dia selecionado
- Necessario ao menos 1 setor selecionado
- Nao permite duplicidade exata (hora + minuto + dias + setores)

Regras de disparo:

- Verificacao por minuto (nao repete no mesmo minuto).
- Se houver conflitos para o mesmo setor no mesmo minuto, aplica maior duracao.
- Se valvula ja estiver aberta, preserva o maior deadline.

## 8. Persistencia de dados

Implementacao com `Preferences` (NVS):

- Namespace: `irrig_sched`
- Chave principal: `bank`
- Estrutura persistida inclui:
  - versao do banco
  - CRC de integridade
  - vetor de agendas

No boot:

- Se leitura/versao/CRC falhar, banco e reinicializado para padrao seguro (agendas inativas).

## 9. Estrutura de arquivos

- `IrrigacaoInteligente.ino` - entrada do firmware
- `Config.h` - configuracoes globais
- `encoder_driver.h/.cpp` - driver do encoder
- `display_driver_oled.h/.cpp` - driver do OLED
- `display_manager.h/.cpp` - logica de telas
- `rtc_driver_ds3231.h/.cpp` - driver RTC
- `menu_controller.h/.cpp` - estado de navegacao e programacao
- `schedule_manager.h/.cpp` - agenda, validacao e NVS
- `irrigation_controller.h/.cpp` - controle de reles/valvulas
- `GUIA_DIDATICO_PROJETO.md` - visao didatica para ensino
- `FASE5_CONTRATO_TECNICO.md` - contrato tecnico de referencia

## 10. Como montar e executar

1. Monte o hardware conforme mapeamento de pinos.
2. Instale as bibliotecas Arduino:
   - `U8g2`
   - `RTClib`
   - `ESP32Encoder`
3. Selecione placa ESP32 na IDE.
4. Compile e grave o firmware.
5. Abra o monitor serial em `115200` para diagnostico.

## 11. Validacao e testes recomendados

Teste funcional minimo:

1. Navegar no menu e alternar setores manualmente.
2. Confirmar fechamento automatico por timeout manual.
3. Criar agenda com dia e setor validos.
4. Reiniciar o ESP32 e confirmar persistencia da agenda.
5. Simular horario de disparo e confirmar acionamento.
6. Testar regra de duplicidade de agenda.
7. Testar comportamento com RTC ausente.

## 12. Limites atuais e roadmap

Estado atual:

- O sistema esta funcional com 4 agendas globais por mascara de setores.
- Tela de configuracoes ainda esta em placeholder.

Ponto de alinhamento com contrato:

- O contrato da Fase 5 descreve 4 setores x 4 agendas por setor (16 agendas).
- O codigo atual adota 4 agendas totais globais.

Proximos passos sugeridos:

- Definir oficialmente o modelo final (global por mascara ou por setor).
- Evoluir a tela de configuracoes.
- Adicionar testes automatizados para regras de agenda e persistencia.
- Criar checklist de homologacao em campo (eletrica, RTC, rele e UX).
