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

// Itens do menu principal
enum class ItemMenu
{
    IRRIGACAO_MANUAL = 0,
    PROGRAMAR,
    CONFIGURACOES,
    TOTAL // sentinela — mantém a contagem automática
};

// Estado atual da navegação
enum class EstadoMenu
{
    STATUS, // tela padrão — não é item de menu
    IRRIGACAO_MANUAL,
    PROGRAMAR,
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

class MenuController
{
public:
    explicit MenuController(ScheduleManager &schedule);

    void begin();

    // Processa entrada do encoder e atualiza estado interno
    void processar(DirecaoEncoder direcao, bool botaoPressionado, bool botaoLongoPressionado = false);

    // Getters para o display_manager consultar
    EstadoMenu estadoAtual() const;
    int itemSelecionado() const;
    bool menuAtivo() const;

    // Irrigação manual — setor selecionado no encoder (0 a NUM_VALVULAS-1)
    int setorAtual() const;

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

private:
    ScheduleManager &_schedule;

    EstadoMenu _estado;
    int _itemAtual;
    bool _menuAtivo;

    // Irrigação manual
    int _setorAtual;      // setor em foco no encoder (0 a NUM_VALVULAS-1)
    bool _timeoutOcorreu; // flag para display exibir aviso
    int _setorTimeout;    // qual setor foi fechado por timeout

    // Programacao Fase 5
    EtapaProgramacao _etapaProgramacao;
    FeedbackProgramacao _feedbackProgramacao;
    int _agendaProgramacao;
    int _opcaoSubmenuProgramacao;
    int _opcaoConfirmarExclusao;
    int _cursorDiaProgramacao;
    int _cursorSetorProgramacao;
    AgendaSetor _agendaEdicao;
    AgendaSetor _agendaSelecionada;

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

    static const char *_nomesItens[];
};
