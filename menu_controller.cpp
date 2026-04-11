// ============================================================
//  menu_controller.cpp — Implementação do MenuController
// ============================================================

#include "menu_controller.h"

// Nomes exibidos no display — ordem deve bater com enum ItemMenu
const char *MenuController::_nomesItens[] = {
    "Irrigar Agora",
    "Programar",
    "Configuracoes"};

MenuController::MenuController(ScheduleManager &schedule, RtcDriverDs3231 &rtc, RuntimeConfigManager &config)
    : _schedule(schedule),
      _rtc(rtc),
      _config(config),
      _estado(EstadoMenu::STATUS),
      _itemAtual(0),
      _menuAtivo(false),
      _ultimoEventoMenuMs(0),
      _setorAtual(0),
      _timeoutOcorreu(false),
      _setorTimeout(-1),
      _etapaProgramacao(EtapaProgramacao::SELECIONAR_AGENDA),
      _feedbackProgramacao(FeedbackProgramacao::NENHUM),
      _agendaProgramacao(0),
      _opcaoSubmenuProgramacao(0),
      _opcaoConfirmarExclusao(1),
      _cursorDiaProgramacao(0),
      _cursorSetorProgramacao(0),
      _etapaConfiguracao(EtapaConfiguracao::MENU),
      _opcaoConfiguracao(0),
      _configHora(0),
      _configMinuto(0),
      _configDia(1),
      _configMes(1),
      _configAno(2026),
      _configTimeoutManualMin(10),
      _configDuracaoPadraoMin(DURACAO_PADRAO_MIN),
      _configSetorTeste(0),
      _feedbackConfiguracaoSalvo(false),
      _feedbackConfiguracaoLimpo(false),
      _feedbackConfiguracaoRestaurado(false),
      _opcaoConfirmarLimparAgendas(1),
      _opcaoConfirmarRestaurarPadrao(1)
{
    _agendaEdicao = {true, 6, 0, _config.duracaoPadraoMin(), 0, 0};
    _agendaSelecionada = {false, 6, 0, _config.duracaoPadraoMin(), 0, 0};
}

void MenuController::begin()
{
    _estado = EstadoMenu::STATUS;
    _itemAtual = 0;
    _menuAtivo = false;
    _ultimoEventoMenuMs = millis();
    _setorAtual = 0;
    _timeoutOcorreu = false;
    _setorTimeout = -1;
    _etapaProgramacao = EtapaProgramacao::SELECIONAR_AGENDA;
    _feedbackProgramacao = FeedbackProgramacao::NENHUM;
    _agendaProgramacao = 0;
    _opcaoSubmenuProgramacao = 0;
    _opcaoConfirmarExclusao = 1;
    _cursorDiaProgramacao = 0;
    _cursorSetorProgramacao = 0;
    _agendaEdicao = {true, 6, 0, _config.duracaoPadraoMin(), 0, 0};
    _agendaSelecionada = {false, 6, 0, _config.duracaoPadraoMin(), 0, 0};
    _etapaConfiguracao = EtapaConfiguracao::MENU;
    _opcaoConfiguracao = 0;
    _configHora = 0;
    _configMinuto = 0;
    _configDia = 1;
    _configMes = 1;
    _configAno = 2026;
    _configTimeoutManualMin = (uint16_t)(_config.timeoutManualMs() / 60000UL);
    _configDuracaoPadraoMin = _config.duracaoPadraoMin();
    _configSetorTeste = 0;
    _feedbackConfiguracaoSalvo = false;
    _feedbackConfiguracaoLimpo = false;
    _feedbackConfiguracaoRestaurado = false;
    _opcaoConfirmarLimparAgendas = 1;
    _opcaoConfirmarRestaurarPadrao = 1;

    if (DEBUG_SERIAL)
    {
        Serial.println("[Menu] Inicializado.");
    }
}

