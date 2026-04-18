# Plano de Aula - Irrigacao Inteligente com ESP32 (12 encontros)

Publico: adolescentes de 14 a 17 anos
Duracao sugerida por aula: 80 a 100 minutos
Objetivo geral: sair do basico de Arduino/ESP32 e chegar ao entendimento completo do projeto IrrigacaoInteligente.

## Como usar este plano

- Cada aula abaixo ja vem com roteiro de slides.
- Cada slide foi pensado para ter pouco texto.
- Use no maximo 3 bullets por slide.
- Priorize demonstracao real e pratica curta em toda aula.

## Padrao visual sugerido para todos os slides

- 1 titulo curto por slide.
- 1 ideia principal por slide.
- 1 imagem forte ocupando boa parte da tela.
- Texto grande (facil leitura no fundo da sala).
- Evite paragrafos longos.

## Aula 1 - O que e Arduino e por que ESP32?

Objetivo:
- Entender o que e microcontrolador e para que serve no mundo real.

Slides (6):
1. Titulo: Bem-vindos ao projeto de irrigacao inteligente
- Problema real: regar no horario certo
- Solucao: automacao com ESP32
Imagem sugerida: foto de horta + sistema simples de irrigacao.

2. Titulo: O que e um microcontrolador?
- Um mini computador para tarefas especificas
- Le sensores e controla saidas
Imagem sugerida: diagrama simples entrada -> processamento -> saida.

3. Titulo: Arduino x ESP32
- Arduino UNO: bom para comecar
- ESP32: mais poder e Wi-Fi
Imagem sugerida: ESP32 e Arduino lado a lado com legenda.

4. Titulo: O que nosso projeto faz
- Controle manual de 8 valvulas
- Agenda semanal automatica
- Dashboard web local
Imagem sugerida: print da tela OLED + print do dashboard.

5. Titulo: Visao geral da arquitetura
- Hardware + firmware + interface web
- Tudo conectado em um loop principal
Imagem sugerida: diagrama de blocos simplificado do projeto.

6. Titulo: Combinados do curso
- Aprender fazendo
- Errar faz parte
- Seguranca sempre
Imagem sugerida: turma em laboratorio maker.

Atividade pratica (15 min):
- Mostrar a placa ESP32 real e identificar pinos de energia, GPIO e USB.

## Aula 2 - Eletricidade basica e seguranca

Objetivo:
- Compreender tensao, corrente e cuidados ao ligar rele e valvulas.

Slides (6):
1. Titulo: Conceitos basicos
- Tensao (V), corrente (A), potencia (W)
- Formula chave: P = V x I
Imagem sugerida: infografico simples com unidades.

2. Titulo: Sinal logico e mundo fisico
- ESP32 trabalha com sinal digital
- Rele conecta carga maior
Imagem sugerida: diagrama GPIO controlando rele.

3. Titulo: O que e um rele
- Interruptor controlado eletronicamente
- Isolamento entre controle e carga
Imagem sugerida: foto de modulo rele 4 canais.

4. Titulo: Cuidados importantes
- Fonte correta
- GND comum quando necessario
- Nao tocar em ligacao energizada
Imagem sugerida: checklist visual de seguranca.

5. Titulo: Erros comuns em bancada
- Cabo invertido
- Falta de alimentacao
- Mau contato no protoboard
Imagem sugerida: foto de protoboard com destaques.

6. Titulo: Mini roteiro de teste seguro
- Conferir ligacoes
- Energizar
- Testar um canal por vez
Imagem sugerida: fluxograma de 3 passos.

Atividade pratica (20 min):
- Simular no quadro o caminho da energia ate uma valvula.

## Aula 3 - Componentes do projeto na pratica

Objetivo:
- Entender papel de OLED, encoder, RTC DS3231 e reles.

Slides (6):
1. Titulo: Componentes principais
- ESP32
- Encoder com botao
- OLED, RTC e reles
Imagem sugerida: foto com todos componentes identificados.

2. Titulo: OLED SSD1306
- Mostra menu e estados
- Usa barramento I2C
Imagem sugerida: esquema I2C com SDA/SCL.

3. Titulo: RTC DS3231
- Mantem hora precisa
- Base para agendamento
Imagem sugerida: RTC com bateria destacada.

4. Titulo: Encoder rotativo
- Giro navega
- Clique seleciona
- Clique longo volta
Imagem sugerida: sequencia de uso do encoder.

