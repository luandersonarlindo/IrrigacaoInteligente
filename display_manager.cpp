// ============================================================
//  display_manager.cpp — Implementação do DisplayManager
// ============================================================

#include "display_manager.h"

namespace
{
    // Icones simples em linha para OLED monocromatico.
    void desenharIconeMenuPrincipal(DisplayDriverOled &d, int x, int y, int tipo)
    {
        // Moldura base 32x32
        d.desenharRetangulo(x, y, 32, 32);

        if (tipo == 0)
        {
            // Irrigar agora: aspersor com gotas e jatos (estilo tecnico mono)
            d.desenharRetanguloPreenchido(x + 11, y + 4, 10, 4); // topo do bico
            d.desenharRetanguloPreenchido(x + 13, y + 9, 6, 11); // corpo central

            // gotas superiores
            d.desenharRetanguloPreenchido(x + 3, y + 8, 2, 2);
            d.desenharRetanguloPreenchido(x + 8, y + 11, 2, 2);
            d.desenharRetanguloPreenchido(x + 24, y + 11, 2, 2);
            d.desenharRetanguloPreenchido(x + 29, y + 8, 2, 2);

            // jatos inferiores em triangulo
            d.desenharLinha(x + 6, y + 28, x + 9, y + 22);
            d.desenharLinha(x + 9, y + 22, x + 12, y + 28);

            d.desenharLinha(x + 12, y + 28, x + 15, y + 22);
            d.desenharLinha(x + 15, y + 22, x + 18, y + 28);

            d.desenharLinha(x + 18, y + 28, x + 21, y + 22);
            d.desenharLinha(x + 21, y + 22, x + 24, y + 28);

            d.desenharLinha(x + 24, y + 28, x + 27, y + 22);
            d.desenharLinha(x + 27, y + 22, x + 30, y + 28);
        }
        else if (tipo == 1)
        {
            // Programar: calculadora/teclado numerico
            d.desenharRetangulo(x + 5, y + 4, 22, 24);
            d.desenharRetangulo(x + 8, y + 7, 16, 4); // display superior

            // grade de teclas 3x3
            int kx = x + 9;
            int ky = y + 14;
            for (int r = 0; r < 3; r++)
            {
                for (int c = 0; c < 3; c++)
                {
                    d.desenharRetanguloPreenchido(kx + (c * 5), ky + (r * 4), 2, 2);
                }
            }

            // tecla lateral (estilo funcao)
            d.desenharRetanguloPreenchido(x + 24, y + 14, 2, 10);
        }
        else
        {
            // Configuracoes: sliders
            d.desenharLinha(x + 6, y + 10, x + 26, y + 10);
            d.desenharLinha(x + 6, y + 16, x + 26, y + 16);
            d.desenharLinha(x + 6, y + 22, x + 26, y + 22);
            d.desenharRetanguloPreenchido(x + 10, y + 8, 4, 4);
            d.desenharRetanguloPreenchido(x + 18, y + 14, 4, 4);
            d.desenharRetanguloPreenchido(x + 13, y + 20, 4, 4);
        }
    }

    void desenharIconeSubmenu(DisplayDriverOled &d, int x, int y, int opcao)
    {
        // area util: 8x8
        if (opcao == 0)
        {
            // Hora: relogio
            d.desenharRetangulo(x, y, 8, 8);
            d.desenharLinha(x + 4, y + 4, x + 4, y + 2);
            d.desenharLinha(x + 4, y + 4, x + 6, y + 5);
        }
        else if (opcao == 1)
        {
            // Minuto: cronometro
            d.desenharRetangulo(x + 1, y + 1, 6, 6);
            d.desenharRetanguloPreenchido(x + 3, y, 2, 1);
            d.desenharLinha(x + 4, y + 4, x + 5, y + 3);
        }
        else if (opcao == 2)
        {
            // Duracao: barra
            d.desenharRetangulo(x, y + 2, 8, 4);
            d.desenharRetanguloPreenchido(x + 1, y + 3, 5, 2);
        }
        else if (opcao == 3)
        {
            // Dias: calendario mini
            d.desenharRetangulo(x, y + 1, 8, 7);
            d.desenharLinha(x, y + 3, x + 7, y + 3);
            d.desenharRetanguloPreenchido(x + 2, y, 1, 2);
            d.desenharRetanguloPreenchido(x + 5, y, 1, 2);
        }
        else if (opcao == 4)
        {
            // Setores: grade 2x2
            d.desenharRetangulo(x, y, 3, 3);
            d.desenharRetangulo(x + 5, y, 3, 3);
            d.desenharRetangulo(x, y + 5, 3, 3);
            d.desenharRetangulo(x + 5, y + 5, 3, 3);
        }
        else if (opcao == 5)
        {
            // Salvar: disquete simplificado
            d.desenharRetangulo(x, y, 8, 8);
            d.desenharRetanguloPreenchido(x + 1, y + 1, 5, 2);
            d.desenharRetangulo(x + 2, y + 5, 4, 2);
        }
        else
        {
            // Excluir: lixeira simplificada
            d.desenharRetangulo(x + 1, y + 2, 6, 6);
            d.desenharRetanguloPreenchido(x, y + 1, 8, 1);
            d.desenharRetanguloPreenchido(x + 3, y, 2, 1);
        }
    }

