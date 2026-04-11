#pragma once

// ============================================================
//  rtc_driver_ds3231.h — Driver do RTC DS3231
//  Biblioteca: RTClib
//
//  Responsabilidade: ler e ajustar data/hora do hardware.
//  NÃO formata texto para display — só entrega os dados.
// ============================================================

#include <Arduino.h>
#include <RTClib.h>
#include "Config.h"

class RtcDriverDs3231 {
public:
    RtcDriverDs3231();

    // Inicializa o RTC. Retorna false se não encontrar no barramento.
    bool begin();

    // Retorna o DateTime atual
    DateTime agora();

    // Ajusta data e hora
    void ajustarHora(const DateTime& novaHora);

    // Verifica se o RTC perdeu energia (bateria descarregada ou primeira inicialização)
    bool perdeuEnergia();

private:
    RTC_DS3231 _rtc;
};
