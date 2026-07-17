// ============================================================
//  rtc_driver_ds3231.cpp — Implementação do RtcDriverDs3231
// ============================================================

#include "rtc_driver_ds3231.h"

RtcDriverDs3231::RtcDriverDs3231() {}

bool RtcDriverDs3231::begin() {
    if (!_rtc.begin()) {
        if (DEBUG_SERIAL) {
            Serial.println("[RTC] ERRO: DS3231 não encontrado no barramento I2C.");
        }
        return false;
    }

    if (perdeuEnergia()) {
        // O Oscillator Stop Flag pode disparar por instabilidade elétrica
        // momentânea no boot, mesmo com a bateria de backup presente e boa —
        // não implica perda real da hora gravada. Não sobrescrever aqui:
        // a hora é corrigida via NTP (WebApManager) ao conectar no WiFi, ou
        // manualmente pelo usuário (web/display).
        if (DEBUG_SERIAL) {
            Serial.println("[RTC] Aviso: oscillator stop flag ativo (possível perda de hora).");
        }
    }

    if (DEBUG_SERIAL) {
        Serial.println("[RTC] Inicializado.");
    }

    return true;
}

DateTime RtcDriverDs3231::agora() {
    return _rtc.now();
}

void RtcDriverDs3231::ajustarHora(const DateTime& novaHora) {
    _rtc.adjust(novaHora);
}

bool RtcDriverDs3231::perdeuEnergia() {
    return _rtc.lostPower();
}
