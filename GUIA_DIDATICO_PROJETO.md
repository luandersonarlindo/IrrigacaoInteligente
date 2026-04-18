# 📘 Guia Didatico do Projeto Irrigacao Inteligente

Este documento explica o projeto de forma simples, para ensino de alunos de 15 a 16 anos.

## 👀 Visao rapida

- ✅ Projeto com ESP32 para irrigacao manual e automatica.
- 🖥️ Interface local com OLED e encoder.
- 🧭 Menu com acesso a Irrigar Agora, Programar, WEBSERVER e Configuracoes.
- 💧 Controle fisico de 8 valvulas por rele.
- 💾 Agendas salvas na memoria flash (NVS).
- 🌐 Dashboard web no celular/computador via Wi-Fi AP.
- 🚨 Dashboard com alertas ativos e historico de eventos.

## 🧭 Sumario

1. 🎯 Escopo
2. 🧰 Hardware
3. ⚙️ Configuracoes principais
4. 🧱 Arquitetura
5. 🔄 Fluxo do firmware
6. 🕹️ Operacao da interface
7. 📅 Agendamento
8. 🌐 Dashboard web
9. 💾 Persistencia
10. 🗂️ Estrutura do projeto
11. 🎓 Resumo pedagogico
12. 🚀 Proximos passos didaticos

## 1. 🎯 Escopo

Este projeto entrega:

- Irrigacao manual por setor.
- Irrigacao automatica por agenda semanal.
- Persistencia de dados apos reiniciar o ESP32.
- Estrutura modular para facilitar manutencao e ensino.

## 2. 🧰 Hardware

Componentes principais:

- ESP32
- Encoder rotativo com botao
- OLED SSD1306 (I2C)
- RTC DS3231 (I2C)
- 2 modulos de rele 4 canais (8 canais no total)

Pinos importantes (Config.h):

- Encoder CLK: 18
- Encoder DT: 19
- Encoder BTN: 4
- OLED SDA/SCL: 21/22

Observacao importante:

- OLED e RTC compartilham o barramento I2C.

## 3. ⚙️ Configuracoes principais

As configuracoes ficam em Config.h.

Exemplos importantes:

- Quantidade de valvulas: 8
- Limite de agendas globais: 4
- Duracao padrao: 10 min
- Timeout manual de seguranca: 10 min
- Maximo de setores simultaneos por lote da agenda: 2
- Faixa de timeout manual em runtime: 1..120 min
- Faixa de duracao padrao em runtime: 1..240 min

Ideia para aula:

- Se for trocar pino ou limite, altere primeiro no Config.h.
- O timeout de inatividade do menu usa o valor runtime de timeout manual.

## 4. 🧱 Arquitetura

Regra de organizacao:

- Driver = conversa com hardware.
- Manager/Controller = decide logica.
- .ino = conecta os modulos e executa o ciclo principal.

### Modulos e papeis

- encoder_driver.*: leitura do giro e botoes.
- display_driver_oled.*: desenho no display.
- rtc_driver_ds3231.*: hora e data.
- runtime_config_manager.*: salva timeout e duracao padrao em NVS.
- menu_controller.*: estados do menu.
- display_manager.*: o que mostrar na tela.
- irrigation_controller.*: abrir/fechar valvulas e controlar tempo.
- schedule_manager.*: salvar, validar e disparar agendas.
- web_ap_manager.*: cria AP Wi-Fi e dashboard web.
- IrrigacaoInteligente.ino: setup e loop.

## 5. 🔄 Fluxo do firmware

No loop principal:

1. Le encoder.
2. Processa menu.
3. Executa irrigacao manual (quando houver clique curto na tela manual).
4. Se sair do teste de valvulas, fecha todas as valvulas por seguranca.
5. Atualiza irrigacao (timeout e deadlines).
6. Verifica disparos de agenda por minuto (RTC).
7. Processa lotes da agenda (simultaneos + intervalo).
8. Atualiza servidor web.
9. Atualiza display.

Resumo:

- entrada do usuario -> decisao da logica -> acao nos reles -> atualizacao da tela e da web

## 6. 🕹️ Operacao da interface

Menu principal:

- Irrigar Agora
- Programar
- WEBSERVER
- Configuracoes

Comandos do encoder:

- Giro: navega entre itens/campos.
- Clique curto: seleciona ou altera.
- Clique longo: volta/confirma dependendo da tela.

Na irrigacao manual:

- Escolhe setor de 1 a 8.
- Existe item Voltar na lista.
- Clique curto abre/fecha rele.

Nas configuracoes:

- Ajusta relogio.
- Ajusta timeout manual e duracao padrao.
- Faz teste de valvulas.
- Limpa agendas e restaura padrao.

## 7. 📅 Agendamento

O modulo schedule_manager faz:

- Criar, editar e remover agendas.
- Validar hora, minuto, duracao, dias e setores.
- Evitar agenda duplicada.
- Disparar irrigacao no minuto correto.
- Retomar lote correto quando o horario atual estiver dentro da janela da agenda.
- Evitar repeticao do mesmo slot no mesmo dia apos disparo.

Modelo atual:

- Ate 4 agendas globais.
- Cada agenda pode controlar varios setores por mascara de bits.
- A execucao e sequencial por lotes quando ha muitos setores.

## 8. 🌐 Dashboard web

O modulo web_ap_manager faz:

- Criar rede Wi-Fi AP do sistema.
- Exibir dashboard com status de valvulas e agendas.
- Permitir comandos web para valvulas e agendas.
- Permitir ajuste de runtime e RTC por API.
- Exibir alertas ativos e historico de registros do sistema em tempo real.
- Disponibilizar endpoint /api/events para alertas e historico.

Uso didatico:

- Alunos podem comparar controle local (encoder) com controle remoto (web).

## 9. 💾 Persistencia

As agendas e configuracoes runtime sao salvas na NVS (flash do ESP32).

- Usa versao + CRC para garantir integridade.
- Se os dados estiverem invalidos no boot, o sistema reinicia banco seguro.

## 10. 🗂️ Estrutura do projeto

- IrrigacaoInteligente.ino
- Config.h
- encoder_driver.h/.cpp
- display_driver_oled.h/.cpp
- display_manager.h/.cpp
- rtc_driver_ds3231.h/.cpp
- runtime_config_manager.h/.cpp
- menu_controller.h/.cpp
- schedule_manager.h/.cpp
- irrigation_controller.h/.cpp
- web_ap_manager.h/.cpp
- README.md
- GUIA_DIDATICO_PROJETO.md
- FASE5_CONTRATO_TECNICO.md

## 11. 🎓 Resumo pedagogico

- O projeto e modularizado.
- Cada modulo resolve um problema especifico.
- Isso ajuda em ensino, manutencao e evolucao do sistema.

Correcoes importantes deste guia (alinhadas ao codigo atual):

- Nao e simulacao: os reles sao acionados de verdade.
- Ja existe modulo de agenda com persistencia (schedule_manager).
- Ja existe dashboard web com AP dedicado (web_ap_manager).
- O loop principal inclui disparo automatico por minuto e execucao por lotes.
- Timeout manual e duracao padrao possuem limites de seguranca no firmware.

## 12. 🚀 Proximos passos didaticos

- Atividade 1: desenhar diagrama de blocos do fluxo completo.
- Atividade 2: criar nova opcao de menu (ex.: Manutencao).
- Atividade 3: mostrar no display o tempo restante de cada setor.
- Atividade 4: comparar modelo de agenda global vs por setor.
- Atividade 5: criar testes de API para validar rotas do dashboard.

---

Ultima revisao deste guia: 2026-04-17