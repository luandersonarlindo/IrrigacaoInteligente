// ============================================================
//  display_manager.cpp — Implementacao do DisplayManager para LCD 16x2
// ============================================================

#include "display_manager.h"

namespace
{
    constexpr unsigned long INTERVALO_PAGINA_INFO_MS = 2000;
}

DisplayManager::DisplayManager(DisplayDriverLcd16x2 &display,
                               MenuController &menu,
                               RtcDriverDs3231 &rtc,
                               IrrigationController &irrigacao)
    : _display(display),
      _menu(menu),
      _rtc(rtc),
      _irrigacao(irrigacao),
      _ultimaAtualizacao(0),
    _ultimaInteracaoMs(0),
    _dormindo(false),
      _agendaExecucaoAtiva(false),
      _agendaAguardandoIntervalo(false),
      _agendaSetoresEmLote(0),
      _agendaSetoresPendentes(0),
      _agendaSetoresLoteMask(0),
      _agendaSetoresPendentesMask(0),
      _dhtTemLeitura(false),
      _dhtOk(false),
      _dhtTempC(0.0f),
      _dhtUmidade(0.0f)
{
}

void DisplayManager::atualizarEstadoAgendaSequencial(bool ativa,
                                                     bool aguardandoIntervalo,
                                                     uint8_t setoresEmLote,
                                                     uint8_t setoresPendentes,
                                                     uint16_t setoresLoteMask,
                                                     uint16_t setoresPendentesMask)
{
    _agendaExecucaoAtiva = ativa;
    _agendaAguardandoIntervalo = aguardandoIntervalo;
    _agendaSetoresEmLote = setoresEmLote;
    _agendaSetoresPendentes = setoresPendentes;
    _agendaSetoresLoteMask = setoresLoteMask;
    _agendaSetoresPendentesMask = setoresPendentesMask;
}

void DisplayManager::atualizarLeituraDht(bool ok, float temperaturaC, float umidade)
{
    _dhtTemLeitura = true;
    _dhtOk = ok;
    if (ok)
    {
        _dhtTempC = temperaturaC;
        _dhtUmidade = umidade;
    }
}

void DisplayManager::begin()
{
    _ultimaInteracaoMs = millis();
    _dormindo = false;
    _display.setBacklight(true);

    if (DEBUG_SERIAL)
    {
        Serial.println("[DisplayManager] LCD 16x2 inicializado.");
    }
}

void DisplayManager::notificarInteracao()
{
    _ultimaInteracaoMs = millis();
    if (_dormindo)
    {
        _dormindo = false;
        _display.setBacklight(true);
        _ultimaAtualizacao = 0;
    }
}