    void desenharIconeAgendaSlot(DisplayDriverOled &d, int x, int y, bool ativa)
    {
        // Moldura do slot
        d.desenharRetangulo(x, y, 16, 16);
        d.desenharRetangulo(x + 2, y + 2, 12, 4);

        if (ativa)
        {
            // Relogio pequeno (agenda ativa)
            d.desenharRetangulo(x + 4, y + 8, 8, 6);
            d.desenharLinha(x + 8, y + 11, x + 8, y + 9);
            d.desenharLinha(x + 8, y + 11, x + 10, y + 12);
        }
        else
        {
            // Sinal de mais (slot vazio)
            d.desenharLinha(x + 8, y + 8, x + 8, y + 13);
            d.desenharLinha(x + 6, y + 11, x + 10, y + 11);
        }
    }

    void desenharAnimacaoCheck(DisplayDriverOled &d, int x, int y)
    {
        // Animacao simples baseada no tempo (4 quadros em loop)
        const unsigned long quadroMs = 90;
        int quadro = (millis() / quadroMs) % 4;

        // Caixa do check
        d.desenharRetangulo(x, y, 10, 10);

        // Quadro 1: primeiro segmento curto
        if (quadro >= 1)
        {
            d.desenharLinha(x + 2, y + 5, x + 4, y + 7);
        }

        // Quadro 2: alonga primeiro segmento
        if (quadro >= 2)
        {
            d.desenharLinha(x + 3, y + 6, x + 5, y + 8);
        }

        // Quadro 3+: completa check
        if (quadro >= 3)
        {
            d.desenharLinha(x + 4, y + 7, x + 8, y + 3);
            d.desenharLinha(x + 5, y + 8, x + 9, y + 4);
        }
    }

    void desenharSegmentoParcial(DisplayDriverOled &d, int x1, int y1, int x2, int y2, int passo, int totalPassos)
    {
        if (passo <= 0)
            return;
        if (passo > totalPassos)
            passo = totalPassos;

        int dx = x2 - x1;
        int dy = y2 - y1;
        int xp = x1 + (dx * passo) / totalPassos;
        int yp = y1 + (dy * passo) / totalPassos;
        d.desenharLinha(x1, y1, xp, yp);
    }

    void desenharAnimacaoCheck48(DisplayDriverOled &d, int frame)
    {
        const int x = 40;
        const int y = 8;
        const int w = 48;
        const int h = 48;
        d.desenharRetangulo(x, y, w, h);

        // Coordenadas do check dentro da caixa 48x48
        const int ax = x + 12;
        const int ay = y + 26;
        const int bx = x + 21;
        const int by = y + 35;
        const int cx = x + 37;
        const int cy = y + 18;

        // 16 quadros totais por loop
        // quadros 0..7: primeira perna, 8..15: segunda perna
        if (frame >= 2)
        {
            int p1 = frame - 1;
            desenharSegmentoParcial(d, ax, ay, bx, by, p1, 7);
            desenharSegmentoParcial(d, ax, ay + 1, bx, by + 1, p1, 7);
        }

        if (frame >= 8)
        {
            int p2 = frame - 7;
            desenharSegmentoParcial(d, bx, by, cx, cy, p2, 8);
            desenharSegmentoParcial(d, bx, by + 1, cx, cy + 1, p2, 8);
        }
    }

    void desenharAnimacaoLixeira48(DisplayDriverOled &d, int frame)
    {
        const int x = 40;
        const int y = 8;

        // Corpo da lixeira
        d.desenharRetangulo(x + 12, y + 10, 24, 30);

        // Tampa animada (abre e fecha)
        int fase = frame % 16;
        int off = 0;
        if (fase >= 3 && fase <= 7)
            off = (fase - 2);
        else if (fase >= 8 && fase <= 12)
            off = (13 - fase);

        d.desenharLinha(x + 10, y + 9 - off, x + 38, y + 9 - off);
        d.desenharRetanguloPreenchido(x + 22, y + 7 - off, 4, 2);

        // Linhas internas
        d.desenharLinha(x + 18, y + 14, x + 18, y + 36);
        d.desenharLinha(x + 24, y + 14, x + 24, y + 36);
        d.desenharLinha(x + 30, y + 14, x + 30, y + 36);

        // "Papel" caindo durante a abertura
        if (fase >= 4 && fase <= 11)
        {
            int py = y + 10 + (fase - 4) * 3;
            if (py < y + 38)
                d.desenharRetanguloPreenchido(x + 24, py, 3, 3);
        }
    }
}

DisplayManager::DisplayManager(DisplayDriverOled &display,
                               MenuController &menu,
                               RtcDriverDs3231 &rtc,
                               IrrigationController &irrigacao)
    : _display(display),
      _menu(menu),
      _rtc(rtc),
      _irrigacao(irrigacao),
      _ultimaAtualizacao(0)
{
}

void DisplayManager::begin()
{
    if (DEBUG_SERIAL)
    {
        Serial.println("[DisplayManager] Inicializado.");
    }
}

