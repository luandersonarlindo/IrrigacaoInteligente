# Guia Didatico do Projeto Irrigacao Inteligente

Este documento explica o projeto de forma simples, para ensino de alunos de 15 a 16 anos.

## 1) O que este projeto faz

Este projeto e um sistema de irrigacao com ESP32.

Ele ja possui:

- Menu no display OLED.
- Controle por encoder rotativo (girar + clique curto + clique longo).
- Relogio de tempo real (RTC DS3231).
- Controle real de 8 valvulas por rele.
- Agendamento automatico semanal com persistencia em memoria flash.

## 2) Ideia de arquitetura (quem faz o que)

Cada arquivo tem uma responsabilidade. Isso deixa o projeto mais facil de entender e evoluir.

- Driver = conversa com o hardware.
- Controller/Manager = decide regras e comportamento.
- Arquivo .ino = ponto de entrada, so conecta os modulos.

## 3) Fluxo principal do sistema

No loop principal, a ordem e:

1. Ler o encoder (giro e botoes).
2. Atualizar o estado do menu.
3. Em irrigacao manual, ligar/desligar o setor escolhido.
4. Atualizar logica de irrigacao (timeout e deadlines).
5. Verificar disparos de agenda pelo RTC (a cada minuto).
6. Atualizar o display.

Resumo rapido: entrada do usuario -> decisao da logica -> acao nos reles -> saida na tela.

---

## 4) Explicacao arquivo por arquivo

## Config.h

### Para que serve
Centraliza configuracoes globais do projeto.

### O que existe nele

- Pinos do encoder.
- Pinos do OLED (I2C).
- Pinos dos 8 reles.
- Tamanho do display.
- Limites de irrigacao e agenda.
- Tempo de seguranca (timeout manual).
- Configuracao de debug serial.

### Ideia para aula
Se for trocar um pino, altere em um unico lugar.

---

## display_driver_oled.h / display_driver_oled.cpp

### Para que serve
E o driver do display OLED. Ele desenha, mas nao decide o conteudo.

### Metodos principais

- begin(): inicializa display e fonte.
- limpar(): limpa buffer.
- renderizar(): envia buffer para tela.
- desenharTexto / desenharTextoGrande / desenharTextoMini.
- desenharLinha / desenharRetangulo / desenharRetanguloPreenchido.
- larguraTexto() e alturaFonte() para ajudar no layout.

### Ideia para aula
Pense nele como o "pincel" da tela.

---

## display_manager.h / display_manager.cpp

### Para que serve
Decide o que mostrar no OLED com base no estado do menu e no estado do sistema.

### O que ele desenha

- Tela STATUS (hora e data).
- Menu principal com icones.
- Tela de irrigacao manual.
- Tela de programacao de agendas.
- Tela de configuracoes (ainda simples).

### Ideia para aula
Se o driver e o pincel, o manager e o diretor da tela.

---

## encoder_driver.h / encoder_driver.cpp

### Para que serve
Ler direcao e botoes do encoder.

### Conceitos importantes

- Direcao: HORARIO, ANTI_HORARIO, NENHUMA.
- Debounce: evita leituras falsas do botao.
- Clique curto e clique longo como eventos diferentes.

### Ideia para aula
Debounce e um "filtro" para parar o efeito de tremida eletrica.

---

## menu_controller.h / menu_controller.cpp

### Para que serve
Gerenciar a navegacao e a maquina de estados da interface.

### Estados principais

- STATUS
- IRRIGACAO_MANUAL
- PROGRAMAR
- CONFIGURACOES

### O que ele controla

- Navegacao no menu principal.
- Setor atual na irrigacao manual.
- Etapas da edicao de agenda (hora, minuto, duracao, dias e setores).
- Feedback de erro/sucesso na programacao.

### Ideia para aula
Maquina de estados: cada acao do usuario muda o estado da tela.

---

## rtc_driver_ds3231.h / rtc_driver_ds3231.cpp

### Para que serve
Ler e ajustar data/hora no modulo RTC DS3231.

### Comportamento importante

- Se o RTC nao for encontrado, o sistema continua sem hora real.
- Se o RTC perdeu energia, o codigo ajusta com hora de compilacao como fallback.

### Ideia para aula
RTC e um relogio externo que continua contando mesmo com ESP32 desligado.

---

## irrigation_controller.h / irrigation_controller.cpp

### Para que serve
Controlar valvulas (reles) e seguranca de tempo.

### Funcionalidades atuais

- Controle real dos GPIOs dos reles (trigger HIGH).
- toggleValvula(): abre/fecha no modo manual.
- iniciarAgendamento(): aciona por duracao (minutos).
- atualizar(): fecha quando chegar o deadline.
- Timeout de seguranca no modo manual (10 minutos).

### Ideia para aula
Este modulo ja faz acionamento fisico real de reles.

---

## schedule_manager.h / schedule_manager.cpp

### Para que serve
Gerenciar agendas automaticas e salvar na NVS.

### O que ele faz

- Salvar, ler e remover agendas.
- Validar dados (hora, minuto, duracao, dias e setores).
- Evitar agenda duplicada.
- Persistir com versao + CRC para checar integridade.
- Avaliar disparos por minuto usando horario do RTC.

### Modelo atual

- Ate 4 agendas globais no sistema.
- Cada agenda pode atuar em varios setores (mascara de bits).

### Ideia para aula
Esse modulo funciona como um "banco de dados pequeno" dentro do ESP32.

---

## IrrigacaoInteligente.ino

### Para que serve
E o ponto de entrada do Arduino.

### setup()

- Inicia serial, I2C e modulos do sistema.
- Tenta inicializar RTC.
- Inicia menu, agenda, irrigacao e display.

### loop()

1. Atualiza encoder.
2. Le direcao e botoes.
3. Processa menu.
4. Executa irrigacao manual (quando aplicavel).
5. Atualiza irrigacao (timeouts/deadlines).
6. Dispara agendas automaticas (se RTC disponivel).
7. Atualiza display.

### Ideia para aula
Boa pratica: manter o .ino limpo, apenas orquestrando modulos.

---

## 5) Resumo pedagogico rapido

- O projeto esta modularizado e organizado.
- Cada modulo resolve um tipo de problema.
- Isso ajuda a:
  - testar por partes
  - manter mais facil
  - evoluir sem quebrar tudo

## 6) Correcoes importantes deste guia

Este guia foi atualizado para refletir o codigo real atual:

- Nao e mais simulacao de irrigacao: os reles ja sao acionados fisicamente.
- Existe modulo de agenda (`schedule_manager`) com persistencia na NVS.
- O loop principal inclui verificacao de disparos automaticos por minuto.

## 7) Proximos passos didaticos sugeridos

- Atividade 1: desenhar diagrama de blocos do fluxo completo.
- Atividade 2: criar uma nova tela no menu (ex.: "Sobre o Sistema").
- Atividade 3: mostrar no display quanto tempo falta para cada setor desligar.
- Atividade 4: comparar 2 modelos de agenda (global x por setor) e discutir vantagens.
