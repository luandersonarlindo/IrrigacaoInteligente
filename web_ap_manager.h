#pragma once

// ============================================================
//  web_ap_manager.h — Gerenciador de Access Point + WebServer
//
//  Responsabilidade: manter rede AP/STA ativa e expor dashboard
//  web com controle completo do sistema de irrigacao.
// ============================================================

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "Config.h"
#include "irrigation_controller.h"
#include "schedule_manager.h"
#include "runtime_config_manager.h"
#include "rtc_driver_ds3231.h"

class WebApManager
{
public:
    WebApManager(IrrigationController &irrigacao,
                 ScheduleManager &schedule,
                 RuntimeConfigManager &config,
                 RtcDriverDs3231 &rtc);

    void begin();
    void atualizar();

    bool iniciarApServidor();
    void pararApServidor();

    bool ativo() const;
    const char *ssid() const;
    const char *senha() const;
    String ipTexto() const;
    String ipStaTexto() const;
    bool staConectada() const;
    String urlAcessoAp() const;
    String urlAcessoSta() const;
    String urlAcesso() const;

private:
    IrrigationController &_irrigacao;
    ScheduleManager &_schedule;
    RuntimeConfigManager &_config;
    RtcDriverDs3231 &_rtc;

    WebServer _server;
    bool _ativo;
    bool _rotasConfiguradas;
    bool _staConfigurada;
    wl_status_t _ultimoStatusSta;
    unsigned long _ultimoRetryStaMs;

    void configurarRotas();
    void tentarConexaoSta();

    void enviarPaginaPrincipal();
    void enviarStatusSistema();
    void enviarListaAgendas();
    void enviarRespostaJson(int statusCode, const String &json);
    void enviarErroJson(int statusCode, const char *mensagem);

    int lerIndiceValvula() const;
    int lerSlotAgenda() const;
    static uint8_t gpioValvula(int indice);
    static bool textoParaBool(const String &texto);
};
