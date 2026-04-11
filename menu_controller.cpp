// ============================================================
//  menu_controller.cpp — Implementação do MenuController
// ============================================================

#include "menu_controller.h"

// Nomes exibidos no display — ordem deve bater com enum ItemMenu
const char *MenuController::_nomesItens[] = {
    "Irrigar Agora",
    "Programar",
    "Configuracoes"};

MenuController::MenuController(ScheduleManager &schedule)
    : _schedule(schedule),
      _estado(EstadoMenu::STATUS),
      _itemAtual(0),
      _menuAtivo(false),
      _setorAtual(0),
      _timeoutOcorreu(false),
      _setorTimeout(-1),
      _etapaProgramacao(EtapaProgramacao::SELECIONAR_AGENDA),
      _feedbackProgramacao(FeedbackProgramacao::NENHUM),
      _agendaProgramacao(0),
      _opcaoSubmenuProgramacao(0),
      _opcaoConfirmarExclusao(1),
      _cursorDiaProgramacao(0),
      _cursorSetorProgramacao(0)
{
    _agendaEdicao = {true, 6, 0, DURACAO_PADRAO_MIN, 0, 0};
    _agendaSelecionada = {false, 6, 0, DURACAO_PADRAO_MIN, 0, 0};
}

void MenuController::begin()
{
    _estado = EstadoMenu::STATUS;
    _itemAtual = 0;
    _menuAtivo = false;
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
    _agendaEdicao = {true, 6, 0, DURACAO_PADRAO_MIN, 0, 0};
    _agendaSelecionada = {false, 6, 0, DURACAO_PADRAO_MIN, 0, 0};

    if (DEBUG_SERIAL)
    {
        Serial.println("[Menu] Inicializado.");
    }
}

void MenuController::processar(DirecaoEncoder direcao, bool botaoPressionado, bool botaoLongoPressionado)
{
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
        // Submenus futuros — botão volta por enquanto
        if (botaoPressionado || botaoLongoPressionado)
            voltar();
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
        _agendaEdicao.duracaoMin = DURACAO_PADRAO_MIN;
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

    _agendaSelecionada = {false, 6, 0, DURACAO_PADRAO_MIN, 0, 0};
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
