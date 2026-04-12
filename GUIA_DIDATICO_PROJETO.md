# Guia Didatico do Projeto Irrigacao Inteligente

Este documento explica o projeto de forma simples, para ensino de alunos de 15 a 16 anos.

## Visao rapida

- Projeto com ESP32 para irrigacao manual e automatica.
- Interface local com OLED e encoder.
- Relogio RTC DS3231 para manter horario.
- Controle fisico de 8 valvulas por rele.
- Agendas salvas na memoria flash (NVS).

## Sumario

1. Escopo
2. Hardware
3. Configuracoes principais
4. Arquitetura
5. Fluxo do firmware
6. Operacao da interface
7. Agendamento
8. Persistencia
9. Estrutura do projeto
10. Resumo pedagogico
11. Proximos passos didaticos

## 1. Escopo

Este projeto entrega:

- Irrigacao manual por setor.
- Irrigacao automatica por agenda semanal.
- Persistencia de dados apos reiniciar o ESP32.
- Estrutura modular para facilitar manutencao e ensino.

## 2. Hardware

Componentes principais:

- ESP32
- Encoder rotativo com botao
- OLED SSD1306 (I2C)
- RTC DS3231 (I2C)
- 2 modulos de rele 4 canais (8 canais no total)

Observacao importante:

- OLED e RTC compartilham o barramento I2C.

## 3. Configuracoes principais

As configuracoes ficam em Config.h.

Exemplos importantes:

- Quantidade de valvulas: 8
- Limite de agendas globais: 4
- Duracao padrao: 10 min
- Timeout manual de seguranca: 10 min

Ideia para aula:

- Se for trocar pino ou limite, altere primeiro no Config.h.

## 4. Arquitetura

Regra de organizacao:

- Driver = conversa com hardware.
- Manager/Controller = decide logica.
- .ino = conecta os modulos e executa o ciclo principal.

### Modulos e papeis

- encoder_driver.*: leitura do giro e botoes.
- display_driver_oled.*: desenho no display.
- rtc_driver_ds3231.*: hora e data.
- menu_controller.*: estados do menu.
- display_manager.*: o que mostrar na tela.
- irrigation_controller.*: abrir/fechar valvulas e controlar tempo.
- schedule_manager.*: salvar, validar e disparar agendas.
- IrrigacaoInteligente.ino: setup e loop.

## 5. Fluxo do firmware

No loop principal:

1. Le encoder.
2. Processa menu.
3. Executa irrigacao manual (quando houver clique curto na tela manual).
4. Atualiza irrigacao (timeout e deadlines).
5. Verifica disparos de agenda por minuto (RTC).
6. Atualiza display.

Resumo:

- entrada do usuario -> decisao da logica -> acao nos reles -> atualizacao da tela

## 6. Operacao da interface

Menu principal:

- Irrigar Agora
- Programar
- Configuracoes

Comandos do encoder:

- Giro: navega entre itens/campos.
- Clique curto: seleciona ou altera.
- Clique longo: volta/confirma dependendo da tela.

Na irrigacao manual:

- Escolhe setor de 1 a 8.
- Clique curto abre/fecha rele.

## 7. Agendamento

O modulo schedule_manager faz:

- Criar, editar e remover agendas.
- Validar hora, minuto, duracao, dias e setores.
- Evitar agenda duplicada.
- Disparar irrigacao no minuto correto.

Modelo atual:

- Ate 4 agendas globais.
- Cada agenda pode controlar varios setores por mascara de bits.

## 8. Persistencia

As agendas sao salvas na NVS (flash do ESP32).

- Usa versao + CRC para garantir integridade.
- Se os dados estiverem invalidos no boot, o sistema reinicia banco seguro.

## 9. Estrutura do projeto

- IrrigacaoInteligente.ino
- Config.h
- encoder_driver.h/.cpp
- display_driver_oled.h/.cpp
- display_manager.h/.cpp
- rtc_driver_ds3231.h/.cpp
- menu_controller.h/.cpp
- schedule_manager.h/.cpp
- irrigation_controller.h/.cpp
- README.md
- GUIA_DIDATICO_PROJETO.md
- FASE5_CONTRATO_TECNICO.md

## 10. Resumo pedagogico

- O projeto e modularizado.
- Cada modulo resolve um problema especifico.
- Isso ajuda em ensino, manutencao e evolucao do sistema.

Correcoes importantes deste guia (alinhadas ao codigo atual):

- Nao e simulacao: os reles sao acionados de verdade.
- Ja existe modulo de agenda com persistencia (schedule_manager).
- O loop principal ja inclui disparo automatico por minuto.

## 11. Proximos passos didaticos

- Atividade 1: desenhar diagrama de blocos do fluxo completo.
- Atividade 2: criar nova opcao de menu (ex.: Sobre o Sistema).
- Atividade 3: mostrar tempo restante de cada setor no display.
- Atividade 4: comparar modelo de agenda global vs por setor.

---

Ultima revisao deste guia: 2026-04-12
