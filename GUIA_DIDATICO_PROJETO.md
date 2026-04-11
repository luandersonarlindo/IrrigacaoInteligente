# Guia Didatico do Projeto Irrigacao Inteligente

Este documento explica o projeto de forma simples, para ensino de alunos de 15 a 16 anos.

## 1) O que este projeto faz

Este projeto e um sistema base de irrigacao com ESP32.
Ele tem:

- Menu no display OLED
- Controle por encoder rotativo (girar + apertar)
- Relogio de tempo real (RTC DS3231)
- Estrutura para controlar valvulas de irrigacao (fase atual: simulada, sem rele acionando de verdade)

## 2) Ideia de arquitetura (quem faz o que)

Cada arquivo tem uma responsabilidade separada. Isso deixa o codigo mais organizado e facil de manter.

- Driver = conversa com o hardware
- Controller/Manager = decide a logica e o comportamento
- Arquivo .ino = ponto de entrada, so liga as pecas

## 3) Fluxo principal do sistema

No loop principal, a ordem e:

1. Ler encoder
2. Descobrir se girou ou apertou
3. Atualizar estado do menu
4. Atualizar logica de irrigacao
5. Atualizar o que aparece no display

Em outras palavras: entrada do usuario -> decisao da logica -> saida na tela.

---

## 4) Explicacao arquivo por arquivo

## Config.h

### Para que serve
Centraliza configuracoes do projeto em um unico lugar.

### O que existe nele
- Definicao de pinos do encoder
- Definicao de pinos do OLED
- Definicao dos pinos dos reles (futuro)
- Tamanho do display
- Parametros de menu e encoder
- Parametros de irrigacao
- Configuracao de debug serial

### Ideia para aula
Se um aluno precisar trocar um pino, ele altera so aqui. Isso evita cacar numeros pelo projeto todo.

---

## display_driver_oled.h / display_driver_oled.cpp

### Para que serve
E o driver do display OLED. Ele sabe desenhar, mas nao decide o conteudo.

### Metodos da classe DisplayDriverOled

- DisplayDriverOled()
  - Construtor da classe.
  - Cria o objeto interno do U8g2 com configuracao do display.

- begin()
  - Inicializa o display e configura fonte, cor e modo de desenho.

- limpar()
  - Limpa o buffer interno (a tela ainda nao muda fisicamente neste ponto).

- renderizar()
  - Envia o buffer para o display (mostra de fato na tela).

- desenharTexto(int x, int y, const char* texto)
  - Desenha texto pequeno na posicao x,y.

- desenharTextoGrande(int x, int y, const char* texto)
  - Desenha texto com fonte maior (para destaque) e depois restaura a fonte padrao.

- desenharLinha(int x1, int y1, int x2, int y2)
  - Desenha uma linha entre dois pontos.

- desenharRetangulo(int x, int y, int largura, int altura)
  - Desenha um retangulo so com contorno.

- desenharRetanguloPreenchido(int x, int y, int largura, int altura)
  - Desenha um retangulo preenchido.

- setCorDesenho(uint8_t cor)
  - Define a cor de desenho (1 branco, 0 preto).

- alturaFonte()
  - Retorna a altura da fonte atual.

### Ideia para aula
Esse arquivo e igual ao motorista de um onibus: ele dirige (desenha), mas nao escolhe o destino (conteudo da tela).

---

## display_manager.h / display_manager.cpp

### Para que serve
Decide o que aparece no display com base no estado do menu e na hora do RTC.

### Metodos da classe DisplayManager

- DisplayManager(DisplayDriverOled& display, MenuController& menu, RtcDriverDs3231& rtc)
  - Construtor com injecao de dependencias.
  - Recebe referencias para driver de display, menu e rtc.

- begin()
  - Inicializacao logica do modulo (neste caso, principalmente debug).

- atualizar()
  - Metodo principal de desenho.
  - Controla taxa de atualizacao (a cada 200 ms).
  - Escolhe qual tela desenhar conforme o estado do menu.

- desenharMenuPrincipal()
  - Mostra os itens do menu.
  - Destaca o item selecionado com fundo preenchido.

- desenharTelaStatus()
  - Mostra tela principal de status com hora grande, data e dica de uso.

- desenharTelaIrrigacao()
  - Tela da opcao Irrigar Agora (ainda em desenvolvimento).

- desenharTelaProgramar()
  - Tela da opcao Programar (ainda em desenvolvimento).

- desenharTelaConfig()
  - Tela da opcao Configuracoes (ainda em desenvolvimento).

- desenharCabecalho(const char* titulo)
  - Desenha titulo no topo e uma linha separadora.

- desenharRodapeHora()
  - Desenha linha de rodape e hora no canto direito.
  - Metodo pronto para reaproveitamento.

- desenharCursor(int y)
  - Declarado no header para uso futuro.
  - Nao esta implementado no .cpp neste momento.

### Ideia para aula
Aqui temos o diretor da tela: ele decide qual cena vai aparecer, enquanto o driver so executa o desenho.

---

## encoder_driver.h / encoder_driver.cpp

### Para que serve
Ler giro e clique do encoder rotativo.

### Enum DirecaoEncoder
- NENHUMA
- HORARIO
- ANTI_HORARIO

### Metodos da classe EncoderDriver

- EncoderDriver()
  - Construtor.
  - Inicia variaveis internas (posicao, estado do botao, debounce).

- begin()
  - Configura o hardware do encoder e do botao.
  - Ativa pull-up interno.