void DisplayManager::atualizar()
{
    unsigned long agora = millis();
    unsigned long intervalo = INTERVALO_ATUALIZACAO_MS;
    if (_menu.menuAtivo())
        intervalo = 80;

    if (agora - _ultimaAtualizacao < intervalo)
        return;
    _ultimaAtualizacao = agora;

    _display.limpar();

    switch (_menu.estadoAtual())
    {
    case EstadoMenu::STATUS:
        // Status puro = tela de relógio
        // Menu ativo = usuário começou a navegar
        if (_menu.menuAtivo())
            desenharMenuPrincipal();
        else
            desenharTelaStatus();
        break;
    case EstadoMenu::IRRIGACAO_MANUAL:
        desenharTelaIrrigacao();
        break;
    case EstadoMenu::PROGRAMAR:
        desenharTelaProgramar();
        break;
    case EstadoMenu::CONFIGURACOES:
        desenharTelaConfig();
        break;
    }

    _display.renderizar();
}

// ============================================================
//  Telas
// ============================================================

void DisplayManager::desenharMenuPrincipal()
{
    const int iconW = 32;
    const int iconGap = 6;
    const int iconY = 8;
    const int centerX = (OLED_LARGURA - iconW) / 2;

    int totalItens = _menu.totalItens();
    int itemAtual = _menu.itemSelecionado();

    int x = centerX - (itemAtual * (iconW + iconGap));
    for (int i = 0; i < totalItens; i++)
    {
        if (x >= 0 && (x + iconW) <= OLED_LARGURA)
        {
            desenharIconeMenuPrincipal(_display, x, iconY, i);
        }
        x += iconW + iconGap;
    }

    // Moldura tripla no item selecionado para reforcar foco
    _display.desenharRetangulo(centerX - 1, iconY - 1, iconW + 2, iconW + 2);
    _display.desenharRetangulo(centerX - 2, iconY - 2, iconW + 4, iconW + 4);
    _display.desenharRetangulo(centerX - 3, iconY - 3, iconW + 6, iconW + 6);

    // Rotulo centralizado no rodape
    const char *rotulo = _menu.nomeItem(itemAtual);
    int larguraRotulo = _display.larguraTexto(rotulo);
    int xRotulo = (OLED_LARGURA - larguraRotulo) / 2;
    if (xRotulo < 0)
        xRotulo = 0;

    _display.desenharTexto(xRotulo, 54, rotulo);
}

void DisplayManager::desenharTelaStatus()
{
    desenharCabecalho("STATUS");

    // Hora em destaque no centro
    DateTime agora = _rtc.agora();
    char hora[9];
    snprintf(hora, sizeof(hora), "%02d:%02d:%02d",
             agora.hour(), agora.minute(), agora.second());

    _display.desenharTextoGrande(20, 22, hora);

    // Data abaixo
    char data[11];
    snprintf(data, sizeof(data), "%02d/%02d/%04d",
             agora.day(), agora.month(), agora.year());
    _display.desenharTexto(28, 46, data);

    _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
    _display.desenharTexto(0, 56, "Gire para abrir menu");
}

