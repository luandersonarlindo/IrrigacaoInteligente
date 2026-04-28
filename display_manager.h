#pragma once

// ============================================================
//  display_manager.h — Gerenciador de Display
//
//  Responsabilidade: decidir O QUE mostrar no display.
//  Consulta o MenuController para saber o estado atual.
//  Usa o DisplayDriverOled para saber COMO mostrar.
//
//  Separação clara:
//    Driver  = como desenhar (hardware)
//    Manager = o que desenhar (lógica de UI)
// ============================================================

#include <Arduino.h>
#include "Config.h"
#include "display_driver_oled.h"
#include "menu_controller.h"
#include "rtc_driver_ds3231.h"
#include "irrigation_controller.h"

class DisplayManager
{
public:
    DisplayManager(DisplayDriverOled &display,
                   MenuController &menu,
                   RtcDriverDs3231 &rtc,
                   IrrigationController &irrigacao);

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
    DisplayDriverOled &_display;
    MenuController &_menu;
    RtcDriverDs3231 &_rtc;
    IrrigationController &_irrigacao;

    // Telas
    void desenharMenuPrincipal();
    void desenharTelaStatus();
    void desenharTelaIrrigacao();
    void desenharTelaProgramar();
    void desenharTelaConfig();

    // Componentes reutilizáveis
    void desenharCabecalho(const char *titulo);
    void desenharRodapeHora();
    void desenharCursor(int y);

    // Controle de atualização (evita redesenho desnecessário)
    unsigned long _ultimaAtualizacao;
    static const unsigned long INTERVALO_ATUALIZACAO_MS = 200;

    // Estado operacional da agenda automatica sequencial
    bool _agendaExecucaoAtiva;
    bool _agendaAguardandoIntervalo;
    uint8_t _agendaSetoresEmLote;
    uint8_t _agendaSetoresPendentes;
    uint16_t _agendaSetoresLoteMask;
    uint16_t _agendaSetoresPendentesMask;
};
