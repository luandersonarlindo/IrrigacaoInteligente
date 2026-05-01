# Plano de Aula - Irrigação Inteligente com ESP32 (12 encontros) — V.3

Público: adolescentes de 14 a 17 anos  
Duração sugerida por aula: 80 a 100 minutos  
Objetivo geral: sair do básico de Arduino/ESP32 e chegar ao entendimento completo do projeto IrrigacaoInteligente.

Versão do projeto (branch IrrigacaoInteligente-V.3): interface local no LCD 16x2 com 3 botões, sem dashboard web.

## Como usar este plano

- Cada aula já vem com roteiro de slides.
- Cada slide foi pensado para ter pouco texto.
- Use, no máximo, 3 bullets por slide.
- Priorize demonstração real e prática curta em toda aula.

## Padrão visual sugerido para todos os slides

- 1 título curto por slide.
- 1 ideia principal por slide.
- 1 imagem forte ocupando boa parte da tela.
- Texto grande (leitura fácil no fundo da sala).
- Evite parágrafos longos.

## Aula 1 - O que é Arduino e por que ESP32?

Objetivo:
- Entender o que é microcontrolador e para que serve no mundo real.

Slides (6):
1. Título: Bem-vindos ao projeto de irrigação inteligente
- Problema real: regar no horário certo
- Solução: automação com ESP32

2. Título: O que é um microcontrolador?
- Um mini computador para tarefas específicas
- Lê sensores e controla saídas

3. Título: Arduino x ESP32
- Arduino UNO: bom para começar
- ESP32: mais poder e Wi-Fi

4. Título: O que nosso projeto faz
- Controle manual de 8 válvulas
- Agenda semanal automática
- Interface local no LCD 16x2

5. Título: Visão geral da arquitetura
- Hardware + firmware + interface local
- Tudo conectado em um loop principal

6. Título: Combinados do curso
- Aprender fazendo
- Errar faz parte
- Segurança sempre

Atividade prática (15 min):
- Mostrar a placa ESP32 real e identificar pinos de energia, GPIO e USB.

## Aula 2 - Eletricidade básica e segurança

Objetivo:
- Compreender tensão, corrente e cuidados ao ligar relés e válvulas.

Slides (6):
1. Título: Conceitos básicos
- Tensão (V), corrente (A), potência (W)
- Fórmula-chave: P = V x I

2. Título: Sinal lógico e mundo físico
- ESP32 trabalha com sinal digital
- Relé conecta carga maior

3. Título: O que é um relé
- Interruptor controlado eletronicamente
- Isolamento entre controle e carga

4. Título: Cuidados importantes
- Fonte correta
- GND comum quando necessário
- Não tocar em ligação energizada

5. Título: Erros comuns em bancada
- Cabo invertido
- Falta de alimentação
- Mau contato no protoboard

6. Título: Mini roteiro de teste seguro
- Conferir ligações
- Energizar
- Testar um canal por vez

Atividade prática (20 min):
- Simular no quadro o caminho da energia até uma válvula.

## Aula 3 - Componentes do projeto na prática

Objetivo:
- Entender o papel de LCD 16x2, botões, RTC DS3231, DHT11 e relés.

Slides (6):
1. Título: Componentes principais
- ESP32
- LCD 16x2 + 3 botões
- RTC, DHT11 e relés

2. Título: LCD 16x2 I2C
- Mostra menu e estados
- Usa barramento I2C

3. Título: RTC DS3231
- Mantém hora precisa
- Base para agendamento

4. Título: DHT11
- Temperatura e umidade
- Leitura periódica e simples

5. Título: Botões de navegação
- Cima/baixo navega
- Selecionar confirma/ativa
- Clique longo volta/cancela

6. Título: Mapa de pinos
- Tudo centralizado em Config.h
- Fácil de manter e evoluir

Atividade prática (20 min):
- Alunos montam um mapa de conexões em papel A4.

## Aula 4 - Primeiros passos no código

Objetivo:
- Ler o projeto sem medo e entender modularização.

Slides (7):
1. Título: Estrutura de pastas e arquivos
- Arquivo principal `.ino`
- Módulos `.h` e `.cpp`

2. Título: O papel do arquivo `.ino`
- Setup inicializa tudo
- Loop roda para sempre

3. Título: Driver vs Manager
- Driver fala com hardware
- Manager decide regra de negócio