void MenuController::processar(DirecaoEncoder direcao, bool botaoPressionado, bool botaoLongoPressionado)
{
    bool houveInteracao = (direcao != DirecaoEncoder::NENHUMA) || botaoPressionado || botaoLongoPressionado;
    if (houveInteracao)
    {
        _ultimoEventoMenuMs = millis();
    }

    if (_menuAtivo && !houveInteracao)
    {
        if ((unsigned long)(millis() - _ultimoEventoMenuMs) >= MENU_TIMEOUT_MS)
        {
            if (DEBUG_SERIAL)
            {
                Serial.println("[Menu] Timeout de inatividade. Voltando ao status.");
            }
            voltar();
            return;
        }
    }

    switch (_estado)
    {

    case EstadoMenu::STATUS:
        // Qualquer interação abre o menu
        if (direcao != DirecaoEncoder::NENHUMA || botaoPressionado || botaoLongoPressionado)
        {
            _menuAtivo = true;
            if (direcao == DirecaoEncoder::HORARIO)
                navegarProximo();
            if (direcao == DirecaoEncoder::ANTI_HORARIO)
                navegarAnterior();
            if (botaoPressionado || botaoLongoPressionado)
                selecionar();
        }
        break;

    case EstadoMenu::IRRIGACAO_MANUAL:
        // Clique longo volta para a tela inicial
        if (botaoLongoPressionado)
        {
            voltar();
            break;
        }

        // Girar navega entre setores
        if (direcao == DirecaoEncoder::HORARIO)
        {
            _setorAtual = (_setorAtual + 1) % NUM_VALVULAS;
            if (DEBUG_SERIAL)
            {
                Serial.print("[Menu] Setor selecionado: ");
                Serial.println(_setorAtual + 1);
            }
        }
        if (direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _setorAtual = (_setorAtual - 1 + NUM_VALVULAS) % NUM_VALVULAS;
            if (DEBUG_SERIAL)
            {
                Serial.print("[Menu] Setor selecionado: ");
                Serial.println(_setorAtual + 1);
            }
        }
        // Botão: toggle do setor OU sair se pressão longa (futuro)
        // Por ora: pressão simples = toggle. Voltar pelo item de menu "Voltar"
        // será adicionado na Fase 5. Por ora pressionar no menu nav volta.
        if (botaoPressionado)
        {
            // Sinaliza toggle — quem executa é o main.ino
            // MenuController só muda estado de UI, não aciona hardware
            // O main.ino consulta setorAtual() e chama irrigacao.toggleValvula()
            if (DEBUG_SERIAL)
            {
                Serial.print("[Menu] Toggle setor: ");
                Serial.println(_setorAtual + 1);
            }
        }
        break;

    case EstadoMenu::PROGRAMAR:
        processarProgramacao(direcao, botaoPressionado, botaoLongoPressionado);
        break;

    case EstadoMenu::CONFIGURACOES:
        processarConfiguracoes(direcao, botaoPressionado, botaoLongoPressionado);
        break;
    }
}

// --- Getters ---

EstadoMenu MenuController::estadoAtual() const { return _estado; }
int MenuController::itemSelecionado() const { return _itemAtual; }
bool MenuController::menuAtivo() const { return _menuAtivo; }
int MenuController::setorAtual() const { return _setorAtual; }
bool MenuController::timeoutOcorreu() const { return _timeoutOcorreu; }

void MenuController::limparTimeout()
{
    _timeoutOcorreu = false;
    _setorTimeout = -1;
}

void MenuController::notificarTimeout(int indiceSetor)
{
    _timeoutOcorreu = true;
    _setorTimeout = indiceSetor;

    if (DEBUG_SERIAL)
    {
        Serial.print("[Menu] Timeout notificado para setor: ");
        Serial.println(indiceSetor + 1);
    }
}

const char *MenuController::nomeItem(int indice) const
{
    int total = (int)ItemMenu::TOTAL;
    if (indice < 0 || indice >= total)
        return "???";
    return _nomesItens[indice];
}

int MenuController::totalItens() const
{
    return (int)ItemMenu::TOTAL;
}

EtapaProgramacao MenuController::etapaProgramacao() const { return _etapaProgramacao; }
int MenuController::agendaProgramacao() const { return _agendaProgramacao; }
int MenuController::opcaoSubmenuProgramacao() const { return _opcaoSubmenuProgramacao; }
int MenuController::opcaoConfirmarExclusao() const { return _opcaoConfirmarExclusao; }
int MenuController::cursorDiaProgramacao() const { return _cursorDiaProgramacao; }
int MenuController::cursorSetorProgramacao() const { return _cursorSetorProgramacao; }
AgendaSetor MenuController::agendaEdicao() const { return _agendaEdicao; }
AgendaSetor MenuController::agendaSelecionada() const { return _agendaSelecionada; }
FeedbackProgramacao MenuController::feedbackProgramacao() const { return _feedbackProgramacao; }
void MenuController::limparFeedbackProgramacao() { _feedbackProgramacao = FeedbackProgramacao::NENHUM; }
EtapaConfiguracao MenuController::etapaConfiguracao() const { return _etapaConfiguracao; }
int MenuController::opcaoConfiguracao() const { return _opcaoConfiguracao; }
int MenuController::configHora() const { return _configHora; }
int MenuController::configMinuto() const { return _configMinuto; }
int MenuController::configDia() const { return _configDia; }
int MenuController::configMes() const { return _configMes; }
int MenuController::configAno() const { return _configAno; }
int MenuController::configTimeoutManualMin() const { return _configTimeoutManualMin; }
int MenuController::configDuracaoPadraoMin() const { return _configDuracaoPadraoMin; }
int MenuController::configSetorTeste() const { return _configSetorTeste; }
bool MenuController::feedbackConfiguracaoSalvo() const { return _feedbackConfiguracaoSalvo; }
void MenuController::limparFeedbackConfiguracaoSalvo() { _feedbackConfiguracaoSalvo = false; }
bool MenuController::feedbackConfiguracaoLimpo() const { return _feedbackConfiguracaoLimpo; }
void MenuController::limparFeedbackConfiguracaoLimpo() { _feedbackConfiguracaoLimpo = false; }
bool MenuController::feedbackConfiguracaoRestaurado() const { return _feedbackConfiguracaoRestaurado; }
void MenuController::limparFeedbackConfiguracaoRestaurado() { _feedbackConfiguracaoRestaurado = false; }
int MenuController::opcaoConfirmarLimparAgendas() const { return _opcaoConfirmarLimparAgendas; }
int MenuController::opcaoConfirmarRestaurarPadrao() const { return _opcaoConfirmarRestaurarPadrao; }
int MenuController::totalAgendasAtivas() const { return _schedule.totalAtivas(); }

