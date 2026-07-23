#pragma once

// ============================================================
//  web_ap_manager.h — Gerenciador de Access Point + WebServer
//
//  Responsabilidade: manter rede AP/STA ativa e expor dashboard
//  web com controle completo do sistema de irrigacao.
// ============================================================

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <stdint.h>
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

    void atualizarLeituraClima(float temperaturaC, float umidadePct, bool leituraValida);

    // Estado da execução sequencial de agenda (setores na fila de lotes).
    void atualizarEstadoAgendaSequencial(uint16_t setoresPendentesMask, bool aguardandoIntervalo);

    bool sensorClimaValido() const;
    float temperaturaC() const;
    float umidadeArPct() const;
    unsigned long idadeLeituraClimaMs() const;

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

    AsyncWebServer _server;
    bool _ativo;
    bool _rotasConfiguradas;
    bool _staConfigurada;
    wl_status_t _ultimoStatusSta;
    unsigned long _ultimoRetryStaMs;
    bool _mdnsAtivo;
    bool _mdnsFalhaLogada;
#if WIFI_WEBSOCKET_ENABLED
    AsyncWebSocket _webSocket;
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
    float _sensorTempC;
    float _sensorUmidadePct;
    bool _sensorLeituraValida;
    unsigned long _sensorUltimoOkMs;
    uint16_t _agendaPendenteMask;
    bool _agendaAguardandoIntervalo;

    void configurarRotas();
    void tentarConexaoSta();
    // Busca hora via NTP (a.st1.ntp.br / pool.ntp.org, fuso Brasília fixo) e ajusta o RTC.
    // Chamado ao detectar transição de STA desconectado -> conectado.
    void sincronizarHoraNtp();
    void atualizarMdns();
    void desativarMdns();
#if WIFI_WEBSOCKET_ENABLED
    void iniciarWebSocket();
    void atualizarWebSocket();
    void enviarStatusWebSocket(int clienteId = -1);
    void aoEventoWebSocket(AsyncWebSocket *servidor, AsyncWebSocketClient *cliente,
                           AwsEventType tipo, void *arg, uint8_t *dados, size_t len);
#endif
    void inicializarMonitoramentoEstado();
    void atualizarHistoricoEstado();
    void registrarEvento(const char *tipo, const char *nivel, const String &mensagem);
    void enviarAlertasHistorico(AsyncWebServerRequest *request);
    int contarValvulasManuaisAbertas() const;
    int contarValvulasAutomaticasAbertas() const;
    static String escaparJson(const char *texto);

    void enviarPaginaPrincipal(AsyncWebServerRequest *request);
    void enviarStatusSistema(AsyncWebServerRequest *request);
    void enviarListaAgendas(AsyncWebServerRequest *request);
    String montarJsonStatusSistema();
    static void enviarRespostaJson(AsyncWebServerRequest *request, int statusCode, const String &json);
    static void enviarErroJson(AsyncWebServerRequest *request, int statusCode, const char *mensagem);

    static int lerIndiceValvula(AsyncWebServerRequest *request);
    static int lerSlotAgenda(AsyncWebServerRequest *request);
    static uint8_t gpioValvula(int indice);
    static bool textoParaBool(const String &texto);
    static bool temArg(AsyncWebServerRequest *request, const char *nome);
    static String lerArg(AsyncWebServerRequest *request, const char *nome);
};