5. Titulo: Rele e valvula
- Rele liga/desliga setor
- Projeto controla ate 8 setores
Imagem sugerida: 8 canais numerados 1..8.

6. Titulo: Mapa de pinos
- Tudo centralizado em Config.h
- Facil de manter e evoluir
Imagem sugerida: tabela de pinos em estilo visual.

Atividade pratica (20 min):
- Alunos montam um mapa de conexoes em papel A4.

## Aula 4 - Primeiros passos no codigo

Objetivo:
- Ler o projeto sem medo e entender modularizacao.

Slides (7):
1. Titulo: Estrutura de pastas e arquivos
- Arquivo principal .ino
- Modulos .h e .cpp
Imagem sugerida: print da arvore de arquivos.

2. Titulo: O papel do arquivo .ino
- Setup inicializa tudo
- Loop roda para sempre
Imagem sugerida: ciclo setup -> loop.

3. Titulo: Driver vs Manager
- Driver fala com hardware
- Manager decide regra de negocio
Imagem sugerida: comparativo em duas colunas.

4. Titulo: Modulos do projeto
- menu_controller
- schedule_manager
- irrigation_controller
Imagem sugerida: cards com cada modulo.

5. Titulo: Config.h e parametros globais
- Pinos e limites
- Wi-Fi e regras de runtime
Imagem sugerida: trecho curto de Config.h em destaque.

6. Titulo: Leitura de codigo para iniciantes
- Primeiro nomes de funcoes
- Depois fluxo de chamadas
Imagem sugerida: marca-texto em codigo curto.

7. Titulo: Dica de estudo
- Entenda primeiro o fluxo
- Depois detalhe modulo por modulo
Imagem sugerida: escada de aprendizado.

Atividade pratica (20 min):
- Em dupla, cada grupo explica 1 modulo em 1 minuto.

## Aula 5 - Entrada do usuario (encoder + menu)

Objetivo:
- Entender como o usuario controla o sistema localmente.

Slides (6):
1. Titulo: Jornada do usuario
- Gira encoder
- Menu muda
- Acao executa
Imagem sugerida: setas de interacao usuario -> sistema.

2. Titulo: Estados de menu
- STATUS
- IRRIGACAO_MANUAL
- PROGRAMAR / WEBSERVER / CONFIGURACOES
Imagem sugerida: maquina de estados simples.

3. Titulo: Clique curto e clique longo
- Curto: selecionar
- Longo: voltar/cancelar
Imagem sugerida: icones de botao curto e longo.

4. Titulo: UX no OLED
- Texto curto
- Cursor claro
- Feedback imediato
Imagem sugerida: mock de tela com cursor.

5. Titulo: Erros de navegacao comuns
- Pular item sem querer
- Entrar em tela errada
Imagem sugerida: meme visual leve de erro de menu.

6. Titulo: Boas praticas de interface embarcada
- Simples
- Rapida
- Previsivel
Imagem sugerida: semaforo verde com 3 criterios.

Atividade pratica (20 min):
- Simular navegacao de menu sem hardware (jogo de papeis em sala).

## Aula 6 - Controle manual de irrigacao

Objetivo:
- Entender como uma valvula abre, fecha e respeita timeout de seguranca.

Slides (6):
1. Titulo: Modo manual
- Escolher setor
- Ligar/desligar com clique
Imagem sugerida: lista de setores 1..8 em mock OLED.

2. Titulo: Estado da valvula
- ABERTA
- FECHADA
- Origem manual ou agenda
Imagem sugerida: diagrama de estados simples.

3. Titulo: Timeout manual
- Evita valvula esquecida aberta
- Valor configuravel em runtime
Imagem sugerida: relogio regressivo.

4. Titulo: Seguranca ao sair do teste
- Fechamento automatico de valvulas
- Evita consumo indevido
Imagem sugerida: icone de escudo + torneira fechando.

5. Titulo: Conflito manual x automatico
- Sistema busca coerencia por deadline
- Mantem operacao segura
Imagem sugerida: balanca com manual e agenda.

6. Titulo: Resultado esperado
- Controle confiavel e previsivel
Imagem sugerida: checklist de sucesso.

Atividade pratica (25 min):
- Teste guiado: abrir setor, aguardar timeout, verificar fechamento.

## Aula 7 - Tempo real e loop principal

Objetivo:
- Entender o ciclo do firmware e o papel do RTC.

