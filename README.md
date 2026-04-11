# IrrigacaoInteligente

Projeto educacional de irrigacao automatizada com ESP32, display OLED, encoder rotativo, RTC DS3231 e acionamento de reles para valvulas.

Este README descreve o estado atual do codigo (implementado) para servir como documento tecnico do projeto.

## 1. Objetivo

Construir um sistema de irrigacao inteligente que permita:

- Acionamento manual de setores (valvulas) pelo menu.
- Programacao de irrigacao automatica por horario e dias da semana.
- Persistencia de agendas em memoria flash do ESP32 (NVS).
- Interface local em display OLED usando encoder (giro + clique curto + clique longo).

## 2. Visao geral da arquitetura

O projeto esta separado em modulos com responsabilidades claras:

- Drivers: acesso ao hardware.
- Controllers/Managers: logica de negocio e estado da interface.
- Arquivo `.ino`: orquestracao do sistema no `setup()` e `loop()`.

### Modulos principais

- `encoder_driver.*`: leitura de rotacao e botao do encoder, com debounce e clique longo.
- `display_driver_oled.*`: primitives de desenho no OLED (U8g2).
- `rtc_driver_ds3231.*`: leitura/ajuste de data e hora do RTC (RTClib).
- `irrigation_controller.*`: controle das valvulas (reles), incluindo timeout de seguranca.
- `schedule_manager.*`: CRUD de agendas, validacao, persistencia em NVS e avaliacao de disparos por minuto.
- `menu_controller.*`: maquina de estados da navegacao e da tela de programacao.
- `display_manager.*`: renderizacao das telas conforme estado do menu e dados do sistema.
- `Config.h`: constantes globais (pinos, limites, timeouts e debug).
- `IrrigacaoInteligente.ino`: integracao de todos os modulos.

## 3. Hardware e pinos (estado atual)

Definidos em `Config.h`:

- Encoder:
	- `PIN_ENCODER_CLK = 18`
	- `PIN_ENCODER_DT = 19`
	- `PIN_ENCODER_BTN = 4`
- OLED I2C:
	- `PIN_OLED_SDA = 21`
	- `PIN_OLED_SCL = 22`
- RTC DS3231 no mesmo barramento I2C do OLED.
- Reles (8 canais): `23, 25, 26, 27, 32, 33, 13, 14`.

Parametros importantes:

- `NUM_VALVULAS = 8`
- `MAX_AGENDAS_TOTAIS = 4`
- `DURACAO_PADRAO_MIN = 10`
- `TIMEOUT_MANUAL_MS = 600000` (10 min)
- `DEBUG_SERIAL = true`

## 4. Fluxo principal do firmware

No `loop()` a ordem atual e:

1. Atualizar o encoder (debounce e deteccao de clique).
2. Ler eventos do encoder (direcao, clique curto, clique longo).
3. Processar navegacao no menu.
4. Se em irrigacao manual e clique curto: fazer `toggle` da valvula selecionada.
5. Rodar `irrigacao.atualizar()` para fechar valvulas por timeout/deadline.
6. Se RTC disponivel: avaliar disparos da agenda no minuto atual.
7. Para cada setor disparado: iniciar irrigacao automatica com duracao definida.
8. Atualizar o display.

## 5. Funcionalidades implementadas

## 5.1 Navegacao e interface

- Tela STATUS com hora/data.
- Menu principal com 3 itens:
	- Irrigar Agora
	- Programar
	- Configuracoes
- Irrigacao manual:
	- Giro do encoder troca setor.
	- Clique curto alterna valvula do setor selecionado.
	- Clique longo retorna ao STATUS.
- Programacao:
	- Selecao de agenda.
	- Submenu com opcoes de editar/excluir.
	- Edicao de hora, minuto, duracao, dias e setores.
	- Feedback visual de salvo/excluido/erros de validacao.

## 5.2 Irrigacao manual e automatica