// --- Privados ---

void MenuController::navegarProximo()
{
    _itemAtual = (_itemAtual + 1) % totalItens();
    if (DEBUG_SERIAL)
    {
        Serial.print("[Menu] Item: ");
        Serial.println(nomeItem(_itemAtual));
    }
}

void MenuController::navegarAnterior()
{
    _itemAtual = (_itemAtual - 1 + totalItens()) % totalItens();
    if (DEBUG_SERIAL)
    {
        Serial.print("[Menu] Item: ");
        Serial.println(nomeItem(_itemAtual));
    }
}

void MenuController::selecionar()
{
    if (DEBUG_SERIAL)
    {
        Serial.print("[Menu] Selecionado: ");
        Serial.println(nomeItem(_itemAtual));
    }

    switch ((ItemMenu)_itemAtual)
    {
    case ItemMenu::IRRIGACAO_MANUAL:
        _estado = EstadoMenu::IRRIGACAO_MANUAL;
        _setorAtual = 0; // sempre começa no setor 1
        break;
    case ItemMenu::PROGRAMAR:
        _estado = EstadoMenu::PROGRAMAR;
        entrarProgramacao();
        break;
    case ItemMenu::CONFIGURACOES:
        _estado = EstadoMenu::CONFIGURACOES;
        entrarConfiguracoes();
        break;
    default:
        break;
    }
}

void MenuController::voltar()
{
    _estado = EstadoMenu::STATUS;
    _itemAtual = 0;
    _menuAtivo = false;
    _setorAtual = 0;

    if (DEBUG_SERIAL)
    {
        Serial.println("[Menu] Voltou ao Status.");
    }
}

void MenuController::entrarProgramacao()
{
    _feedbackProgramacao = FeedbackProgramacao::NENHUM;
    _agendaProgramacao = 0;
    _opcaoSubmenuProgramacao = 0;
    _opcaoConfirmarExclusao = 1;
    _cursorDiaProgramacao = 0;
    _cursorSetorProgramacao = 0;
    _etapaProgramacao = EtapaProgramacao::SELECIONAR_AGENDA;
    carregarAgendaSelecionada();
}