void DisplayManager::atualizar()
{
    unsigned long agora = millis();

    if (LCD_SLEEP_TIMEOUT_MS > 0)
    {
        if (!_dormindo && (unsigned long)(agora - _ultimaInteracaoMs) >= LCD_SLEEP_TIMEOUT_MS)
        {
            _dormindo = true;
            _display.setBacklight(false);
            return;
        }

        if (_dormindo)
        {
            return;
        }
    }

    unsigned long intervalo = _menu.menuAtivo() ? 130 : INTERVALO_ATUALIZACAO_MS;

    if ((agora - _ultimaAtualizacao) < intervalo)
    {
        return;
    }
    _ultimaAtualizacao = agora;

    _display.limpar();

    switch (_menu.estadoAtual())
    {
    case EstadoMenu::STATUS:
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

void DisplayManager::escreverTela(const String &linha1, const String &linha2)
{
    _display.escreverLinha(0, ajustar16(linha1));
    _display.escreverLinha(1, ajustar16(linha2));
}

String DisplayManager::ajustar16(const String &texto) const
{
    String s = texto;
    if ((int)s.length() > LCD_COLUNAS)
    {
        s = s.substring(0, LCD_COLUNAS);
    }

    while ((int)s.length() < LCD_COLUNAS)
    {
        s += ' ';
    }

    return s;
}

String DisplayManager::cortar(const String &texto, uint8_t limite) const
{
    if ((int)texto.length() <= limite)
    {
        return texto;
    }
    return texto.substring(0, limite);
}

String DisplayManager::nomeItemPrincipalCurto(int indice) const
{
    switch (indice)
    {
    case 0:
        return "Irrigar";
    case 1:
        return "Programar";
    case 2:
        return "Config";
    default:
        return "Menu";
    }
}

String DisplayManager::nomeSubmenuProgramacaoCurto(int indice) const
{
    switch (indice)
    {
    case 0:
        return "Hora";
    case 1:
        return "Minuto";
    case 2:
        return "Duracao";
    case 3:
        return "Dias";
    case 4:
        return "Setores";
    case 5:
        return "Salvar";
    case 6:
        return "Excluir";
    case 7:
        return "Voltar";
    default:
        return "Opcao";
    }
}

String DisplayManager::nomeOpcaoConfigCurto(EtapaConfiguracao etapa, int indice) const
{
    if (etapa == EtapaConfiguracao::MENU)
    {
        switch (indice)
        {
        case 0:
            return "Relogio";
        case 1:
            return "Sistema";
        case 2:
            return "Voltar";
        default:
            return "Config";
        }
    }

    if (etapa == EtapaConfiguracao::SUBMENU_RELOGIO)
    {
        switch (indice)
        {
        case 0:
            return "Hora";
        case 1:
            return "Minuto";
        case 2:
            return "Dia";
        case 3:
            return "Mes";
        case 4:
            return "Ano";
        case 5:
            return "Timeout";
        case 6:
            return "Salvar";
        case 7:
            return "Voltar";
        default:
            return "Relogio";
        }
    }

    if (etapa == EtapaConfiguracao::SUBMENU_SISTEMA)
    {
        switch (indice)
        {
        case 0:
            return "Duracao";
        case 1:
            return "Teste";
        case 2:
            return "Limpar";
        case 3:
            return "Padrao";
        case 4:
            return "Info";
        case 5:
            return "Voltar";
        default:
            return "Sistema";
        }
    }

    return "Config";
}

String DisplayManager::mensagemFeedbackProgramacao(FeedbackProgramacao feedback) const
{
    switch (feedback)
    {
    case FeedbackProgramacao::SALVO:
        return "Agenda salva";
    case FeedbackProgramacao::EXCLUIDO:
        return "Agenda excluida";
    case FeedbackProgramacao::ERRO_SEM_DIA:
        return "Marque 1 dia";
    case FeedbackProgramacao::ERRO_SEM_SETOR:
        return "Marque 1 setor";
    case FeedbackProgramacao::ERRO_DUPLICADA:
        return "Agenda duplicada";
    case FeedbackProgramacao::ERRO_PERSISTENCIA:
        return "Erro ao salvar";
    case FeedbackProgramacao::ERRO_EXCLUSAO:
        return "Erro ao excluir";
    case FeedbackProgramacao::NENHUM:
    default:
        return "";
    }
}

String DisplayManager::diaSigla(uint8_t dia) const
{
    switch (dia)
    {
    case 0:
        return "DOM";
    case 1:
        return "SEG";
    case 2:
        return "TER";
    case 3:
        return "QUA";
    case 4:
        return "QUI";
    case 5:
        return "SEX";
    case 6:
        return "SAB";
    default:
        return "---";
    }
}

int DisplayManager::contarBits16(uint16_t mask) const
{
    int total = 0;
    for (int i = 0; i < 16; i++)
    {
        if ((mask & (1U << i)) != 0)
            total++;
    }
    return total;
}

// ============================================================
//  Telas (adaptadas para LCD 16x2)
// ============================================================

void DisplayManager::desenharMenuPrincipal()
{
    String linha1 = "Menu>" + nomeItemPrincipalCurto(_menu.itemSelecionado());
    escreverTela(linha1, "OK entra Seg sai");
}

void DisplayManager::desenharTelaStatus()
{
    DateTime agora = _rtc.agora();

    int totalAbertas = 0;
    int totalAutoAbertas = 0;
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        if (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA)
        {
            totalAbertas++;
            if (_irrigacao.valvulaEmAgendamento(i))
            {
                totalAutoAbertas++;
            }
        }
    }

    int totalManualAbertas = totalAbertas - totalAutoAbertas;

    char topo[24];
    snprintf(topo, sizeof(topo), "%02d:%02d:%02d V%d/%d",
             agora.hour(),
             agora.minute(),
             agora.second(),
             totalAbertas,
             NUM_VALVULAS);

    String linha2Base;
    if (totalAbertas > 0)
    {
        char tmp[24];
        if (totalManualAbertas > 0 && totalAutoAbertas > 0)
        {
            snprintf(tmp, sizeof(tmp), "MAN %d AUTO %d", totalManualAbertas, totalAutoAbertas);
        }
        else if (totalManualAbertas > 0)
        {
            snprintf(tmp, sizeof(tmp), "Manual ativo %d", totalManualAbertas);
        }
        else
        {
            snprintf(tmp, sizeof(tmp), "Auto ativo %d", totalAutoAbertas);
        }
        linha2Base = tmp;
    }
    else if (_agendaExecucaoAtiva)
    {
        if (_agendaAguardandoIntervalo)
        {
            linha2Base = "Agenda: intervalo";
        }
        else
        {
            char tmp[24];
            snprintf(tmp, sizeof(tmp), "Lote %d Pend %d", _agendaSetoresEmLote, _agendaSetoresPendentes);
            linha2Base = tmp;
        }
    }
    else
    {
        DateTime proximaDataHora;
        AgendaSetor proximaAgenda;
        int slot = -1;
        if (_menu.proximaAgenda(agora, proximaDataHora, proximaAgenda, slot))
        {
            String sigla = diaSigla((uint8_t)proximaDataHora.dayOfTheWeek());
            char tmp[24];
            snprintf(tmp, sizeof(tmp), "Prox %s %02d:%02d",
                     sigla.c_str(),
                     proximaDataHora.hour(),
                     proximaDataHora.minute());
            linha2Base = tmp;
        }
        else
        {
            linha2Base = "Sem agenda ativa";
        }
    }

    String linha2 = linha2Base;
    bool mostrarDht = _dhtTemLeitura && ((millis() / INTERVALO_PAGINA_INFO_MS) % 2 == 1);
    if (mostrarDht)
    {
        if (_dhtOk)
        {
            int temp = (int)(_dhtTempC + (_dhtTempC >= 0.0f ? 0.5f : -0.5f));
            int umid = (int)(_dhtUmidade + 0.5f);
            char tmp[24];
            snprintf(tmp, sizeof(tmp), "T:%dC U:%d%%", temp, umid);
            linha2 = tmp;
        }
        else
        {
            linha2 = "DHT11 erro";
        }
    }

    escreverTela(topo, linha2);
}

void DisplayManager::desenharTelaIrrigacao()
{
    int setorAtual = _menu.setorAtual();

    if (_menu.opcaoVoltarIrrigacaoSelecionada())
    {
        escreverTela("Irrigar >Voltar", "OK volta Seg sai");
        return;
    }

    bool aberta = (_irrigacao.estadoValvula(setorAtual) == EstadoValvula::ABERTA);
    char linha1[24];
    snprintf(linha1, sizeof(linha1), "Setor %d [%s]", setorAtual + 1, aberta ? "ON" : "OFF");

    if (_menu.timeoutOcorreu())
    {
        escreverTela(linha1, "Timeout seguranca");
    }
    else
    {
        escreverTela(linha1, "OK toggle Seg sai");
    }
}

void DisplayManager::desenharTelaProgramar()
{
    EtapaProgramacao etapa = _menu.etapaProgramacao();
    FeedbackProgramacao feedback = _menu.feedbackProgramacao();

    static FeedbackProgramacao ultimoFeedback = FeedbackProgramacao::NENHUM;
    static unsigned long inicioFeedbackMs = 0;

    if (feedback != FeedbackProgramacao::NENHUM && feedback != ultimoFeedback)
    {
        ultimoFeedback = feedback;
        inicioFeedbackMs = millis();
    }

    if ((feedback == FeedbackProgramacao::SALVO || feedback == FeedbackProgramacao::EXCLUIDO) &&
        (millis() - inicioFeedbackMs) > 1400)
    {
        _menu.limparFeedbackProgramacao();
        feedback = FeedbackProgramacao::NENHUM;
        ultimoFeedback = FeedbackProgramacao::NENHUM;
    }

    if (etapa == EtapaProgramacao::SELECIONAR_AGENDA)
    {
        AgendaSetor agenda = _menu.agendaSelecionada();
        int slot = _menu.agendaProgramacao() + 1;

        char linha1[24];
        snprintf(linha1, sizeof(linha1), "Agenda %d/%d %s", slot, MAX_AGENDAS_TOTAIS, agenda.ativa ? "ATV" : "VAZ");

        String linha2;
        if (feedback != FeedbackProgramacao::NENHUM)
        {
            linha2 = mensagemFeedbackProgramacao(feedback);
        }
        else if (agenda.ativa)
        {
            char tmp[24];
            int setores = contarBits16(agenda.setoresMask);
            snprintf(tmp, sizeof(tmp), "%02d:%02d %umin S%d", agenda.hora, agenda.minuto, agenda.duracaoMin, setores);
            linha2 = tmp;
        }
        else
        {
            linha2 = "OK cria/edita";
        }

        escreverTela(linha1, linha2);
        return;
    }

    AgendaSetor agenda = _menu.agendaEdicao();

    if (etapa == EtapaProgramacao::SUBMENU_AGENDA)
    {
        int opcao = _menu.opcaoSubmenuProgramacao();
        String linha1 = "A" + String(_menu.agendaProgramacao() + 1) + ":" + nomeSubmenuProgramacaoCurto(opcao);

        String linha2 = "OK abre Seg sair";
        switch (opcao)
        {
        case 0:
        {
            char tmp[24];
            snprintf(tmp, sizeof(tmp), "Hora: %02dh", agenda.hora);
            linha2 = tmp;
            break;
        }
        case 1:
        {
            char tmp[24];
            snprintf(tmp, sizeof(tmp), "Minuto: %02d", agenda.minuto);
            linha2 = tmp;
            break;
        }
        case 2:
        {
            char tmp[24];
            snprintf(tmp, sizeof(tmp), "Duracao: %umin", agenda.duracaoMin);
            linha2 = tmp;
            break;
        }
        case 3:
        {
            char tmp[24];
            snprintf(tmp, sizeof(tmp), "Dias marcados: %d", contarBits16(agenda.diasMask));
            linha2 = tmp;
            break;
        }
        case 4:
        {
            char tmp[24];
            snprintf(tmp, sizeof(tmp), "Setores: %d", contarBits16(agenda.setoresMask));
            linha2 = tmp;
            break;
        }
        case 5:
            linha2 = "OK para salvar";
            break;
        case 6:
            linha2 = "OK para excluir";
            break;
        case 7:
            linha2 = "OK voltar";
            break;
        }

        if (feedback != FeedbackProgramacao::NENHUM && feedback != FeedbackProgramacao::SALVO && feedback != FeedbackProgramacao::EXCLUIDO)
        {
            linha2 = mensagemFeedbackProgramacao(feedback);
        }

        escreverTela(linha1, linha2);
        return;
    }

    if (etapa == EtapaProgramacao::CONFIRMAR_EXCLUSAO)
    {
        String linha2 = (_menu.opcaoConfirmarExclusao() == 0) ? ">SIM   NAO" : " SIM  >NAO";
        escreverTela("Excluir agenda?", linha2);
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_HORA)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%02dh Seg volta", agenda.hora);
        escreverTela("Editar hora", linha2);
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_MINUTO)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%02d Seg volta", agenda.minuto);
        escreverTela("Editar minuto", linha2);
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_DURACAO)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%umin Seg volta", agenda.duracaoMin);
        escreverTela("Editar duracao", linha2);
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_DIAS)
    {
        int cursor = _menu.cursorDiaProgramacao();
        bool marcado = (agenda.diasMask & (1 << cursor)) != 0;

        String linha1 = "Dia " + diaSigla((uint8_t)cursor) + (marcado ? " [X]" : " [ ]");
        escreverTela(linha1, "OK marca Seg vl");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_SETORES)
    {
        int cursor = _menu.cursorSetorProgramacao();
        if (cursor == NUM_VALVULAS)
        {
            escreverTela("Setores >Voltar", "OK volta submenu");
            return;
        }

        bool marcado = (agenda.setoresMask & (1 << cursor)) != 0;
        char linha1[24];
        snprintf(linha1, sizeof(linha1), "Setor %d [%c]", cursor + 1, marcado ? 'X' : ' ');
        escreverTela(linha1, "OK marca Seg vl");
        return;
    }

    escreverTela("Programacao", "Estado invalido");
}