Slides (7):
1. Titulo: Por que o relogio importa?
- Agenda depende de hora correta
- RTC garante referencia estavel
Imagem sugerida: relogio analogico + DS3231.

2. Titulo: RTC DS3231 no projeto
- Le data e hora
- Pode ajustar pela interface
Imagem sugerida: print de ajuste de RTC.

3. Titulo: Loop principal explicado
- Ler entrada
- Processar logica
- Atualizar saidas
Imagem sugerida: ciclo circular com 3 blocos.

4. Titulo: Ordem de execucao
- Menu
- Irrigacao
- Agenda
- Web server
- Display
Imagem sugerida: linha do tempo horizontal.

5. Titulo: Execucao por minuto
- Scheduler avalia quando minuto muda
- Evita disparo repetido
Imagem sugerida: relogio marcando virada de minuto.

6. Titulo: Sem travar o sistema
- Sem delays longos no loop
- Servidor web continua responsivo
Imagem sugerida: comparativo travado x fluido.

7. Titulo: Diagnostico por log
- Monitor serial ajuda muito
Imagem sugerida: print de logs limpos.

Atividade pratica (20 min):
- Desenhar no caderno a ordem do loop em 8 passos.

## Aula 8 - Agendamento semanal

Objetivo:
- Dominar o modelo de agenda global com dias e setores por mascara de bits.

Slides (8):
1. Titulo: Modelo de agenda do projeto
- Ate 4 agendas globais
- Cada agenda controla 1..8 setores
Imagem sugerida: grade com 4 slots.

2. Titulo: Campos de uma agenda
- hora, minuto, duracao
- diasMask, setoresMask
Imagem sugerida: card com campos e exemplo.

3. Titulo: Dias da semana em bits
- Cada bit representa um dia
- Combinacoes formam agenda semanal
Imagem sugerida: tabela DOM..SAB com bits 1/0.

4. Titulo: Setores em bits
- Setor 1..8 em mascara
- Facil salvar e ler
Imagem sugerida: 8 lampadas com bits.

5. Titulo: Regras de validacao
- Minimo 1 dia
- Minimo 1 setor
- Sem duplicata exata
Imagem sugerida: selo "valido" x "invalido".

6. Titulo: Execucao por lotes
- Limite de simultaneos
- Intervalo entre lotes
Imagem sugerida: fila de setores por lote.

7. Titulo: Retomada em janela ativa
- Se cair no meio da janela, calcula remanescente
Imagem sugerida: linha do tempo com trecho restante.

8. Titulo: Evitar repeticao no mesmo dia
- Slot nao dispara duas vezes apos execucao confirmada
Imagem sugerida: calendario com check unico.

Atividade pratica (25 min):
- Exercicio: converter uma agenda de texto para diasMask e setoresMask.

## Aula 9 - Persistencia na NVS e integridade de dados

Objetivo:
- Entender como salvar configuracoes e agendas apos reboot.

Slides (6):
1. Titulo: O que e NVS
- Memoria nao volatil do ESP32
- Dados sobrevivem ao desligar
Imagem sugerida: icone de chip + cadeado.

2. Titulo: O que o projeto salva
- Banco de agendas
- Config runtime
Imagem sugerida: duas caixas de dados.

3. Titulo: Versao e CRC
- Detecta dados corrompidos
- Garante leitura segura
Imagem sugerida: etiqueta "ok" e "erro".

4. Titulo: Politica de boot
- Se invalido: reset para padrao seguro
- Depois salva novamente
Imagem sugerida: fluxograma de inicializacao.

5. Titulo: Persistir so quando mudar
- Menos desgaste de escrita
- Melhor confiabilidade
Imagem sugerida: contador de escritas.

6. Titulo: Beneficio para usuario
- Nao perde agenda ao reiniciar
Imagem sugerida: antes/depois reboot.

Atividade pratica (20 min):
- Simular cenario: dado invalido no boot, qual caminho o sistema segue?

## Aula 10 - Dashboard web e API

Objetivo:
- Entender o controle remoto via AP Wi-Fi e endpoints.

Slides (7):
1. Titulo: AP local do sistema
- ESP32 cria rede Wi-Fi propria
- Celular acessa dashboard
Imagem sugerida: celular conectado ao AP do ESP32.

2. Titulo: O que aparece no dashboard
- Estado das valvulas
- Agendas
- Alertas e historico
Imagem sugerida: print da pagina principal.

