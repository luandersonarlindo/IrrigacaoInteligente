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
#include <stdint.h>
#include "Config.h"
#if defined(__has_include)
#if __has_include(<WebSocketsServer.h>)
#define IRRIGACAO_WS_LIB_AVAILABLE 1
#include <WebSocketsServer.h>
#else
#define IRRIGACAO_WS_LIB_AVAILABLE 0
#endif
#else
#define IRRIGACAO_WS_LIB_AVAILABLE 0
#endif
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
    static const uint8_t MAX_HISTORICO_EVENTOS = 40;

    struct EventoSistema
    {
        uint32_t id;
        uint32_t uptimeS;
        char data[11];
        char hora[9];
        char tipo[20];
        char nivel[10];
        char mensagem[96];
    };

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
    bool _mdnsAtivo;
    bool _mdnsFalhaLogada;
#if WIFI_WEBSOCKET_ENABLED && IRRIGACAO_WS_LIB_AVAILABLE
    WebSocketsServer _webSocket;
    bool _webSocketIniciado;
    unsigned long _ultimoPushStatusWsMs;
#endif
    EventoSistema _historicoEventos[MAX_HISTORICO_EVENTOS];
    uint8_t _historicoCount;
    uint8_t _historicoHead;
    uint32_t _proximoEventoId;
    bool _monitoramentoInicializado;
    bool _ultimoEstadoValvulas[NUM_VALVULAS];
    bool _ultimaOrigemAgenda[NUM_VALVULAS];
    bool _ultimaStaConectada;
    int _ultimoTotalAgendasAtivas;

    void configurarRotas();
    void tentarConexaoSta();
    void atualizarMdns();
    void desativarMdns();
#if WIFI_WEBSOCKET_ENABLED && IRRIGACAO_WS_LIB_AVAILABLE
    void iniciarWebSocket();
    void atualizarWebSocket();
    void enviarStatusWebSocket(int clienteId = -1);
#endif
    void inicializarMonitoramentoEstado();
    void atualizarHistoricoEstado();
    void registrarEvento(const char *tipo, const char *nivel, const String &mensagem);
    void enviarAlertasHistorico();
    int contarValvulasManuaisAbertas() const;
    int contarValvulasAutomaticasAbertas() const;
    static String escaparJson(const char *texto);

    void enviarPaginaPrincipal();
    void enviarStatusSistema();
    void enviarListaAgendas();
    String montarJsonStatusSistema();
    void enviarRespostaJson(int statusCode, const String &json);
    void enviarErroJson(int statusCode, const char *mensagem);

    int lerIndiceValvula() const;
    int lerSlotAgenda() const;
    static uint8_t gpioValvula(int indice);
    static bool textoParaBool(const String &texto);
};
