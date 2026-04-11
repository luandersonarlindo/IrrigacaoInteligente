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
        // Ajusta para a hora da compilação como fallback
        // Em produção isso deve vir de uma tela de configuração
        _rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

        if (DEBUG_SERIAL) {
            Serial.println("[RTC] Aviso: RTC sem bateria ou primeira inicialização.");
            Serial.println("[RTC] Hora ajustada para hora da compilação.");
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
