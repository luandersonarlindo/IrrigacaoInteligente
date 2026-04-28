#pragma once

// ============================================================
//  dht11_driver.h - Driver do sensor DHT11
//
//  Responsabilidade: ler temperatura e umidade do hardware.
//  Nao formata texto para display.
// ============================================================

#include <Arduino.h>
#include <DHT.h>
#include "Config.h"

class Dht11Driver
{
public:
    Dht11Driver();

    void begin();

    // Faz leitura respeitando o intervalo configurado.
    // Retorna true quando houve tentativa de leitura.
    bool atualizar();

    // Retorna true se a ultima leitura foi valida.
    bool ultimaLeitura(float &temperaturaC, float &umidade) const;

    bool leituraOk() const;

private:
    DHT _dht;
    unsigned long _ultimaLeituraMs;
    bool _leituraOk;
    float _temperaturaC;
    float _umidade;
};
