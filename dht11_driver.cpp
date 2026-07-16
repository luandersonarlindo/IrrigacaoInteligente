// ============================================================
//  dht11_driver.cpp — Implementacao do Dht11Driver
// ============================================================

#include "dht11_driver.h"

#include <math.h>

Dht11Driver::Dht11Driver()
#if IRRIGACAO_DHT_BACKEND_DHT
    : _dht(PIN_DHT11, DHT11)
#endif
{
    _temperaturaC = WEB_TEMP_DEFAULT_C;
    _umidadePct = WEB_HUMIDADE_AR_DEFAULT_PCT;
    _temLeitura = false;
    _ultimaLeituraMs = 0;
    _ultimoOkMs = 0;
    _ultimoStatus = 0;
    snprintf(_ultimoStatusTexto, sizeof(_ultimoStatusTexto), "OK");
    SensorHistorico::iniciar(_historico);
    _ultimaAmostraHistoricoMs = 0;
}

void Dht11Driver::begin()
{
#if IRRIGACAO_DHT_BACKEND_DHTESP
    pinMode(PIN_DHT11, INPUT_PULLUP);
    _dht.setup(PIN_DHT11, DHTesp::DHT11);
    if (DEBUG_SERIAL)
    {
        Serial.println("[DHT11] Inicializado (DHTesp). ");
    }
#elif IRRIGACAO_DHT_BACKEND_DHT
    pinMode(PIN_DHT11, INPUT_PULLUP);
    _dht.begin();
    if (DEBUG_SERIAL)
    {
        Serial.println("[DHT11] Inicializado (DHT).");
    }
#else
    if (DEBUG_SERIAL)
    {
        Serial.println("[DHT11] Biblioteca ausente. Leituras desativadas.");
    }
#endif
}

void Dht11Driver::atualizar()
{
#if IRRIGACAO_DHT_BACKEND_NONE
    return;
#else
    unsigned long agora = millis();
    if ((unsigned long)(agora - _ultimaLeituraMs) < DHT11_READ_INTERVAL_MS)
    {
        return;
    }

    _ultimaLeituraMs = agora;

#if IRRIGACAO_DHT_BACKEND_DHTESP
    TempAndHumidity dados = _dht.getTempAndHumidity();
    float umidade = dados.humidity;
    float temperatura = dados.temperature;
    _ultimoStatus = _dht.getStatus();
    const char *statusTxt = _dht.getStatusString();
    if (statusTxt == nullptr || statusTxt[0] == '\0')
    {
        statusTxt = "SEM_STATUS";
    }
    snprintf(_ultimoStatusTexto, sizeof(_ultimoStatusTexto), "%s", statusTxt);
#else
    float umidade = _dht.readHumidity();
    float temperatura = _dht.readTemperature();
    if (isnan(umidade) || isnan(temperatura))
    {
        _ultimoStatus = 1;
        snprintf(_ultimoStatusTexto, sizeof(_ultimoStatusTexto), "ERRO");
    }
    else
    {
        _ultimoStatus = 0;
        snprintf(_ultimoStatusTexto, sizeof(_ultimoStatusTexto), "OK");
    }
#endif

    if (isnan(umidade) || isnan(temperatura))
    {
        if (_temLeitura && (unsigned long)(agora - _ultimoOkMs) <= DHT11_MAX_STALE_MS)
        {
            return;
        }
        _temLeitura = false;
        return;
    }

    _temperaturaC = temperatura;
    _umidadePct = umidade;
    _temLeitura = true;
    _ultimoOkMs = agora;

    bool primeiraAmostra = (SensorHistorico::total(_historico) == 0);
    bool intervaloVencido = (unsigned long)(agora - _ultimaAmostraHistoricoMs) >= DHT11_HISTORICO_INTERVALO_MS;
    if (primeiraAmostra || intervaloVencido)
    {
        SensorHistorico::inserir(_historico, _temperaturaC, _umidadePct);
        _ultimaAmostraHistoricoMs = agora;
    }
#endif
}

bool Dht11Driver::leituraValida() const
{
    if (!_temLeitura)
    {
        return false;
    }

    unsigned long agora = millis();
    return (unsigned long)(agora - _ultimoOkMs) <= DHT11_MAX_STALE_MS;
}

float Dht11Driver::temperaturaC() const
{
    return _temperaturaC;
}

float Dht11Driver::umidadePct() const
{
    return _umidadePct;
}

unsigned long Dht11Driver::idadeLeituraMs() const
{
    if (!_temLeitura)
    {
        return 0;
    }

    return (unsigned long)(millis() - _ultimoOkMs);
}

const char *Dht11Driver::statusTexto() const
{
    return _ultimoStatusTexto;
}

const char *Dht11Driver::backendNome() const
{
#if IRRIGACAO_DHT_BACKEND_DHTESP
    return "DHTesp";
#elif IRRIGACAO_DHT_BACKEND_DHT
    return "DHT";
#else
    return "Nenhum";
#endif
}

const SensorHistorico::Buffer &Dht11Driver::historico() const
{
    return _historico;
}