3. Titulo: API em linguagem simples
- Rotas GET para consultar
- Rotas POST para acionar
Imagem sugerida: diagrama cliente -> API -> firmware.

4. Titulo: Endpoints principais
- /api/status
- /api/schedules
- /api/events
Imagem sugerida: tabela curta de rotas.

5. Titulo: Comandos de acao
- /api/valve/toggle
- /api/schedule/save
- /api/config/runtime
Imagem sugerida: seta de ida e volta com JSON.

6. Titulo: Tratamento de erro
- Em /api/* erro volta em JSON com ok=false
Imagem sugerida: exemplo visual de resposta de erro.

7. Titulo: Historico de eventos
- Buffer circular de ate 40 registros
Imagem sugerida: timeline de eventos.

Atividade pratica (25 min):
- Fazer chamada simulada de endpoint e interpretar resposta JSON.

## Aula 11 - Integracao completa e depuracao

Objetivo:
- Juntar tudo e aprender a diagnosticar problemas reais.

Slides (7):
1. Titulo: Fluxo completo ponta a ponta
- Usuario local ou web
- Regras
- Rele
Imagem sugerida: diagrama completo simplificado.

2. Titulo: Cenario de teste real
- Criar agenda
- Aguardar horario
- Verificar acionamento
Imagem sugerida: checklist de teste.

3. Titulo: Onde olhar quando falha
- Cabos e energia
- Configuracao
- Logs
Imagem sugerida: lupa sobre placa e serial.

4. Titulo: Problemas comuns
- RTC sem hora
- Agenda invalida
- Rede nao conecta
Imagem sugerida: 3 cards de erro.

5. Titulo: Estrategia de depuracao
- Isolar modulo
- Testar pequeno
- Confirmar resultado
Imagem sugerida: funil de diagnostico.

6. Titulo: Qualidade de software embarcado
- Reproducibilidade
- Seguranca
- Clareza de codigo
Imagem sugerida: triangulo de qualidade.

7. Titulo: Preparacao para projeto final
- Definir objetivo
- Planejar demo
Imagem sugerida: quadro kanban simples.

Atividade pratica (30 min):
- Cada grupo recebe um "bug" e apresenta como resolveu.

## Aula 12 - Projeto final e apresentacao

Objetivo:
- Consolidar aprendizado com mini projeto e demonstracao.

Slides (6):
1. Titulo: Missao da aula final
- Entregar demo funcional
- Explicar como funciona
Imagem sugerida: palco de apresentacao escolar.

2. Titulo: Sugestoes de mini projeto
- Nova tela no OLED
- Novo alerta no dashboard
- Novo teste de seguranca
Imagem sugerida: 3 opcoes em cards.

3. Titulo: Rubrica de avaliacao
- Tecnica (40%)
- Organizacao (30%)
- Comunicacao (30%)
Imagem sugerida: barra de avaliacao.

4. Titulo: Como apresentar bem
- Problema
- Solucao
- Demo ao vivo
Imagem sugerida: roteiro em 3 blocos.

5. Titulo: Reflexao final
- O que aprendemos do basico ao avancado
Imagem sugerida: escada de evolucao.

6. Titulo: Proximo nivel
- Sensores reais de umidade
- Controle por app
- Dados em nuvem
Imagem sugerida: roadmap visual.

Atividade pratica (40 min):
- Apresentacao em grupos com demo e feedback da turma.

## Avaliacao geral do curso (simples e didatica)

- Participacao em aula: 20%
- Praticas curtas: 30%
- Mini desafios (aulas 8, 10, 11): 20%
- Projeto final: 30%

## Checklist rapido para montar os slides

- Use no maximo 6 slides por aula (7 ou 8 apenas em aulas tecnicas).
- Limite a 3 bullets por slide.
- Fonte grande e alto contraste.
- Sempre incluir 1 imagem por slide.
- Fechar cada aula com atividade pratica curta.

## Banco de imagens sugerido (uso educacional)

- Fotos proprias da bancada e do hardware do projeto.
- Diagramas feitos no draw.io ou Canva.
- Icones livres (Flaticon com creditos, Icons8, Noun Project).
- Fotos livres (Unsplash, Pexels, Pixabay) verificando licenca.

## Sugestao de expansao (opcional: aulas 13 e 14)

Aula 13 (opcional): sensores de umidade do solo e tomada de decisao automatica.
Aula 14 (opcional): melhoria de UX no dashboard e alertas inteligentes.
