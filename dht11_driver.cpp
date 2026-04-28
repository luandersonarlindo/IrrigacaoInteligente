// ============================================================
//  dht11_driver.cpp - Implementacao do Dht11Driver
// ============================================================

#include "dht11_driver.h"

Dht11Driver::Dht11Driver()
    : _dht(PIN_DHT11, DHT11),
      _ultimaLeituraMs(0),
      _leituraOk(false),
      _temperaturaC(0.0f),
      _umidade(0.0f)
{
}

void Dht11Driver::begin()
{
    _dht.begin();
}

bool Dht11Driver::atualizar()
{
    unsigned long agora = millis();
    if ((long)(agora - _ultimaLeituraMs) < (long)DHT11_INTERVALO_MS)
    {
        return false;
    }

    _ultimaLeituraMs = agora;

    float umid = _dht.readHumidity();
    float temp = _dht.readTemperature();

    if (isnan(umid) || isnan(temp))
    {
        _leituraOk = false;
        return true;
    }

    _leituraOk = true;
    _umidade = umid;
    _temperaturaC = temp;
    return true;
}

bool Dht11Driver::ultimaLeitura(float &temperaturaC, float &umidade) const
{
    if (!_leituraOk)
    {
        return false;
    }

    temperaturaC = _temperaturaC;
    umidade = _umidade;
    return true;
}

bool Dht11Driver::leituraOk() const
{
    return _leituraOk;
}