4. Título: Módulos do projeto
- menu_controller
- schedule_manager
- irrigation_controller

5. Título: Config.h e parâmetros globais
- Pinos e limites
- Regras de runtime e limites

6. Título: Leitura de código para iniciantes
- Primeiro nomes de funções
- Depois fluxo de chamadas

7. Título: Dica de estudo
- Entenda primeiro o fluxo
- Depois detalhe módulo por módulo

Atividade prática (20 min):
- Em dupla, cada grupo explica 1 módulo em 1 minuto.

## Aula 5 - Entrada do usuário (3 botões + menu)

Objetivo:
- Entender como o usuário controla o sistema localmente.

Slides (6):
1. Título: Jornada do usuário
- Botão cima/baixo navega
- Botão selecionar confirma
- Ação executa

2. Título: Estados de menu
- STATUS
- IRRIGACAO_MANUAL
- PROGRAMAR / CONFIGURACOES

3. Título: Clique curto e clique longo
- Curto: selecionar/alternar
- Longo: voltar/cancelar

4. Título: UX no LCD 16x2
- Texto curto
- Cursor claro
- Feedback imediato

5. Título: Erros de navegação comuns
- Pular item sem querer
- Entrar em tela errada

6. Título: Boas práticas de interface embarcada
- Simples
- Rápida
- Previsível

Atividade prática (20 min):
- Simular navegação de menu sem hardware (jogo de papéis em sala).

## Aula 6 - Controle manual de irrigação

Objetivo:
- Entender como uma válvula abre, fecha e respeita timeout de segurança.

Slides (6):
1. Título: Modo manual
- Escolher setor
- Ligar/desligar com clique

2. Título: Estado da válvula
- ABERTA
- FECHADA
- Origem manual ou agenda

3. Título: Timeout manual
- Evita válvula esquecida aberta
- Valor configurável em runtime

4. Título: Segurança ao sair do teste
- Fechamento automático de válvulas
- Evita consumo indevido

5. Título: Conflito manual x automático
- Sistema usa o maior deadline
- Mantém operação segura

6. Título: Resultado esperado
- Controle confiável e previsível

Atividade prática (25 min):
- Teste guiado: abrir setor, aguardar timeout e verificar fechamento.

## Aula 7 - Tempo real e loop principal

Objetivo:
- Entender o ciclo do firmware e o papel do RTC.

Slides (7):
1. Título: Por que o relógio importa?
- Agenda depende de hora correta
- RTC garante referência estável

2. Título: RTC DS3231 no projeto
- Lê data e hora
- Pode ajustar pela interface

3. Título: Loop principal explicado
- Ler entrada
- Processar lógica
- Atualizar saídas

4. Título: Ordem de execução
- Menu
- Irrigação
- Agenda
- Display

5. Título: Execução por minuto
- Scheduler avalia quando minuto muda
- Evita disparo repetido

6. Título: Sem travar o sistema
- Sem delays longos no loop
- Leitura de botões continua responsiva

7. Título: Diagnóstico por log
- Monitor serial ajuda muito

Atividade prática (20 min):
- Desenhar no caderno a ordem do loop em 8 passos.

## Aula 8 - Agendamento semanal

Objetivo:
- Dominar o modelo de agenda global com dias e setores por máscara de bits.

Slides (8):
1. Título: Modelo de agenda do projeto
- Até 4 agendas globais
- Cada agenda controla 1..8 setores

2. Título: Campos de uma agenda
- hora, minuto, duracaoMin
- diasMask, setoresMask

3. Título: Dias da semana em bits
- Cada bit representa um dia
- Combinações formam agenda semanal

4. Título: Setores em bits
- Setor 1..8 em máscara
- Fácil de salvar e ler

5. Título: Regras de validação
- Mínimo 1 dia
- Mínimo 1 setor
- Sem duplicata exata

6. Título: Execução por lotes
- Limite de simultâneos
- Intervalo entre lotes

7. Título: Retomada em janela ativa
- Se cair no meio da janela, calcula remanescente

8. Título: Evitar repetição no mesmo dia
- Slot não dispara duas vezes após execução confirmada

Atividade prática (25 min):
- Converter uma agenda em texto para diasMask e setoresMask.

## Aula 9 - Persistência na NVS e integridade de dados

