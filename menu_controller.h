#pragma once

// ============================================================
//  menu_controller.h — Controlador de Menu
//
//  Responsabilidade: gerenciar navegação e estado do menu.
//  Recebe eventos do encoder (direção, botão).
//  NÃO desenha nada — avisa o display_manager o que mostrar.
// ============================================================

#include <Arduino.h>
#include "Config.h"
#include "encoder_driver.h"
#include "schedule_manager.h"
#include "rtc_driver_ds3231.h"
#include "runtime_config_manager.h"

// Itens do menu principal
enum class ItemMenu
{
    IRRIGACAO_MANUAL = 0,
    PROGRAMAR,
    WEBSERVER,
    CONFIGURACOES,
    TOTAL // sentinela — mantém a contagem automática
};

// Estado atual da navegação
enum class EstadoMenu
{
    STATUS, // tela padrão — não é item de menu
    IRRIGACAO_MANUAL,
    PROGRAMAR,
    WEBSERVER,
    CONFIGURACOES
};

enum class EtapaProgramacao
{
    SELECIONAR_AGENDA,
    SUBMENU_AGENDA,
    CONFIRMAR_EXCLUSAO,
    EDIT_HORA,
    EDIT_MINUTO,
    EDIT_DURACAO,
    EDIT_DIAS,
    EDIT_SETORES
};

enum class FeedbackProgramacao
{
    NENHUM,
    SALVO,
    EXCLUIDO,
    ERRO_SEM_DIA,
    ERRO_SEM_SETOR,
    ERRO_DUPLICADA,
    ERRO_PERSISTENCIA,
    ERRO_EXCLUSAO
};

enum class EtapaConfiguracao
{
    MENU,
    SUBMENU_RELOGIO,
    SUBMENU_SISTEMA,
    EDIT_HORA,
    EDIT_MINUTO,
    EDIT_DIA,
    EDIT_MES,
    EDIT_ANO,
    EDIT_TIMEOUT_MANUAL,
    EDIT_DURACAO_PADRAO,
    TESTE_VALVULAS,
    CONFIRMAR_LIMPAR_AGENDAS,
    CONFIRMAR_RESTAURAR_PADRAO,
    INFO_SISTEMA
};

class MenuController
{
public:
    MenuController(ScheduleManager &schedule, RtcDriverDs3231 &rtc, RuntimeConfigManager &config);

    void begin();

    // Processa entrada do encoder e atualiza estado interno
    void processar(DirecaoEncoder direcao, bool botaoPressionado, bool botaoLongoPressionado = false);

    // Getters para o display_manager consultar
    EstadoMenu estadoAtual() const;
    int itemSelecionado() const;
    bool menuAtivo() const;

    // Irrigação manual — setor selecionado no encoder (0 a NUM_VALVULAS-1)
    int setorAtual() const;
    bool opcaoVoltarIrrigacaoSelecionada() const;

    // Notificação de timeout — display_manager consulta para exibir aviso
    bool timeoutOcorreu() const;
    void limparTimeout();

    // Chamado pelo main quando IrrigationController reporta timeout
    void notificarTimeout(int indiceSetor);

    // Nomes dos itens para exibição
    const char *nomeItem(int indice) const;

    // Total de itens no menu principal
    int totalItens() const;

    // Programacao Fase 5
    EtapaProgramacao etapaProgramacao() const;
    int agendaProgramacao() const;
    int opcaoSubmenuProgramacao() const;
    int opcaoConfirmarExclusao() const;
    int cursorDiaProgramacao() const;
    int cursorSetorProgramacao() const;
    AgendaSetor agendaEdicao() const;
    AgendaSetor agendaSelecionada() const;
    FeedbackProgramacao feedbackProgramacao() const;
    void limparFeedbackProgramacao();
    bool consumirEventoAgendaExcluida();

    // Configuracoes
    EtapaConfiguracao etapaConfiguracao() const;
    int opcaoConfiguracao() const;
    int configHora() const;
    int configMinuto() const;
    int configDia() const;
    int configMes() const;
    int configAno() const;
    int configTimeoutManualMin() const;
    int configDuracaoPadraoMin() const;
    int configSetorTeste() const;
    bool feedbackConfiguracaoSalvo() const;
    void limparFeedbackConfiguracaoSalvo();
    bool feedbackConfiguracaoLimpo() const;
    void limparFeedbackConfiguracaoLimpo();
    bool feedbackConfiguracaoRestaurado() const;
    void limparFeedbackConfiguracaoRestaurado();
    int opcaoConfirmarLimparAgendas() const;
    int opcaoConfirmarRestaurarPadrao() const;
    int totalAgendasAtivas() const;
    bool proximaAgenda(const DateTime &agora, DateTime &proximaDataHora, AgendaSetor &agenda, int &slot) const;

private:
    ScheduleManager &_schedule;
    RtcDriverDs3231 &_rtc;
    RuntimeConfigManager &_config;

    EstadoMenu _estado;
    int _itemAtual;
    bool _menuAtivo;
    unsigned long _ultimoEventoMenuMs;

    // Irrigação manual
    int _setorAtual;      // setor em foco no encoder (0 a NUM_VALVULAS-1)
    bool _timeoutOcorreu; // flag para display exibir aviso
    int _setorTimeout;    // qual setor foi fechado por timeout

    // Programacao Fase 5
    EtapaProgramacao _etapaProgramacao;
    FeedbackProgramacao _feedbackProgramacao;
    bool _eventoAgendaExcluida;
    int _agendaProgramacao;
    int _opcaoSubmenuProgramacao;
    int _opcaoConfirmarExclusao;
    int _cursorDiaProgramacao;
    int _cursorSetorProgramacao;
    AgendaSetor _agendaEdicao;
    AgendaSetor _agendaSelecionada;

    // Configuracoes
    EtapaConfiguracao _etapaConfiguracao;
    int _opcaoConfiguracao;
    uint8_t _configHora;
    uint8_t _configMinuto;
    uint8_t _configDia;
    uint8_t _configMes;
    uint16_t _configAno;
    uint16_t _configTimeoutManualMin;
    uint16_t _configDuracaoPadraoMin;
    int _configSetorTeste;
    bool _feedbackConfiguracaoSalvo;
    bool _feedbackConfiguracaoLimpo;
    bool _feedbackConfiguracaoRestaurado;
    int _opcaoConfirmarLimparAgendas;
    int _opcaoConfirmarRestaurarPadrao;

    void navegarProximo();
    void navegarAnterior();
    void selecionar();
    void voltar();
    void entrarProgramacao();
    void processarProgramacao(DirecaoEncoder direcao, bool botaoPressionado, bool botaoLongoPressionado);
    void carregarAgendaSelecionada();
    void prepararEdicaoDaAgenda();
    bool salvarAgendaEdicao();
    bool excluirAgendaSelecionada();
    void ajustarCampoEdicao(DirecaoEncoder direcao);
    void alternarDiaCursor();
    void alternarSetorCursor();
    void entrarConfiguracoes();
    void processarConfiguracoes(DirecaoEncoder direcao, bool botaoPressionado, bool botaoLongoPressionado);
    void ajustarCampoConfiguracao(DirecaoEncoder direcao);
    bool salvarConfiguracaoRelogio();
    bool salvarConfiguracoesRuntime();
    int totalOpcoesConfiguracao() const;
    static bool anoBissexto(int ano);
    static int diasNoMes(int ano, int mes);

    static const char *_nomesItens[];
};
