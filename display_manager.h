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
#include "dht11_driver.h"
#include "irrigation_controller.h"
#include "web_ap_manager.h"

class DisplayManager
{
public:
    DisplayManager(DisplayDriverOled &display,
                   MenuController &menu,
                   RtcDriverDs3231 &rtc,
                   IrrigationController &irrigacao,
                   WebApManager &webAp,
                   Dht11Driver &dht11);

    void begin();

    // Estado da execução sequencial de agenda (lotes + intervalo).
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
    Dht11Driver &_dht11;
    IrrigationController &_irrigacao;
    WebApManager &_webAp;

    // Telas
    void desenharMenuPrincipal();
    void desenharTelaStatus();
    void desenharTelaSensores();
    void desenharTelaIrrigacao();
    void desenharTelaProgramar();
    void desenharTelaConfig();
    void desenharTelaWebServer();

    // Componentes reutilizáveis
    void desenharCabecalho(const char *titulo);
    void desenharRodapeHora();
    void desenharCursor(int y);

    // Controle de atualização (evita redesenho desnecessário)
    unsigned long _ultimaAtualizacao;
    static const unsigned long INTERVALO_ATUALIZACAO_MS = 200;

    // Estado operacional da agenda automática sequencial
    bool _agendaExecucaoAtiva;
    bool _agendaAguardandoIntervalo;
    uint8_t _agendaSetoresEmLote;
    uint8_t _agendaSetoresPendentes;
    uint16_t _agendaSetoresLoteMask;
    uint16_t _agendaSetoresPendentesMask;
};