void MenuController::processarProgramacao(DirecaoEncoder direcao, bool botaoPressionado, bool botaoLongoPressionado)
{
    if (botaoLongoPressionado)
    {
        if (_etapaProgramacao == EtapaProgramacao::SELECIONAR_AGENDA)
        {
            voltar();
            return;
        }

        if (_etapaProgramacao == EtapaProgramacao::SUBMENU_AGENDA)
        {
            _etapaProgramacao = EtapaProgramacao::SELECIONAR_AGENDA;
            return;
        }

        if (_etapaProgramacao == EtapaProgramacao::CONFIRMAR_EXCLUSAO)
        {
            _etapaProgramacao = EtapaProgramacao::SUBMENU_AGENDA;
            return;
        }

        if (_etapaProgramacao == EtapaProgramacao::EDIT_DIAS)
        {
            _etapaProgramacao = EtapaProgramacao::SUBMENU_AGENDA;
            _feedbackProgramacao = FeedbackProgramacao::NENHUM;
            return;
        }

        if (_etapaProgramacao == EtapaProgramacao::EDIT_SETORES)
        {
            _etapaProgramacao = EtapaProgramacao::SUBMENU_AGENDA;
            return;
        }

        if (_etapaProgramacao == EtapaProgramacao::EDIT_HORA ||
            _etapaProgramacao == EtapaProgramacao::EDIT_MINUTO ||
            _etapaProgramacao == EtapaProgramacao::EDIT_DURACAO)
        {
            _etapaProgramacao = EtapaProgramacao::SUBMENU_AGENDA;
            return;
        }

        _etapaProgramacao = EtapaProgramacao::SELECIONAR_AGENDA;
        return;
    }

    switch (_etapaProgramacao)
    {
    case EtapaProgramacao::SELECIONAR_AGENDA:
        if (direcao == DirecaoEncoder::HORARIO)
        {
            _agendaProgramacao = (_agendaProgramacao + 1) % MAX_AGENDAS_TOTAIS;
            _feedbackProgramacao = FeedbackProgramacao::NENHUM;
            carregarAgendaSelecionada();
        }
        if (direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _agendaProgramacao = (_agendaProgramacao - 1 + MAX_AGENDAS_TOTAIS) % MAX_AGENDAS_TOTAIS;
            _feedbackProgramacao = FeedbackProgramacao::NENHUM;
            carregarAgendaSelecionada();
        }
        if (botaoPressionado)
        {
            prepararEdicaoDaAgenda();
            _opcaoSubmenuProgramacao = 0;
            _etapaProgramacao = EtapaProgramacao::SUBMENU_AGENDA;
            _feedbackProgramacao = FeedbackProgramacao::NENHUM;
        }
        break;

    case EtapaProgramacao::SUBMENU_AGENDA:
        if (direcao == DirecaoEncoder::HORARIO)
        {
            _opcaoSubmenuProgramacao = (_opcaoSubmenuProgramacao + 1) % 7;
        }
        if (direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _opcaoSubmenuProgramacao = (_opcaoSubmenuProgramacao - 1 + 7) % 7;
        }
        if (botaoPressionado)
        {
            switch (_opcaoSubmenuProgramacao)
            {
            case 0:
                _etapaProgramacao = EtapaProgramacao::EDIT_HORA;
                break;
            case 1:
                _etapaProgramacao = EtapaProgramacao::EDIT_MINUTO;
                break;
            case 2:
                _etapaProgramacao = EtapaProgramacao::EDIT_DURACAO;
                break;
            case 3:
                _etapaProgramacao = EtapaProgramacao::EDIT_DIAS;
                break;
            case 4:
                _etapaProgramacao = EtapaProgramacao::EDIT_SETORES;
                break;
            case 5:
                if (salvarAgendaEdicao())
                {
                    _feedbackProgramacao = FeedbackProgramacao::SALVO;
                    _etapaProgramacao = EtapaProgramacao::SELECIONAR_AGENDA;
                    carregarAgendaSelecionada();
                }
                break;
            case 6:
                _opcaoConfirmarExclusao = 1; // 0=SIM, 1=NAO (default seguro)
                _etapaProgramacao = EtapaProgramacao::CONFIRMAR_EXCLUSAO;
                break;
            }
        }
        break;

    case EtapaProgramacao::CONFIRMAR_EXCLUSAO:
        if (direcao == DirecaoEncoder::HORARIO || direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _opcaoConfirmarExclusao = (_opcaoConfirmarExclusao == 0) ? 1 : 0;
        }
        if (botaoPressionado)
        {
            if (_opcaoConfirmarExclusao == 0)
            {
                if (excluirAgendaSelecionada())
                {
                    _feedbackProgramacao = FeedbackProgramacao::EXCLUIDO;
                    _etapaProgramacao = EtapaProgramacao::SELECIONAR_AGENDA;
                    carregarAgendaSelecionada();
                }
            }
            else
            {
                _etapaProgramacao = EtapaProgramacao::SUBMENU_AGENDA;
            }
        }
        break;

    case EtapaProgramacao::EDIT_HORA:
    case EtapaProgramacao::EDIT_MINUTO:
    case EtapaProgramacao::EDIT_DURACAO:
    case EtapaProgramacao::EDIT_DIAS:
    case EtapaProgramacao::EDIT_SETORES:
        ajustarCampoEdicao(direcao);
        if (botaoPressionado)
        {
            if (_etapaProgramacao == EtapaProgramacao::EDIT_DIAS)
            {
                alternarDiaCursor();
            }
            else if (_etapaProgramacao == EtapaProgramacao::EDIT_SETORES)
            {
                alternarSetorCursor();
            }
            else
            {
                _etapaProgramacao = EtapaProgramacao::SUBMENU_AGENDA;
            }
            _feedbackProgramacao = FeedbackProgramacao::NENHUM;
        }
        break;
    }
}

void MenuController::carregarAgendaSelecionada()
{
    AgendaSetor agenda;
    if (_schedule.obterAgenda(_agendaProgramacao, agenda))
    {
        _agendaSelecionada = agenda;
    }
}

