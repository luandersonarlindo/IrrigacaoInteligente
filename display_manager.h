#pragma once

// ============================================================
//  display_manager.h — Gerenciador de Display
//
//  Responsabilidade: decidir O QUE mostrar no display.
//  Consulta o MenuController para saber o estado atual.
//  Usa o DisplayDriverLcd16x2 para saber COMO mostrar.
//
//  Separação clara:
//    Driver  = como desenhar (hardware)
//    Manager = o que desenhar (lógica de UI)
// ============================================================

#include <Arduino.h>
#include "Config.h"
#include "display_driver_lcd16x2.h"
#include "menu_controller.h"
#include "rtc_driver_ds3231.h"
#include "irrigation_controller.h"
#include "web_ap_manager.h"

class DisplayManager
{
public:
    DisplayManager(DisplayDriverLcd16x2 &display,
                   MenuController &menu,
                   RtcDriverDs3231 &rtc,
                   IrrigationController &irrigacao,
                   WebApManager &webAp);

    void begin();

    // Estado da execucao sequencial de agenda (lotes + intervalo).
    void atualizarEstadoAgendaSequencial(bool ativa,
                                         bool aguardandoIntervalo,
                                         uint8_t setoresEmLote,
                                         uint8_t setoresPendentes,
                                         uint16_t setoresLoteMask,
                                         uint16_t setoresPendentesMask);

    // Deve ser chamado no loop() — renderiza a tela correta
    void atualizar();

private:
    DisplayDriverLcd16x2 &_display;
    MenuController &_menu;
    RtcDriverDs3231 &_rtc;
    IrrigationController &_irrigacao;
    WebApManager &_webAp;

    // Telas
    void desenharMenuPrincipal();
    void desenharTelaStatus();
    void desenharTelaIrrigacao();
    void desenharTelaProgramar();
    void desenharTelaConfig();
    void desenharTelaWebServer();

    // Helpers de texto para LCD 16x2
    void escreverTela(const String &linha1, const String &linha2);
    String ajustar16(const String &texto) const;
    String cortar(const String &texto, uint8_t limite) const;
    String nomeItemPrincipalCurto(int indice) const;
    String nomeSubmenuProgramacaoCurto(int indice) const;
    String nomeOpcaoConfigCurto(EtapaConfiguracao etapa, int indice) const;
    String mensagemFeedbackProgramacao(FeedbackProgramacao feedback) const;
    String diaSigla(uint8_t dia) const;
    int contarBits16(uint16_t mask) const;

    // Controle de atualização (evita redesenho desnecessário)
    unsigned long _ultimaAtualizacao;
    static const unsigned long INTERVALO_ATUALIZACAO_MS = 250;

    // Estado operacional da agenda automatica sequencial
    bool _agendaExecucaoAtiva;
    bool _agendaAguardandoIntervalo;
    uint8_t _agendaSetoresEmLote;
    uint8_t _agendaSetoresPendentes;
    uint16_t _agendaSetoresLoteMask;
    uint16_t _agendaSetoresPendentesMask;
};