Objetivo:
- Entender como salvar configurações e agendas após reboot.

Slides (6):
1. Título: O que é NVS
- Memória não volátil do ESP32
- Dados sobrevivem ao desligar

2. Título: O que o projeto salva
- Banco de agendas
- Config runtime

3. Título: Versão e CRC
- Detecta dados corrompidos
- Garante leitura segura

4. Título: Política de boot
- Se inválido: reset para padrão seguro
- Depois salva novamente

5. Título: Persistir só quando mudar
- Menos desgaste de escrita
- Melhor confiabilidade

6. Título: Benefício para usuário
- Não perde agenda ao reiniciar

Atividade prática (20 min):
- Simular cenário: dado inválido no boot, qual caminho o sistema segue?

## Aula 10 - Configurações do sistema e manutenção local

Objetivo:
- Dominar as configurações do sistema feitas no menu local.

Slides (6):
1. Título: Menu Configurações
- Ajuste de relógio
- Timeout manual e duração padrão
- Teste de válvulas

2. Título: Ajuste de data e hora
- Hora e minuto
- Dia, mês e ano

3. Título: Timeout manual
- Fecha válvula esquecida
- Valor em minutos

4. Título: Duração padrão
- Usada ao criar agenda
- Pode ser ajustada

5. Título: Teste de válvulas
- Liga/desliga cada setor
- Uso rápido em bancada

6. Título: Reset seguro
- Limpar agendas
- Restaurar padrão

Atividade prática (25 min):
- Ajustar o timeout manual para 5 min e testar um setor.

## Aula 11 - Integração completa e depuração

Objetivo:
- Juntar tudo e aprender a diagnosticar problemas reais.

Slides (7):
1. Título: Fluxo completo ponta a ponta
- Usuário local
- Regras
- Relé

2. Título: Cenário de teste real
- Criar agenda
- Aguardar horário
- Verificar acionamento

3. Título: Onde olhar quando falha
- Cabos e energia
- Configuração
- Logs

4. Título: Problemas comuns
- RTC sem hora
- Agenda inválida
- Relé não aciona

5. Título: Estratégia de depuração
- Isolar módulo
- Testar pequeno
- Confirmar resultado

6. Título: Qualidade de software embarcado
- Reprodutibilidade
- Segurança
- Clareza de código

7. Título: Preparação para projeto final
- Definir objetivo
- Planejar demo

Atividade prática (30 min):
- Cada grupo recebe um "bug" e apresenta como resolveu.

## Aula 12 - Projeto final e apresentação

Objetivo:
- Consolidar aprendizado com mini projeto e demonstração.

Slides (6):
1. Título: Missão da aula final
- Entregar demo funcional
- Explicar como funciona

2. Título: Sugestões de mini projeto
- Nova tela no LCD
- Novo aviso no LCD
- Novo teste de segurança

3. Título: Rubrica de avaliação
- Técnica (40%)
- Organização (30%)
- Comunicação (30%)

4. Título: Como apresentar bem
- Problema
- Solução
- Demo ao vivo

5. Título: Reflexão final
- O que aprendemos do básico ao avançado

6. Título: Próximo nível
- Sensores reais de umidade
- Controle por app
- Dados em nuvem

Atividade prática (40 min):
- Apresentação em grupos com demo e feedback da turma.

## Avaliação geral do curso (simples e didática)

- Participação em aula: 20%
- Práticas curtas: 30%
- Mini desafios (aulas 8, 10, 11): 20%
- Projeto final: 30%

## Checklist rápido para montar os slides

- Use no máximo 6 slides por aula (7 ou 8 apenas em aulas técnicas).
- Limite a 3 bullets por slide.
- Fonte grande e alto contraste.
- Sempre incluir 1 imagem por slide.
- Fechar cada aula com atividade prática curta.

## Banco de imagens sugerido (uso educacional)

- Fotos próprias da bancada e do hardware do projeto.
- Diagramas feitos no draw.io ou Canva.
- Ícones livres (Flaticon com créditos, Icons8, Noun Project).
- Fotos livres (Unsplash, Pexels, Pixabay), sempre verificando licença.

## Sugestão de expansão (opcional: aulas 13 e 14)

- Aula 13: sensores de umidade do solo e tomada de decisão automática.
- Aula 14: interface web local (fase futura) e alertas inteligentes.