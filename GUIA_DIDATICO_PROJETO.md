# 📘 Guia Didático do Projeto Irrigação Inteligente

Este documento explica o projeto de forma simples, para ensino de alunos de 15 a 16 anos.

## 👀 Visão rápida

- ✅ Projeto com ESP32 para irrigação manual e automática.
- 🖥️ Interface local com OLED e encoder.
- 🧭 Menu com acesso a Irrigar Agora, Programar, WEBSERVER e Configurações.
- 💧 Controle físico de 8 válvulas por relé.
- 💾 Agendas e configurações salvas em memória flash (NVS).
- 🌐 Dashboard web local via Wi-Fi.
- 🚨 Painel com alertas ativos e histórico de eventos.

## 🧭 Sumário

1. 🎯 Escopo
2. 🧰 Hardware
3. ⚙️ Configurações principais
4. 🧱 Arquitetura
5. 🔄 Fluxo do firmware
6. 🕹️ Operação da interface
7. 📅 Agendamento
8. 🌐 Dashboard web
9. 💾 Persistência
10. 🗂️ Estrutura do projeto
11. 🎓 Resumo pedagógico
12. 🚀 Próximos passos didáticos

## 1. 🎯 Escopo

Este projeto entrega:

- Irrigação manual por setor.
- Irrigação automática por agenda semanal.
- Persistência de dados após reiniciar o ESP32.
- Estrutura modular para facilitar manutenção e ensino.

## 2. 🧰 Hardware

Componentes principais:

- ESP32
- Encoder rotativo com botão
- OLED SSD1306 (I2C)
- RTC DS3231 (I2C)
- 2 módulos de relé 4 canais (8 canais no total)

Pinos importantes (`Config.h`):

- Encoder CLK: 18
- Encoder DT: 19
- Encoder BTN: 4
- OLED SDA/SCL: 21/22

Observação importante:

- OLED e RTC compartilham o barramento I2C.

## 3. ⚙️ Configurações principais

As configurações ficam em `Config.h`.

Exemplos importantes:

- Quantidade de válvulas: 8
- Limite de agendas globais: 4
- Duração padrão: 10 min
- Timeout manual de segurança: 10 min
- Máximo de setores simultâneos por lote de agenda: 2
- Faixa de timeout manual em runtime: 1..120 min
- Faixa de duração padrão em runtime: 1..240 min
- Timeout de inatividade do menu: `MENU_TIMEOUT_MS` (fixo em 30 s)

Ideia para aula:

- Se for trocar pino ou limite, altere primeiro no `Config.h`.

## 4. 🧱 Arquitetura

Regra de organização:

- Driver = conversa com hardware.
- Manager/Controller = decide lógica.
- `.ino` = conecta os módulos e executa o ciclo principal.

### Módulos e papéis

- `encoder_driver.*`: leitura do giro e botão.
- `display_driver_oled.*`: desenho no display.
- `rtc_driver_ds3231.*`: hora e data.
- `runtime_config_manager.*`: salva timeout e duração padrão em NVS.
- `menu_controller.*`: estados do menu.
- `display_manager.*`: o que mostrar na tela.
- `irrigation_controller.*`: abrir/fechar válvulas e controlar tempo.
- `schedule_manager.*`: salvar, validar e disparar agendas.
- `web_ap_manager.*`: AP Wi-Fi e dashboard web.
- `IrrigacaoInteligente.ino`: setup e loop.

## 5. 🔄 Fluxo do firmware

No loop principal:

1. Lê encoder.
2. Processa menu.
3. Executa irrigação manual (clique curto na tela manual).
4. Se sair do teste de válvulas, fecha todas as válvulas por segurança.
5. Atualiza irrigação (timeout e deadlines).
6. Verifica disparos de agenda por minuto (RTC).
7. Processa lotes da agenda (simultâneos + intervalo).
8. Atualiza servidor web.
9. Atualiza display.

Resumo:

- Entrada do usuário -> decisão da lógica -> ação nos relés -> atualização da tela e da web.

## 6. 🕹️ Operação da interface

Menu principal:

- Irrigar Agora
- Programar
- WEBSERVER
- Configurações

Comandos do encoder:

- Giro: navega entre itens/campos.
- Clique curto: seleciona ou altera.
- Clique longo: atalho na edição de dias da agenda (retorna ao submenu).

Na irrigação manual:

- Escolhe setor de 1 a 8.
- Existe item Voltar na lista.
- Clique curto abre/fecha relé.

Nas configurações:

- Ajusta relógio.
- Ajusta timeout manual e duração padrão.
- Faz teste de válvulas.
- Limpa agendas e restaura padrão.

## 7. 📅 Agendamento

O módulo `schedule_manager` faz:

- Criar, editar e remover agendas.
- Validar hora, minuto, duração, dias e setores.
- Evitar agenda duplicada.
- Disparar irrigação no minuto correto.
- Retomar lote correto quando o horário atual estiver dentro da janela da agenda.
- Evitar repetição do mesmo slot no mesmo dia, com cache persistido em NVS.

Modelo atual:

- Até 4 agendas globais.
- Cada agenda pode controlar vários setores por máscara de bits.
- A execução é sequencial por lotes quando há muitos setores.

## 8. 🌐 Dashboard web

O módulo `web_ap_manager` faz:

- Criar rede Wi-Fi AP do sistema.
- Exibir dashboard com status de válvulas e agendas.
- Permitir comandos web para válvulas e agendas.
- Permitir ajuste de runtime e RTC por API.
- Exibir alertas ativos e histórico de eventos em tempo real.
- Disponibilizar endpoint `/api/events`.

Uso didático:

- Alunos podem comparar controle local (encoder) com controle remoto (web).

## 9. 💾 Persistência

As agendas e configurações runtime são salvas na NVS (flash do ESP32).

- Usa versão + CRC para garantir integridade.
- Se os dados estiverem inválidos no boot, o sistema reinicia com banco seguro.

## 10. 🗂️ Estrutura do projeto

- `IrrigacaoInteligente.ino`
- `Config.h`
- `Config_privado.h` (opcional, não versionado)
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

## 11. 🎓 Resumo pedagógico

- O projeto é modularizado.
- Cada módulo resolve um problema específico.
- Isso ajuda no ensino, na manutenção e na evolução do sistema.

Correções importantes deste guia (alinhadas ao código atual):

- Não é simulação: os relés são acionados de verdade.
- Já existe módulo de agenda com persistência (`schedule_manager`).
- Já existe dashboard web com AP dedicado (`web_ap_manager`).
- O loop principal inclui disparo automático por minuto e execução por lotes.
- Timeout manual e duração padrão possuem limites de segurança no firmware.

## 12. 🚀 Próximos passos didáticos

- Atividade 1: desenhar diagrama de blocos do fluxo completo.
- Atividade 2: criar nova opção de menu (ex.: Manutenção).
- Atividade 3: mostrar no display o tempo restante de cada setor.
- Atividade 4: comparar modelo de agenda global vs por setor.
- Atividade 5: criar testes de API para validar rotas do dashboard.

---

Última revisão deste guia: 2026-04-19