void MenuController::prepararEdicaoDaAgenda()
{
    _agendaEdicao = _agendaSelecionada;
    if (_agendaEdicao.duracaoMin < 1)
    {
        _agendaEdicao.duracaoMin = _schedule.duracaoPadraoMin();
    }
    if (_agendaEdicao.diasMask == 0)
    {
        _agendaEdicao.diasMask = DIA_SEG;
    }
    if (_agendaEdicao.setoresMask == 0)
    {
        _agendaEdicao.setoresMask = 0x01;
    }
    _agendaEdicao.ativa = true;
    _cursorDiaProgramacao = 0;
    _cursorSetorProgramacao = 0;
}

bool MenuController::salvarAgendaEdicao()
{
    String erro;
    _agendaEdicao.ativa = true;

    if (!_schedule.salvarAgenda(_agendaProgramacao, _agendaEdicao, erro))
    {
        if (erro == "selecione ao menos um dia")
        {
            _feedbackProgramacao = FeedbackProgramacao::ERRO_SEM_DIA;
        }
        else if (erro == "selecione ao menos um setor")
        {
            _feedbackProgramacao = FeedbackProgramacao::ERRO_SEM_SETOR;
        }
        else if (erro == "agenda duplicada")
        {
            _feedbackProgramacao = FeedbackProgramacao::ERRO_DUPLICADA;
        }
        else
        {
            _feedbackProgramacao = FeedbackProgramacao::ERRO_PERSISTENCIA;
        }
        return false;
    }

    return true;
}

bool MenuController::excluirAgendaSelecionada()
{
    if (!_schedule.removerAgenda(_agendaProgramacao))
    {
        _feedbackProgramacao = FeedbackProgramacao::ERRO_EXCLUSAO;
        return false;
    }

    _agendaSelecionada = {false, 6, 0, _schedule.duracaoPadraoMin(), 0, 0};
    _agendaEdicao = _agendaSelecionada;
    return true;
}

void MenuController::ajustarCampoEdicao(DirecaoEncoder direcao)
{
    if (direcao == DirecaoEncoder::NENHUMA)
    {
        return;
    }

    int delta = (direcao == DirecaoEncoder::HORARIO) ? 1 : -1;

    switch (_etapaProgramacao)
    {
    case EtapaProgramacao::EDIT_HORA:
        _agendaEdicao.hora = (_agendaEdicao.hora + 24 + delta) % 24;
        break;
    case EtapaProgramacao::EDIT_MINUTO:
        _agendaEdicao.minuto = (_agendaEdicao.minuto + 60 + delta) % 60;
        break;
    case EtapaProgramacao::EDIT_DURACAO:
    {
        int novaDuracao = (int)_agendaEdicao.duracaoMin + delta;
        if (novaDuracao < 1)
            novaDuracao = 1;
        if (novaDuracao > 240)
            novaDuracao = 240;
        _agendaEdicao.duracaoMin = (uint16_t)novaDuracao;
        break;
    }
    case EtapaProgramacao::EDIT_DIAS:
        _cursorDiaProgramacao = (_cursorDiaProgramacao + 7 + delta) % 7;
        break;
    case EtapaProgramacao::EDIT_SETORES:
        _cursorSetorProgramacao = (_cursorSetorProgramacao + NUM_VALVULAS + delta) % NUM_VALVULAS;
        break;
    default:
        break;
    }
}

void MenuController::alternarDiaCursor()
{
    uint8_t bit = (1 << _cursorDiaProgramacao);
    if (_agendaEdicao.diasMask & bit)
    {
        _agendaEdicao.diasMask &= ~bit;
    }
    else
    {
        _agendaEdicao.diasMask |= bit;
    }
}

void MenuController::alternarSetorCursor()
{
    uint8_t bit = (1 << _cursorSetorProgramacao);
    if (_agendaEdicao.setoresMask & bit)
    {
        _agendaEdicao.setoresMask &= ~bit;
    }
    else
    {
        _agendaEdicao.setoresMask |= bit;
    }
}

void MenuController::entrarConfiguracoes()
{
    DateTime agora = _rtc.agora();
    _etapaConfiguracao = EtapaConfiguracao::MENU;
    _opcaoConfiguracao = 0;
    _configHora = agora.hour();
    _configMinuto = agora.minute();
    _configDia = agora.day();
    _configMes = agora.month();
    _configAno = agora.year();
    _configTimeoutManualMin = (uint16_t)(_config.timeoutManualMs() / 60000UL);
    _configDuracaoPadraoMin = _config.duracaoPadraoMin();
    _configSetorTeste = 0;
    _feedbackConfiguracaoSalvo = false;
    _feedbackConfiguracaoLimpo = false;
    _feedbackConfiguracaoRestaurado = false;
    _opcaoConfirmarLimparAgendas = 1;
    _opcaoConfirmarRestaurarPadrao = 1;
}