- atualizar()
  - Le o botao e aplica debounce para evitar leituras falsas.

- lerDirecao()
  - Compara contagem atual e anterior do encoder.
  - Retorna a direcao de giro.

- botaoPressionado()
  - Retorna true uma unica vez quando houve clique valido.
  - Depois consome o evento.

### Ideia para aula
Debounce e como ignorar tremidas eletricas do botao para nao contar varios cliques quando foi so um.

---

## menu_controller.h / menu_controller.cpp

### Para que serve
Gerenciar navegacao e estado do menu com base nos eventos do encoder.

### Enum ItemMenu
- IRRIGACAO_MANUAL
- PROGRAMAR
- CONFIGURACOES
- TOTAL (sentinela para contar itens)

### Enum EstadoMenu
- STATUS
- IRRIGACAO_MANUAL
- PROGRAMAR
- CONFIGURACOES

### Metodos da classe MenuController

- MenuController()
  - Construtor.
  - Comeca em STATUS, item 0 e menu inativo.

- begin()
  - Reinicia o estado inicial do menu.

- processar(DirecaoEncoder direcao, bool botaoPressionado)
  - Core da navegacao.
  - Em STATUS: giro/pressionamento ativa menu e pode navegar/selecionar.
  - Em submenu: botao volta para STATUS.

- estadoAtual() const
  - Informa qual estado/tela esta ativa.

- itemSelecionado() const
  - Informa qual item esta selecionado no menu principal.

- menuAtivo() const
  - Diz se o menu esta aberto ou se esta na tela pura de status.

- nomeItem(int indice) const
  - Retorna o nome textual do item para mostrar no display.

- totalItens() const
  - Retorna quantidade de itens do menu principal.

- navegarProximo()
  - Vai para o proximo item (com retorno ao inicio quando chega no fim).

- navegarAnterior()
  - Vai para o item anterior (com retorno circular).

- selecionar()
  - Troca o estado atual para a tela correspondente ao item escolhido.

- voltar()
  - Volta ao STATUS e fecha menu.

### Ideia para aula
Esse modulo e uma maquina de estados simples: cada entrada (giro/clique) muda o estado atual.

---

## rtc_driver_ds3231.h / rtc_driver_ds3231.cpp

### Para que serve
Controlar leitura e ajuste de data/hora no modulo RTC DS3231.

### Metodos da classe RtcDriverDs3231

- RtcDriverDs3231()
  - Construtor da classe.

- begin()
  - Inicializa o RTC.
  - Se o RTC perdeu energia, ajusta para hora de compilacao como fallback.

- agora()
  - Retorna data/hora atual (DateTime).

- ajustarHora(const DateTime& novaHora)
  - Ajusta data/hora do RTC manualmente.

- perdeuEnergia()
  - Informa se o RTC perdeu alimentacao/bateria.

### Ideia para aula
O RTC funciona como um relogio independente, mantendo hora mesmo com ESP32 desligado (se a bateria estiver boa).

---

## irrigation_controller.h / irrigation_controller.cpp

### Para que serve
Controlar logica das valvulas de irrigacao.

### Enum EstadoValvula
- FECHADA
- ABERTA

### Metodos da classe IrrigationController

- IrrigationController()
  - Construtor.
  - Inicializa todas as valvulas como FECHADA.

- begin()
  - Inicializa o modulo e garante todas as valvulas fechadas.

- abrirValvula(int indice)
  - Marca uma valvula como ABERTA.
  - No futuro vai acionar rele real.

- fecharValvula(int indice)
  - Marca uma valvula como FECHADA.
  - No futuro vai desligar rele real.

- fecharTodas()
  - Fecha todas as valvulas por seguranca.

- estadoValvula(int indice) const
  - Retorna o estado atual da valvula informada.

- atualizar()
  - Espaco para logicas futuras de temporizador de irrigacao.

- indiceValido(int indice) const
  - Verifica se o indice da valvula esta no intervalo correto.

### Ideia para aula
Hoje ele faz simulacao de estados. Na fase 2, vai virar controle fisico dos reles.

---

## IrrigacaoInteligente.ino

### Para que serve
E o ponto de entrada do Arduino. Ele conecta todos os modulos.

### Funcoes

- setup()
  - Inicializa serial (debug), I2C, encoder, display, rtc, menu, display manager e irrigacao.
  - Se o RTC nao for encontrado, o sistema continua funcionando sem hora real.

- loop()
  - Executa continuamente o ciclo principal:
    1) atualizar encoder
    2) ler direcao e botao
    3) processar menu
    4) atualizar irrigacao
    5) atualizar display

### Ideia para aula
Boa pratica: manter o .ino limpo, sem regra de negocio. Ele deve orquestrar, nao decidir tudo.

---

## 5) Resumo pedagogico rapido

- O projeto esta bem modularizado.
- Cada modulo tem funcao clara.
- Isso facilita:
  - testar
  - manter
  - evoluir para novas fases (reles reais, programacao de horarios etc.)

## 6) Proximos passos didaticos sugeridos

- Atividade 1: pedir para alunos desenharem o fluxo em um diagrama de blocos.
- Atividade 2: implementar uma opcao nova de menu (exemplo: Informacoes do Sistema).
- Atividade 3: na irrigacao, criar temporizador simples por millis() para fechar valvula apos alguns segundos.
- Atividade 4: criar tela de ajuste de hora usando encoder.
