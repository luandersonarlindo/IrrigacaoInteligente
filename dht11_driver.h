#pragma once

// ============================================================
//  dht11_driver.h — Driver do sensor DHT11
//
//  Responsabilidade: ler temperatura/umidade do hardware.
//  Nao formata texto para display — apenas entrega valores.
// ============================================================

#include <Arduino.h>
#include "Config.h"

#if DHT11_USE_DHTESP
#include <DHTesp.h>
#endif

#if DHT11_USE_DHT
#include <DHT.h>
#endif

#if DHT11_USE_DHTESP && DHT11_USE_DHT
#error "Selecione apenas um backend DHT11 (DHTesp ou DHT)."
#endif

#define IRRIGACAO_DHT_BACKEND_DHTESP DHT11_USE_DHTESP
#define IRRIGACAO_DHT_BACKEND_DHT DHT11_USE_DHT

#if (!IRRIGACAO_DHT_BACKEND_DHTESP) && (!IRRIGACAO_DHT_BACKEND_DHT)
#define IRRIGACAO_DHT_BACKEND_NONE 1
#else
#define IRRIGACAO_DHT_BACKEND_NONE 0
#endif

class Dht11Driver
{
public:
    Dht11Driver();

    // Inicializa o sensor DHT11.
    void begin();

    // Deve ser chamado no loop() — respeita intervalo minimo de leitura.
    void atualizar();

    bool leituraValida() const;
    float temperaturaC() const;
    float umidadePct() const;
    unsigned long idadeLeituraMs() const;
    const char *statusTexto() const;
    const char *backendNome() const;

private:
#if IRRIGACAO_DHT_BACKEND_DHTESP
    DHTesp _dht;
#elif IRRIGACAO_DHT_BACKEND_DHT
    DHT _dht;
#endif
    float _temperaturaC;
    float _umidadePct;
    bool _temLeitura;
    unsigned long _ultimaLeituraMs;
    unsigned long _ultimoOkMs;
    int _ultimoStatus;
    char _ultimoStatusTexto[28];
};