- Acionamento real de GPIO para rele (trigger HIGH).
- Ao abrir manualmente, aplica timeout de seguranca (10 min).
- Disparos automaticos por agenda usam deadline por duracao em minutos.
- Se ja estiver aberto e chegar novo disparo, prevalece o maior tempo restante.

## 5.3 Agendamento e persistencia

- Banco de agendas salvo em NVS (`Preferences`).
- Estrutura persistida com:
	- versao
	- crc
	- vetor de agendas
- Na inicializacao:
	- se banco invalido/incompativel, reinicializa padrao e salva.
- Validacoes ao salvar agenda:
	- hora 0..23
	- minuto 0..59
	- duracao >= 1
	- pelo menos 1 dia selecionado
	- pelo menos 1 setor selecionado
	- bloqueio de duplicidade exata
- Avaliacao de disparo por minuto:
	- evita repetir disparo no mesmo minuto com chave de data/hora.

## 6. Modelo de dados da agenda (implementado)

`AgendaSetor` atual:

- `ativa` (bool)
- `hora` (uint8_t)
- `minuto` (uint8_t)
- `duracaoMin` (uint16_t)
- `diasMask` (bits DOM..SAB)
- `setoresMask` (bits de setores 1..8)

Observacao importante: no estado atual, o projeto usa **4 agendas globais** (`MAX_AGENDAS_TOTAIS = 4`) com mascara de setores, em vez de 4 agendas por setor.

## 7. Estrutura de arquivos

- `IrrigacaoInteligente.ino`: ponto de entrada.
- `Config.h`: configuracoes globais.
- `encoder_driver.h/.cpp`: entrada do encoder.
- `display_driver_oled.h/.cpp`: driver OLED.
- `rtc_driver_ds3231.h/.cpp`: driver RTC.
- `irrigation_controller.h/.cpp`: logica de acionamento de valvulas.
- `schedule_manager.h/.cpp`: agendamento e persistencia.
- `menu_controller.h/.cpp`: maquina de estados do menu.
- `display_manager.h/.cpp`: composicao das telas.
- `GUIA_DIDATICO_PROJETO.md`: guia didatico para ensino.
- `FASE5_CONTRATO_TECNICO.md`: contrato tecnico de referencia da fase 5.

## 8. Como usar no dispositivo

1. Ligue o sistema.
2. Aguarde inicializacao dos modulos.
3. Na tela de STATUS, gire o encoder para abrir o menu.
4. Em `Irrigar Agora`:
	 - gire para escolher setor;
	 - clique curto para abrir/fechar;
	 - clique longo para voltar.
5. Em `Programar`:
	 - escolha uma agenda;
	 - edite campos (hora, minuto, duracao, dias, setores);
	 - confirme para salvar.

## 9. Dependencias (Arduino)

Bibliotecas utilizadas no codigo:

- `U8g2`
- `RTClib`
- `ESP32Encoder`
- `Preferences` (core ESP32)
- `Wire` (I2C)

## 10. Logs de depuracao

Com `DEBUG_SERIAL = true`, o firmware envia mensagens no monitor serial com:

- Status de inicializacao de modulos.
- Eventos de menu.
- Mudancas de valvulas.
- Erros de RTC/NVS.

Baud rate: `115200`.

## 11. Estado atual x contrato da Fase 5

O projeto ja implementa grande parte da Fase 5 (edicao, validacao, persistencia e disparo), porem com uma decisao de modelagem atual:

- Implementado hoje: 4 agendas globais com `setoresMask`.
- Contrato tecnico citado: ate 4 setores x 4 agendas por setor (16 agendas).

Se for necessario aderir estritamente ao contrato, o proximo passo e ajustar o modelo de dados e a UI para a estrategia por setor.

## 12. Proximos passos sugeridos

- Alinhar modelo de agenda ao contrato final escolhido (global por mascara ou por setor).
- Expandir testes de campo com RTC ausente/presente.
- Incluir testes automatizados de validacao de agenda e regras de disparo.
- Evoluir tela de configuracoes (ainda placeholder).