void DisplayManager::desenharTelaConfig()
{
    EtapaConfiguracao etapa = _menu.etapaConfiguracao();

    static unsigned long expiraMsgMs = 0;
    static String msgCurta;

    if (_menu.feedbackConfiguracaoSalvo())
    {
        msgCurta = "Configuracao salva";
        expiraMsgMs = millis() + 1500;
        _menu.limparFeedbackConfiguracaoSalvo();
    }

    if (_menu.feedbackConfiguracaoLimpo())
    {
        msgCurta = "Agendas apagadas";
        expiraMsgMs = millis() + 1500;
        _menu.limparFeedbackConfiguracaoLimpo();
    }

    if (_menu.feedbackConfiguracaoRestaurado())
    {
        msgCurta = "Padrao restaurado";
        expiraMsgMs = millis() + 1500;
        _menu.limparFeedbackConfiguracaoRestaurado();
    }

    if ((long)(expiraMsgMs - millis()) > 0)
    {
        escreverTela("Configuracoes", msgCurta);
        return;
    }

    if (etapa == EtapaConfiguracao::MENU)
    {
        String linha1 = "Cfg:" + nomeOpcaoConfigCurto(etapa, _menu.opcaoConfiguracao());
        escreverTela(linha1, "OK entra Seg sai");
        return;
    }

    if (etapa == EtapaConfiguracao::SUBMENU_RELOGIO)
    {
        int opcao = _menu.opcaoConfiguracao();
        String linha1 = "Rel:" + nomeOpcaoConfigCurto(etapa, opcao);

        String linha2 = "OK abre Seg menu";
        char tmp[24];
        if (opcao == 0)
        {
            snprintf(tmp, sizeof(tmp), "Hora: %02d", _menu.configHora());
            linha2 = tmp;
        }
        else if (opcao == 1)
        {
            snprintf(tmp, sizeof(tmp), "Minuto: %02d", _menu.configMinuto());
            linha2 = tmp;
        }
        else if (opcao == 2)
        {
            snprintf(tmp, sizeof(tmp), "Dia: %02d", _menu.configDia());
            linha2 = tmp;
        }
        else if (opcao == 3)
        {
            snprintf(tmp, sizeof(tmp), "Mes: %02d", _menu.configMes());
            linha2 = tmp;
        }
        else if (opcao == 4)
        {
            snprintf(tmp, sizeof(tmp), "Ano: %04d", _menu.configAno());
            linha2 = tmp;
        }
        else if (opcao == 5)
        {
            snprintf(tmp, sizeof(tmp), "Timeout: %dmin", _menu.configTimeoutManualMin());
            linha2 = tmp;
        }
        else if (opcao == 6)
        {
            linha2 = "OK salva relogio";
        }
        else if (opcao == 7)
        {
            linha2 = "OK voltar";
        }

        escreverTela(linha1, linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::SUBMENU_SISTEMA)
    {
        int opcao = _menu.opcaoConfiguracao();
        String linha1 = "Sis:" + nomeOpcaoConfigCurto(etapa, opcao);

        String linha2 = "OK abre Seg menu";
        char tmp[24];
        if (opcao == 0)
        {
            snprintf(tmp, sizeof(tmp), "Duracao: %dmin", _menu.configDuracaoPadraoMin());
            linha2 = tmp;
        }
        else if (opcao == 1)
        {
            linha2 = "OK teste valv.";
        }
        else if (opcao == 2)
        {
            linha2 = "OK limpar agendas";
        }
        else if (opcao == 3)
        {
            linha2 = "OK restaurar";
        }
        else if (opcao == 4)
        {
            linha2 = "OK info sistema";
        }
        else if (opcao == 5)
        {
            linha2 = "OK voltar";
        }

        escreverTela(linha1, linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::TESTE_VALVULAS)
    {
        int setor = _menu.configSetorTeste();
        bool aberta = (_irrigacao.estadoValvula(setor) == EstadoValvula::ABERTA);

        char linha1[24];
        snprintf(linha1, sizeof(linha1), "Teste S%d [%s]", setor + 1, aberta ? "ON" : "OFF");
        escreverTela(linha1, "OK toggle Seg sai");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_HORA)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%02d Seg volta", _menu.configHora());
        escreverTela("Ajustar hora", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_MINUTO)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%02d Seg volta", _menu.configMinuto());
        escreverTela("Ajustar minuto", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_DIA)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%02d Seg volta", _menu.configDia());
        escreverTela("Ajustar dia", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_MES)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%02d Seg volta", _menu.configMes());
        escreverTela("Ajustar mes", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_ANO)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%04d Seg volta", _menu.configAno());
        escreverTela("Ajustar ano", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_TIMEOUT_MANUAL)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%dmin Seg volta", _menu.configTimeoutManualMin());
        escreverTela("Timeout manual", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_DURACAO_PADRAO)
    {
        char linha2[24];
        snprintf(linha2, sizeof(linha2), "%dmin Seg volta", _menu.configDuracaoPadraoMin());
        escreverTela("Duracao padrao", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::CONFIRMAR_LIMPAR_AGENDAS)
    {
        String linha2 = (_menu.opcaoConfirmarLimparAgendas() == 0) ? ">SIM   NAO" : " SIM  >NAO";
        escreverTela("Limpar agendas?", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::CONFIRMAR_RESTAURAR_PADRAO)
    {
        String linha2 = (_menu.opcaoConfirmarRestaurarPadrao() == 0) ? ">SIM   NAO" : " SIM  >NAO";
        escreverTela("Restaurar padrao", linha2);
        return;
    }

    if (etapa == EtapaConfiguracao::INFO_SISTEMA)
    {
        unsigned long pagina = (millis() / INTERVALO_PAGINA_INFO_MS) % 3;
        if (pagina == 0)
        {
            char linha1[24];
            char linha2[24];
            snprintf(linha1, sizeof(linha1), "Agendas: %d", _menu.totalAgendasAtivas());
            snprintf(linha2, sizeof(linha2), "Timeout: %dmin", _menu.configTimeoutManualMin());
            escreverTela(linha1, linha2);
        }
        else if (pagina == 1)
        {
            DateTime agora = _rtc.agora();
            char linha1[24];
            char linha2[24];
            snprintf(linha1, sizeof(linha1), "DurPad: %dmin", _menu.configDuracaoPadraoMin());
            snprintf(linha2, sizeof(linha2), "%02d/%02d %02d:%02d", agora.day(), agora.month(), agora.hour(), agora.minute());
            escreverTela(linha1, linha2);
        }
        else
        {
            escreverTela("Info sistema", "OK/Seg para sair");
        }
        return;
    }

    escreverTela("Configuracoes", "Estado invalido");
}