void DisplayManager::desenharTelaIrrigacao()
{
    desenharCabecalho("IRRIGAR AGORA");

    int setorAtual = _menu.setorAtual();
    int totalItens = NUM_VALVULAS + 1; // setores + item Voltar

    const int linhasVisiveis = 3;
    int inicio = (setorAtual / linhasVisiveis) * linhasVisiveis;
    int fim = inicio + linhasVisiveis;
    if (fim > totalItens)
        fim = totalItens;

    // Exibe uma janela paginada com setores e item Voltar
    for (int i = inicio; i < fim; i++)
    {
        int linhaIdx = i - inicio;
        int y = 16 + (linhaIdx * 11);

        bool itemVoltar = (i == NUM_VALVULAS);
        bool aberta = false;
        if (!itemVoltar)
        {
            aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
        }
        bool selecionado = (i == setorAtual);

        if (selecionado)
        {
            _display.setCorDesenho(1);
            _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 11);
            _display.setCorDesenho(0);
        }

        char linha[22];
        if (itemVoltar)
        {
            snprintf(linha, sizeof(linha), "< Voltar");
        }
        else
        {
            snprintf(linha, sizeof(linha), "Setor %d  %s",
                     i + 1,
                     aberta ? "[LIGADO ]" : "[------]");
        }

        _display.desenharTexto(2, y, linha);

        if (selecionado)
            _display.setCorDesenho(1);
    }

    // Aviso de timeout se ocorreu
    if (_menu.timeoutOcorreu())
    {
        _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
        _display.desenharTexto(0, 56, "! Timeout: setor fechado");
    }
    else
    {
        _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
        char rodape[28];
        snprintf(rodape, sizeof(rodape), "OK=toggle Pg %d/%d", (inicio / linhasVisiveis) + 1,
                 (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        if (_menu.opcaoVoltarIrrigacaoSelecionada())
        {
            snprintf(rodape, sizeof(rodape), "OK=voltar Pg %d/%d", (inicio / linhasVisiveis) + 1,
                     (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        }
        _display.desenharTextoMini(0, 56, rodape);
    }
}

void DisplayManager::desenharTelaProgramar()
{
    EtapaProgramacao etapa = _menu.etapaProgramacao();
    int agenda = _menu.agendaProgramacao() + 1;

    auto desenharBarra = [&](int x, int y, int largura, int altura, int valor, int maximo)
    {
        if (maximo <= 0)
            return;
        _display.desenharRetangulo(x, y, largura, altura);
        int interno = largura - 2;
        int preenchido = (interno * valor) / maximo;
        if (preenchido < 0)
            preenchido = 0;
        if (preenchido > interno)
            preenchido = interno;
        _display.desenharRetanguloPreenchido(x + 1, y + 1, preenchido, altura - 2);
    };

    if (etapa == EtapaProgramacao::SELECIONAR_AGENDA)
    {
        static bool animFeedbackAtivo = false;
        static unsigned long animFeedbackInicioMs = 0;
        static FeedbackProgramacao animTipo = FeedbackProgramacao::NENHUM;

        FeedbackProgramacao fb = _menu.feedbackProgramacao();
        if (fb == FeedbackProgramacao::SALVO || fb == FeedbackProgramacao::EXCLUIDO)
        {
            const unsigned long FRAME_DELAY_MS = 42;
            const int FRAME_COUNT = 16;
            const int LOOP_COUNT = 3;

            if (!animFeedbackAtivo || animTipo != fb)
            {
                animFeedbackAtivo = true;
                animFeedbackInicioMs = millis();
                animTipo = fb;
            }

            unsigned long elapsed = millis() - animFeedbackInicioMs;
            unsigned long frameGlobal = elapsed / FRAME_DELAY_MS;
            unsigned long totalFrames = (unsigned long)FRAME_COUNT * LOOP_COUNT;

            if (frameGlobal >= totalFrames)
            {
                animFeedbackAtivo = false;
                animTipo = FeedbackProgramacao::NENHUM;
                _menu.limparFeedbackProgramacao();
            }
            else
            {
                int frameLocal = (int)(frameGlobal % FRAME_COUNT);
                if (fb == FeedbackProgramacao::SALVO)
                    desenharAnimacaoCheck48(_display, frameLocal);
                else
                    desenharAnimacaoLixeira48(_display, frameLocal);
                return;
            }
        }
        else
        {
            animFeedbackAtivo = false;
            animTipo = FeedbackProgramacao::NENHUM;
        }

        desenharCabecalho("PROGRAMAR");

        char linhaAgenda[24];
        snprintf(linhaAgenda, sizeof(linhaAgenda), "Agenda %d de %d", agenda, MAX_AGENDAS_TOTAIS);
        _display.desenharTexto(0, 16, linhaAgenda);

        AgendaSetor ag = _menu.agendaSelecionada();
        desenharIconeAgendaSlot(_display, 0, 27, ag.ativa);

        if (ag.ativa)
        {
            char linhaHorario[24];
            snprintf(linhaHorario, sizeof(linhaHorario), "%02dh:%02d  %umin", ag.hora, ag.minuto, ag.duracaoMin);

            int totalDias = 0;
            int totalSetores = 0;
            for (int i = 0; i < 7; i++)
                if (ag.diasMask & (1 << i))
                    totalDias++;
            for (int i = 0; i < NUM_VALVULAS; i++)
                if (ag.setoresMask & (1 << i))
                    totalSetores++;

            _display.desenharTexto(19, 28, linhaHorario);

            char linhaResumo[24];
            snprintf(linhaResumo, sizeof(linhaResumo), "Dias:%d  Set:%d", totalDias, totalSetores);
            _display.desenharTexto(19, 38, linhaResumo);
            // Sem texto de acao: o proprio icone indica que a agenda e editavel.
        }
        else
        {
            _display.desenharTexto(19, 30, "Slot vazio");
            _display.desenharTextoMini(19, 48, "OK para criar agenda");
        }

        if (fb == FeedbackProgramacao::ERRO_SEM_DIA)
            _display.desenharTexto(0, 48, "Marque ao menos 1 dia");
        else if (fb == FeedbackProgramacao::ERRO_SEM_SETOR)
            _display.desenharTexto(0, 48, "Marque ao menos 1 setor");
        else if (fb == FeedbackProgramacao::ERRO_DUPLICADA)
            _display.desenharTexto(0, 48, "Agenda duplicada");
        else if (fb == FeedbackProgramacao::ERRO_PERSISTENCIA)
            _display.desenharTexto(0, 48, "Erro ao salvar");

        _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
        _display.desenharTextoMini(0, 56, "Segure: sair");
        return;
    }

    AgendaSetor ag = _menu.agendaEdicao();
    if (etapa == EtapaProgramacao::SUBMENU_AGENDA)
    {
        desenharCabecalho("EDITAR AGENDA");

        const char *opcoes[8] = {
            "Hora",
            "Minuto",
            "Duracao",
            "Dias",
            "Setores",
            "Salvar",
            "Excluir",
            "Voltar"};

        int selecionada = _menu.opcaoSubmenuProgramacao();
        int inicio = 0;
        if (selecionada > 2)
            inicio = selecionada - 2;
        if (inicio > 4)
            inicio = 4;

        for (int i = 0; i < 4; i++)
        {
            int idx = inicio + i;
            int y = 16 + (i * 10);
            if (idx >= 8)
                break;

            if (idx == selecionada)
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 9);
                _display.setCorDesenho(0);
            }

            char linha[28];
            if (idx == 0)
                snprintf(linha, sizeof(linha), "Hora: %02dh", ag.hora);
            else if (idx == 1)
                snprintf(linha, sizeof(linha), "Minuto: %02dmin", ag.minuto);
            else if (idx == 2)
                snprintf(linha, sizeof(linha), "Duracao: %umin", ag.duracaoMin);
            else
                snprintf(linha, sizeof(linha), "%s", opcoes[idx]);

            if (idx <= 6)
            {
                desenharIconeSubmenu(_display, 2, y, idx);
            }
            else
            {
                _display.desenharLinha(3, y + 4, 10, y + 4);
                _display.desenharLinha(3, y + 4, 6, y + 1);
                _display.desenharLinha(3, y + 4, 6, y + 7);
            }
            _display.desenharTexto(14, y, linha);

            if (idx == selecionada)
                _display.setCorDesenho(1);
        }

        _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
        _display.desenharTextoMini(0, 56, "OK: entrar  Segure: voltar");
        return;
    }

    if (etapa == EtapaProgramacao::CONFIRMAR_EXCLUSAO)
    {
        desenharCabecalho("EXCLUIR AGENDA?");
        _display.desenharTexto(0, 20, "Confirma exclusao?");
        _display.desenharTextoMini(0, 30, "Gire: SIM/NAO | OK confirma");

        int opc = _menu.opcaoConfirmarExclusao(); // 0=SIM, 1=NAO

        if (opc == 0)
        {
            _display.desenharRetanguloPreenchido(8, 40, 48, 12);
            _display.setCorDesenho(0);
            _display.desenharTexto(22, 42, "SIM");
            _display.setCorDesenho(1);

            _display.desenharRetangulo(72, 40, 48, 12);
            _display.desenharTexto(88, 42, "NAO");
        }
        else
        {
            _display.desenharRetangulo(8, 40, 48, 12);
            _display.desenharTexto(22, 42, "SIM");

            _display.desenharRetanguloPreenchido(72, 40, 48, 12);
            _display.setCorDesenho(0);
            _display.desenharTexto(88, 42, "NAO");
            _display.setCorDesenho(1);
        }

        _display.desenharTextoMini(0, 56, "Segure: cancelar");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_HORA)
    {
        desenharCabecalho("EDITAR HORA");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02dh", ag.hora);
        _display.desenharTextoGrande(44, 16, valor);
        desenharBarra(8, 38, 112, 10, ag.hora, 23);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta | Segure");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_MINUTO)
    {
        desenharCabecalho("EDITAR MINUTO");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02dmin", ag.minuto);
        _display.desenharTextoGrande(32, 16, valor);
        desenharBarra(8, 38, 112, 10, ag.minuto, 59);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta | Segure");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_DURACAO)
    {
        desenharCabecalho("EDITAR DURACAO");
        char valor[18];
        snprintf(valor, sizeof(valor), "%umin", ag.duracaoMin);
        _display.desenharTextoGrande(30, 16, valor);
        desenharBarra(8, 38, 112, 10, ag.duracaoMin, 240);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta | Segure");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_DIAS)
    {
        desenharCabecalho("EDITAR DIAS");
        const char *diasSigla[7] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};
        int xBase = 2;
        int passo = 18;

        for (int i = 0; i < 7; i++)
        {
            int x = xBase + (i * passo);
            _display.desenharTextoMini(x, 22, diasSigla[i]);

            bool marcado = (ag.diasMask & (1 << i)) != 0;
            if (marcado)
                _display.desenharRetanguloPreenchido(x + 4, 34, 6, 6);
            else
                _display.desenharRetangulo(x + 4, 34, 6, 6);

            if (i == _menu.cursorDiaProgramacao())
            {
                _display.desenharRetangulo(x + 2, 32, 10, 10);
            }
        }
        _display.desenharTextoMini(0, 50, "Gire: cursor | OK: marca");
        _display.desenharTextoMini(0, 56, "Segure: voltar submenu");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_SETORES)
    {
        desenharCabecalho("EDITAR SETORES");

        const int linhasVisiveis = 3;
        int cursor = _menu.cursorSetorProgramacao();
        int totalItens = NUM_VALVULAS + 1; // setores + voltar
        int inicio = (cursor / linhasVisiveis) * linhasVisiveis;
        int fim = inicio + linhasVisiveis;
        if (fim > totalItens)
            fim = totalItens;

        for (int i = inicio; i < fim; i++)
        {
            int linhaIdx = i - inicio;
            int y = 16 + (linhaIdx * 11);
            bool itemVoltar = (i == NUM_VALVULAS);
            bool marcado = false;
            if (!itemVoltar)
            {
                marcado = (ag.setoresMask & (1 << i)) != 0;
            }
            if (i == _menu.cursorSetorProgramacao())
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 10);
                _display.setCorDesenho(0);
            }

            char linha[24];
            if (itemVoltar)
                snprintf(linha, sizeof(linha), "< Voltar");
            else
                snprintf(linha, sizeof(linha), "Setor %d   [%c]", i + 1, marcado ? 'X' : ' ');
            _display.desenharTexto(2, y, linha);

            if (i == _menu.cursorSetorProgramacao())
            {
                _display.setCorDesenho(1);
            }
        }
        char rodape[26];
        if (_menu.cursorSetorProgramacao() == NUM_VALVULAS)
            snprintf(rodape, sizeof(rodape), "OK voltar Pg %d/%d",
                     (inicio / linhasVisiveis) + 1,
                     (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        else
            snprintf(rodape, sizeof(rodape), "Gire/OK marca Pg %d/%d",
                     (inicio / linhasVisiveis) + 1,
                     (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        _display.desenharTextoMini(0, 56, rodape);
    }

    FeedbackProgramacao fb = _menu.feedbackProgramacao();
    if (fb == FeedbackProgramacao::ERRO_SEM_DIA)
        _display.desenharTexto(0, 50, "Marque ao menos 1 dia");
    else if (fb == FeedbackProgramacao::ERRO_SEM_SETOR)
        _display.desenharTexto(0, 50, "Marque ao menos 1 setor");
    else if (fb == FeedbackProgramacao::ERRO_DUPLICADA)
        _display.desenharTexto(0, 50, "Agenda duplicada");
    else if (fb == FeedbackProgramacao::ERRO_PERSISTENCIA)
        _display.desenharTexto(0, 50, "Erro ao salvar");
    else if (fb == FeedbackProgramacao::ERRO_EXCLUSAO)
        _display.desenharTexto(0, 50, "Erro ao excluir");
}

void DisplayManager::desenharTelaConfig()
{
    EtapaConfiguracao etapa = _menu.etapaConfiguracao();
    static bool animSalvoAtiva = false;
    static bool animLimpoAtiva = false;
    static bool animRestauradoAtiva = false;
    static unsigned long animSalvoInicioMs = 0;
    static unsigned long animLimpoInicioMs = 0;
    static unsigned long animRestauradoInicioMs = 0;

    const unsigned long FRAME_DELAY_MS = 42;
    const int FRAME_COUNT = 16;
    const int LOOP_COUNT = 3;

    if (_menu.feedbackConfiguracaoSalvo())
    {
        animSalvoAtiva = true;
        animSalvoInicioMs = millis();
        _menu.limparFeedbackConfiguracaoSalvo();
    }

    if (_menu.feedbackConfiguracaoLimpo())
    {
        animLimpoAtiva = true;
        animLimpoInicioMs = millis();
        _menu.limparFeedbackConfiguracaoLimpo();
    }

    if (_menu.feedbackConfiguracaoRestaurado())
    {
        animRestauradoAtiva = true;
        animRestauradoInicioMs = millis();
        _menu.limparFeedbackConfiguracaoRestaurado();
    }

    if (animSalvoAtiva)
    {
        desenharCabecalho("CONFIGURACOES");
        unsigned long elapsed = millis() - animSalvoInicioMs;
        unsigned long frameGlobal = elapsed / FRAME_DELAY_MS;
        unsigned long totalFrames = (unsigned long)FRAME_COUNT * LOOP_COUNT;
        if (frameGlobal >= totalFrames)
        {
            animSalvoAtiva = false;
            return;
        }

        int frameLocal = (int)(frameGlobal % FRAME_COUNT);
        desenharAnimacaoCheck48(_display, frameLocal);
        _display.desenharTextoMini(34, 56, "Relogio salvo!");
        return;
    }

    if (animLimpoAtiva)
    {
        desenharCabecalho("CONFIGURACOES");
        unsigned long elapsed = millis() - animLimpoInicioMs;
        unsigned long frameGlobal = elapsed / FRAME_DELAY_MS;
        unsigned long totalFrames = (unsigned long)FRAME_COUNT * LOOP_COUNT;
        if (frameGlobal >= totalFrames)
        {
            animLimpoAtiva = false;
            return;
        }

        int frameLocal = (int)(frameGlobal % FRAME_COUNT);
        desenharAnimacaoLixeira48(_display, frameLocal);
        _display.desenharTextoMini(20, 56, "Agendas apagadas!");
        return;
    }

    if (animRestauradoAtiva)
    {
        desenharCabecalho("CONFIGURACOES");
        unsigned long elapsed = millis() - animRestauradoInicioMs;
        unsigned long frameGlobal = elapsed / FRAME_DELAY_MS;
        unsigned long totalFrames = (unsigned long)FRAME_COUNT * LOOP_COUNT;
        if (frameGlobal >= totalFrames)
        {
            animRestauradoAtiva = false;
            return;
        }

        int frameLocal = (int)(frameGlobal % FRAME_COUNT);
        desenharAnimacaoCheck48(_display, frameLocal);
        _display.desenharTextoMini(16, 56, "Padrao restaurado!");
        return;
    }

    if (etapa == EtapaConfiguracao::MENU)
    {
        desenharCabecalho("CONFIGURACOES");

        const char *opcoes[3] = {
            "Relogio",
            "Sistema",
            "Voltar"};

        int selecionada = _menu.opcaoConfiguracao();
        for (int idx = 0; idx < 3; idx++)
        {
            int y = 18 + (idx * 12);
            if (idx == selecionada)
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 10);
                _display.setCorDesenho(0);
            }

            if (idx == 0)
                desenharIconeSubmenu(_display, 2, y, 0);
            else if (idx == 1)
                desenharIconeSubmenu(_display, 2, y, 2);
            else
            {
                _display.desenharLinha(3, y + 4, 10, y + 4);
                _display.desenharLinha(3, y + 4, 6, y + 1);
                _display.desenharLinha(3, y + 4, 6, y + 7);
            }

            _display.desenharTexto(14, y, opcoes[idx]);
            if (idx == selecionada)
                _display.setCorDesenho(1);
        }

        _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
        _display.desenharTextoMini(0, 56, "OK entra | Segure sair");
        return;
    }

    if (etapa == EtapaConfiguracao::SUBMENU_RELOGIO)
    {
        desenharCabecalho("CONFIG > RELOGIO");

        const char *opcoes[8] = {
            "Hora",
            "Minuto",
            "Dia",
            "Mes",
            "Ano",
            "Timeout",
            "Salvar relogio",
            "Voltar"};

        int selecionada = _menu.opcaoConfiguracao();
        int inicio = 0;
        if (selecionada > 2)
            inicio = selecionada - 2;
        if (inicio > 4)
            inicio = 4;

        for (int i = 0; i < 4; i++)
        {
            int idx = inicio + i;
            int y = 16 + (i * 10);
            if (idx >= 8)
                break;

            if (idx == selecionada)
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 9);
                _display.setCorDesenho(0);
            }

            char linha[32];
            if (idx == 0)
                snprintf(linha, sizeof(linha), "Hora: %02d", _menu.configHora());
            else if (idx == 1)
                snprintf(linha, sizeof(linha), "Minuto: %02d", _menu.configMinuto());
            else if (idx == 2)
                snprintf(linha, sizeof(linha), "Dia: %02d", _menu.configDia());
            else if (idx == 3)
                snprintf(linha, sizeof(linha), "Mes: %02d", _menu.configMes());
            else if (idx == 4)
                snprintf(linha, sizeof(linha), "Ano: %04d", _menu.configAno());
            else if (idx == 5)
                snprintf(linha, sizeof(linha), "Timeout: %d min", _menu.configTimeoutManualMin());
            else
                snprintf(linha, sizeof(linha), "%s", opcoes[idx]);

            if (idx <= 6)
                desenharIconeSubmenu(_display, 2, y, idx <= 5 ? idx : 5);
            else
            {
                _display.desenharLinha(3, y + 4, 10, y + 4);
                _display.desenharLinha(3, y + 4, 6, y + 1);
                _display.desenharLinha(3, y + 4, 6, y + 7);
            }

            _display.desenharTexto(14, y, linha);
            if (idx == selecionada)
                _display.setCorDesenho(1);
        }

        _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
        _display.desenharTextoMini(0, 56, "OK entra | Segure menu");
        return;
    }

    if (etapa == EtapaConfiguracao::SUBMENU_SISTEMA)
    {
        desenharCabecalho("CONFIG > SISTEMA");

        const char *opcoes[6] = {
            "Duracao padrao",
            "Teste valvulas",
            "Limpar agendas",
            "Restaurar padrao",
            "Info do sistema",
            "Voltar"};

        int selecionada = _menu.opcaoConfiguracao();
        int inicio = 0;
        if (selecionada > 2)
            inicio = selecionada - 2;
        if (inicio > 2)
            inicio = 2;

        for (int i = 0; i < 4; i++)
        {
            int idx = inicio + i;
            int y = 16 + (i * 10);
            if (idx >= 6)
                break;

            if (idx == selecionada)
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 9);
                _display.setCorDesenho(0);
            }

            char linha[32];
            if (idx == 0)
                snprintf(linha, sizeof(linha), "Duracao: %d min", _menu.configDuracaoPadraoMin());
            else
                snprintf(linha, sizeof(linha), "%s", opcoes[idx]);

            if (idx == 0)
                desenharIconeSubmenu(_display, 2, y, 2);
            else if (idx == 1)
                desenharIconeSubmenu(_display, 2, y, 4);
            else if (idx == 2)
                desenharIconeSubmenu(_display, 2, y, 6);
            else if (idx == 3)
                desenharIconeSubmenu(_display, 2, y, 5);
            else if (idx == 4)
                desenharIconeSubmenu(_display, 2, y, 1);
            else
            {
                _display.desenharLinha(3, y + 4, 10, y + 4);
                _display.desenharLinha(3, y + 4, 6, y + 1);
                _display.desenharLinha(3, y + 4, 6, y + 7);
            }

            _display.desenharTexto(14, y, linha);
            if (idx == selecionada)
                _display.setCorDesenho(1);
        }

        _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
        _display.desenharTextoMini(0, 56, "OK entra | Segure menu");
        return;
    }

    if (etapa == EtapaConfiguracao::TESTE_VALVULAS)
    {
        desenharCabecalho("TESTE VALVULAS");

        int setorAtual = _menu.configSetorTeste();
        const int linhasVisiveis = 3;
        int inicio = (setorAtual / linhasVisiveis) * linhasVisiveis;
        int fim = inicio + linhasVisiveis;
        if (fim > NUM_VALVULAS)
            fim = NUM_VALVULAS;

        for (int i = inicio; i < fim; i++)
        {
            int linhaIdx = i - inicio;
            int y = 16 + (linhaIdx * 11);
            bool aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);

            if (i == setorAtual)
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 10);
                _display.setCorDesenho(0);
            }

            char linha[24];
            snprintf(linha, sizeof(linha), "Setor %d   [%c]", i + 1, aberta ? 'X' : ' ');
            _display.desenharTexto(2, y, linha);

            if (i == setorAtual)
            {
                _display.setCorDesenho(1);
            }
        }

        char rodape[28];
        snprintf(rodape, sizeof(rodape), "OK toggle | Pg %d/%d", (inicio / linhasVisiveis) + 1,
                 (NUM_VALVULAS + linhasVisiveis - 1) / linhasVisiveis);
        _display.desenharLinha(0, 50, OLED_LARGURA - 1, 50);
        _display.desenharTextoMini(0, 52, "MODO MANUTENCAO");
        _display.desenharTextoMini(0, 56, rodape);
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_HORA)
    {
        desenharCabecalho("AJUSTAR HORA");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02d", _menu.configHora());
        _display.desenharTextoGrande(52, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_MINUTO)
    {
        desenharCabecalho("AJUSTAR MINUTO");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02d", _menu.configMinuto());
        _display.desenharTextoGrande(52, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_DIA)
    {
        desenharCabecalho("AJUSTAR DIA");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02d", _menu.configDia());
        _display.desenharTextoGrande(52, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_MES)
    {
        desenharCabecalho("AJUSTAR MES");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02d", _menu.configMes());
        _display.desenharTextoGrande(52, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_ANO)
    {
        desenharCabecalho("AJUSTAR ANO");
        char valor[16];
        snprintf(valor, sizeof(valor), "%04d", _menu.configAno());
        _display.desenharTextoGrande(40, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_TIMEOUT_MANUAL)
    {
        desenharCabecalho("TIMEOUT MANUAL");
        char valor[16];
        snprintf(valor, sizeof(valor), "%dmin", _menu.configTimeoutManualMin());
        _display.desenharTextoGrande(36, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK salva");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_DURACAO_PADRAO)
    {
        desenharCabecalho("DURACAO PADRAO");
        char valor[16];
        snprintf(valor, sizeof(valor), "%dmin", _menu.configDuracaoPadraoMin());
        _display.desenharTextoGrande(36, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK salva");
        return;
    }

    if (etapa == EtapaConfiguracao::CONFIRMAR_LIMPAR_AGENDAS)
    {
        desenharCabecalho("LIMPAR AGENDAS?");
        _display.desenharTextoMini(0, 20, "Apagar todas as agendas?");
        _display.desenharTextoMini(0, 30, "Gire: SIM/NAO | OK confirma");

        int opc = _menu.opcaoConfirmarLimparAgendas();
        if (opc == 0)
        {
            _display.desenharRetanguloPreenchido(8, 40, 48, 12);
            _display.setCorDesenho(0);
            _display.desenharTexto(22, 42, "SIM");
            _display.setCorDesenho(1);

            _display.desenharRetangulo(72, 40, 48, 12);
            _display.desenharTexto(88, 42, "NAO");
        }
        else
        {
            _display.desenharRetangulo(8, 40, 48, 12);
            _display.desenharTexto(22, 42, "SIM");

            _display.desenharRetanguloPreenchido(72, 40, 48, 12);
            _display.setCorDesenho(0);
            _display.desenharTexto(88, 42, "NAO");
            _display.setCorDesenho(1);
        }
        return;
    }

    if (etapa == EtapaConfiguracao::CONFIRMAR_RESTAURAR_PADRAO)
    {
        desenharCabecalho("RESTAURAR PADRAO?");
        _display.desenharTextoMini(0, 20, "Resetar timeout/duracao?");
        _display.desenharTextoMini(0, 30, "Gire: SIM/NAO | OK confirma");

        int opc = _menu.opcaoConfirmarRestaurarPadrao();
        if (opc == 0)
        {
            _display.desenharRetanguloPreenchido(8, 40, 48, 12);
            _display.setCorDesenho(0);
            _display.desenharTexto(22, 42, "SIM");
            _display.setCorDesenho(1);

            _display.desenharRetangulo(72, 40, 48, 12);
            _display.desenharTexto(88, 42, "NAO");
        }
        else
        {
            _display.desenharRetangulo(8, 40, 48, 12);
            _display.desenharTexto(22, 42, "SIM");

            _display.desenharRetanguloPreenchido(72, 40, 48, 12);
            _display.setCorDesenho(0);
            _display.desenharTexto(88, 42, "NAO");
            _display.setCorDesenho(1);
        }
        return;
    }

    if (etapa == EtapaConfiguracao::INFO_SISTEMA)
    {
        desenharCabecalho("INFO SISTEMA");
        DateTime agora = _rtc.agora();

        char linha1[24];
        snprintf(linha1, sizeof(linha1), "Agendas: %d", _menu.totalAgendasAtivas());
        _display.desenharTexto(0, 16, linha1);

        char linha2[24];
        snprintf(linha2, sizeof(linha2), "RTC: %02d/%02d %02d:%02d", agora.day(), agora.month(), agora.hour(), agora.minute());
        _display.desenharTextoMini(0, 30, linha2);

        char linha3[24];
        snprintf(linha3, sizeof(linha3), "T.Man: %dmin", _menu.configTimeoutManualMin());
        _display.desenharTextoMini(0, 40, linha3);

        char linha4[24];
        snprintf(linha4, sizeof(linha4), "Dur.Pad: %dmin", _menu.configDuracaoPadraoMin());
        _display.desenharTextoMini(0, 48, linha4);

        _display.desenharTextoMini(0, 56, "OK/Segure para voltar");
        return;
    }
}

// ============================================================
//  Componentes reutilizáveis
// ============================================================

void DisplayManager::desenharCabecalho(const char *titulo)
{
    _display.desenharTexto(0, 0, titulo);
    _display.desenharLinha(0, 12, OLED_LARGURA - 1, 12);
}

void DisplayManager::desenharRodapeHora()
{
    DateTime agora = _rtc.agora();

    char hora[9]; // "HH:MM:SS\0"
    snprintf(hora, sizeof(hora), "%02d:%02d:%02d",
             agora.hour(), agora.minute(), agora.second());

    // Rodapé: linha separadora + hora alinhada à direita
    _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
    _display.desenharTexto(80, 56, hora);
}