void MenuController::processarConfiguracoes(DirecaoEncoder direcao, bool botaoPressionado, bool botaoLongoPressionado)
{
    if (botaoLongoPressionado)
    {
        if (_etapaConfiguracao == EtapaConfiguracao::MENU)
        {
            voltar();
            return;
        }

        if (_etapaConfiguracao == EtapaConfiguracao::SUBMENU_RELOGIO ||
            _etapaConfiguracao == EtapaConfiguracao::SUBMENU_SISTEMA)
        {
            _etapaConfiguracao = EtapaConfiguracao::MENU;
            _opcaoConfiguracao = 0;
            return;
        }

        if (_etapaConfiguracao == EtapaConfiguracao::EDIT_HORA ||
            _etapaConfiguracao == EtapaConfiguracao::EDIT_MINUTO ||
            _etapaConfiguracao == EtapaConfiguracao::EDIT_DIA ||
            _etapaConfiguracao == EtapaConfiguracao::EDIT_MES ||
            _etapaConfiguracao == EtapaConfiguracao::EDIT_ANO ||
            _etapaConfiguracao == EtapaConfiguracao::EDIT_TIMEOUT_MANUAL)
        {
            _etapaConfiguracao = EtapaConfiguracao::SUBMENU_RELOGIO;
            return;
        }

        if (_etapaConfiguracao == EtapaConfiguracao::TESTE_VALVULAS)
        {
            _etapaConfiguracao = EtapaConfiguracao::SUBMENU_SISTEMA;
            return;
        }

        if (_etapaConfiguracao == EtapaConfiguracao::CONFIRMAR_RESTAURAR_PADRAO)
        {
            _etapaConfiguracao = EtapaConfiguracao::SUBMENU_SISTEMA;
            return;
        }

        _etapaConfiguracao = EtapaConfiguracao::SUBMENU_SISTEMA;
        return;
    }

    if (_etapaConfiguracao == EtapaConfiguracao::MENU)
    {
        if (direcao == DirecaoEncoder::HORARIO)
        {
            _opcaoConfiguracao = (_opcaoConfiguracao + 1) % totalOpcoesConfiguracao();
        }
        else if (direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _opcaoConfiguracao = (_opcaoConfiguracao - 1 + totalOpcoesConfiguracao()) % totalOpcoesConfiguracao();
        }

        if (botaoPressionado)
        {
            switch (_opcaoConfiguracao)
            {
            case 0:
                _etapaConfiguracao = EtapaConfiguracao::SUBMENU_RELOGIO;
                _opcaoConfiguracao = 0;
                break;
            case 1:
                _etapaConfiguracao = EtapaConfiguracao::SUBMENU_SISTEMA;
                _opcaoConfiguracao = 0;
                break;
            case 2:
                voltar();
                break;
            }
        }

        return;
    }

    if (_etapaConfiguracao == EtapaConfiguracao::SUBMENU_RELOGIO)
    {
        if (direcao == DirecaoEncoder::HORARIO)
        {
            _opcaoConfiguracao = (_opcaoConfiguracao + 1) % totalOpcoesConfiguracao();
        }
        else if (direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _opcaoConfiguracao = (_opcaoConfiguracao - 1 + totalOpcoesConfiguracao()) % totalOpcoesConfiguracao();
        }

        if (botaoPressionado)
        {
            switch (_opcaoConfiguracao)
            {
            case 0:
                _etapaConfiguracao = EtapaConfiguracao::EDIT_HORA;
                break;
            case 1:
                _etapaConfiguracao = EtapaConfiguracao::EDIT_MINUTO;
                break;
            case 2:
                _etapaConfiguracao = EtapaConfiguracao::EDIT_DIA;
                break;
            case 3:
                _etapaConfiguracao = EtapaConfiguracao::EDIT_MES;
                break;
            case 4:
                _etapaConfiguracao = EtapaConfiguracao::EDIT_ANO;
                break;
            case 5:
                _etapaConfiguracao = EtapaConfiguracao::EDIT_TIMEOUT_MANUAL;
                break;
            case 6:
                if (salvarConfiguracaoRelogio())
                {
                    _feedbackConfiguracaoSalvo = true;
                }
                break;
            case 7:
                _etapaConfiguracao = EtapaConfiguracao::MENU;
                _opcaoConfiguracao = 0;
                break;
            }
        }
        return;
    }

    if (_etapaConfiguracao == EtapaConfiguracao::SUBMENU_SISTEMA)
    {
        if (direcao == DirecaoEncoder::HORARIO)
        {
            _opcaoConfiguracao = (_opcaoConfiguracao + 1) % totalOpcoesConfiguracao();
        }
        else if (direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _opcaoConfiguracao = (_opcaoConfiguracao - 1 + totalOpcoesConfiguracao()) % totalOpcoesConfiguracao();
        }

        if (botaoPressionado)
        {
            switch (_opcaoConfiguracao)
            {
            case 0:
                _etapaConfiguracao = EtapaConfiguracao::EDIT_DURACAO_PADRAO;
                break;
            case 1:
                _etapaConfiguracao = EtapaConfiguracao::TESTE_VALVULAS;
                break;
            case 2:
                _opcaoConfirmarLimparAgendas = 1;
                _etapaConfiguracao = EtapaConfiguracao::CONFIRMAR_LIMPAR_AGENDAS;
                break;
            case 3:
                _opcaoConfirmarRestaurarPadrao = 1;
                _etapaConfiguracao = EtapaConfiguracao::CONFIRMAR_RESTAURAR_PADRAO;
                break;
            case 4:
                _etapaConfiguracao = EtapaConfiguracao::INFO_SISTEMA;
                break;
            case 5:
                _etapaConfiguracao = EtapaConfiguracao::MENU;
                _opcaoConfiguracao = 0;
                break;
            }
        }
        return;
    }

    if (_etapaConfiguracao == EtapaConfiguracao::TESTE_VALVULAS)
    {
        if (direcao == DirecaoEncoder::HORARIO)
        {
            _configSetorTeste = (_configSetorTeste + 1) % NUM_VALVULAS;
        }
        else if (direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _configSetorTeste = (_configSetorTeste - 1 + NUM_VALVULAS) % NUM_VALVULAS;
        }
        return;
    }

    ajustarCampoConfiguracao(direcao);
    if (_etapaConfiguracao == EtapaConfiguracao::CONFIRMAR_LIMPAR_AGENDAS)
    {
        if (botaoPressionado)
        {
            if (_opcaoConfirmarLimparAgendas == 0)
            {
                if (_schedule.limparTodasAgendas())
                {
                    _feedbackConfiguracaoLimpo = true;
                }
            }
            _etapaConfiguracao = EtapaConfiguracao::SUBMENU_SISTEMA;
            _opcaoConfiguracao = 0;
        }
        return;
    }

    if (_etapaConfiguracao == EtapaConfiguracao::CONFIRMAR_RESTAURAR_PADRAO)
    {
        if (botaoPressionado)
        {
            if (_opcaoConfirmarRestaurarPadrao == 0)
            {
                if (_config.restaurarPadrao())
                {
                    _configTimeoutManualMin = (uint16_t)(_config.timeoutManualMs() / 60000UL);
                    _configDuracaoPadraoMin = _config.duracaoPadraoMin();
                    _feedbackConfiguracaoRestaurado = true;
                }
            }
            _etapaConfiguracao = EtapaConfiguracao::SUBMENU_SISTEMA;
            _opcaoConfiguracao = 0;
        }
        return;
    }

    if (_etapaConfiguracao == EtapaConfiguracao::INFO_SISTEMA)
    {
        if (botaoPressionado)
        {
            _etapaConfiguracao = EtapaConfiguracao::SUBMENU_SISTEMA;
            _opcaoConfiguracao = 0;
        }
        return;
    }

    if (botaoPressionado)
    {
        if (_etapaConfiguracao == EtapaConfiguracao::EDIT_TIMEOUT_MANUAL)
        {
            salvarConfiguracoesRuntime();
            _feedbackConfiguracaoSalvo = true;
            _etapaConfiguracao = EtapaConfiguracao::SUBMENU_RELOGIO;
            return;
        }

        if (_etapaConfiguracao == EtapaConfiguracao::EDIT_DURACAO_PADRAO)
        {
            salvarConfiguracoesRuntime();
            _feedbackConfiguracaoSalvo = true;
            _etapaConfiguracao = EtapaConfiguracao::SUBMENU_SISTEMA;
            return;
        }

        _etapaConfiguracao = EtapaConfiguracao::SUBMENU_RELOGIO;
    }
}

void MenuController::ajustarCampoConfiguracao(DirecaoEncoder direcao)
{
    if (direcao == DirecaoEncoder::NENHUMA)
    {
        return;
    }

    int delta = (direcao == DirecaoEncoder::HORARIO) ? 1 : -1;

    switch (_etapaConfiguracao)
    {
    case EtapaConfiguracao::EDIT_HORA:
        _configHora = (_configHora + 24 + delta) % 24;
        break;
    case EtapaConfiguracao::EDIT_MINUTO:
        _configMinuto = (_configMinuto + 60 + delta) % 60;
        break;
    case EtapaConfiguracao::EDIT_DIA:
    {
        int maxDia = diasNoMes(_configAno, _configMes);
        int novoDia = (int)_configDia + delta;
        if (novoDia < 1)
            novoDia = maxDia;
        if (novoDia > maxDia)
            novoDia = 1;
        _configDia = (uint8_t)novoDia;
        break;
    }
    case EtapaConfiguracao::EDIT_MES:
    {
        int novoMes = (int)_configMes + delta;
        if (novoMes < 1)
            novoMes = 12;
        if (novoMes > 12)
            novoMes = 1;
        _configMes = (uint8_t)novoMes;
        int maxDia = diasNoMes(_configAno, _configMes);
        if (_configDia > maxDia)
            _configDia = (uint8_t)maxDia;
        break;
    }
    case EtapaConfiguracao::EDIT_ANO:
    {
        int novoAno = (int)_configAno + delta;
        if (novoAno < 2000)
            novoAno = 2099;
        if (novoAno > 2099)
            novoAno = 2000;
        _configAno = (uint16_t)novoAno;
        int maxDia = diasNoMes(_configAno, _configMes);
        if (_configDia > maxDia)
            _configDia = (uint8_t)maxDia;
        break;
    }
    case EtapaConfiguracao::EDIT_TIMEOUT_MANUAL:
    {
        int novoValor = (int)_configTimeoutManualMin + delta;
        if (novoValor < 1)
            novoValor = 1;
        if (novoValor > 120)
            novoValor = 120;
        _configTimeoutManualMin = (uint16_t)novoValor;
        break;
    }
    case EtapaConfiguracao::EDIT_DURACAO_PADRAO:
    {
        int novoValor = (int)_configDuracaoPadraoMin + delta;
        if (novoValor < 1)
            novoValor = 1;
        if (novoValor > 240)
            novoValor = 240;
        _configDuracaoPadraoMin = (uint16_t)novoValor;
        break;
    }
    case EtapaConfiguracao::TESTE_VALVULAS:
        break;
    case EtapaConfiguracao::SUBMENU_RELOGIO:
    case EtapaConfiguracao::SUBMENU_SISTEMA:
        break;
    case EtapaConfiguracao::CONFIRMAR_LIMPAR_AGENDAS:
        if (direcao == DirecaoEncoder::HORARIO || direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _opcaoConfirmarLimparAgendas = (_opcaoConfirmarLimparAgendas == 0) ? 1 : 0;
        }
        break;
    case EtapaConfiguracao::CONFIRMAR_RESTAURAR_PADRAO:
        if (direcao == DirecaoEncoder::HORARIO || direcao == DirecaoEncoder::ANTI_HORARIO)
        {
            _opcaoConfirmarRestaurarPadrao = (_opcaoConfirmarRestaurarPadrao == 0) ? 1 : 0;
        }
        break;
    case EtapaConfiguracao::INFO_SISTEMA:
        break;
    case EtapaConfiguracao::MENU:
    default:
        break;
    }
}

bool MenuController::salvarConfiguracaoRelogio()
{
    int maxDia = diasNoMes(_configAno, _configMes);
    if (_configDia > maxDia)
    {
        _configDia = (uint8_t)maxDia;
    }

    DateTime novoHorario(_configAno, _configMes, _configDia, _configHora, _configMinuto, 0);
    _rtc.ajustarHora(novoHorario);
    return true;
}

int MenuController::totalOpcoesConfiguracao() const
{
    if (_etapaConfiguracao == EtapaConfiguracao::MENU)
        return 3;
    if (_etapaConfiguracao == EtapaConfiguracao::SUBMENU_RELOGIO)
        return 8;
    if (_etapaConfiguracao == EtapaConfiguracao::SUBMENU_SISTEMA)
        return 6;
    return 3;
}

bool MenuController::salvarConfiguracoesRuntime()
{
    bool okTimeout = _config.salvarTimeoutManualMs((uint32_t)_configTimeoutManualMin * 60000UL);
    bool okDuracao = _config.salvarDuracaoPadraoMin(_configDuracaoPadraoMin);
    return okTimeout && okDuracao;
}

bool MenuController::anoBissexto(int ano)
{
    if ((ano % 400) == 0)
        return true;
    if ((ano % 100) == 0)
        return false;
    return (ano % 4) == 0;
}

int MenuController::diasNoMes(int ano, int mes)
{
    static const uint8_t diasPorMes[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (mes < 1 || mes > 12)
        return 31;
    if (mes == 2 && anoBissexto(ano))
        return 29;
    return diasPorMes[mes - 1];
}
