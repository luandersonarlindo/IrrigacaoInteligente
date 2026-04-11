#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"

struct BancoConfigRuntime
{
    uint16_t versao;
    uint16_t crc;
    uint32_t timeoutManualMs;
    uint16_t duracaoPadraoMin;
};

class RuntimeConfigManager
{
public:
    RuntimeConfigManager();

    bool begin();

    uint32_t timeoutManualMs() const;
    uint16_t duracaoPadraoMin() const;

    bool salvarTimeoutManualMs(uint32_t valorMs);
    bool salvarDuracaoPadraoMin(uint16_t valorMin);

private:
    static const uint16_t VERSAO_BANCO = 1;
    static constexpr const char *NAMESPACE_NVS = "irrig_cfg";
    static constexpr const char *KEY_BANCO = "cfg";

    BancoConfigRuntime _banco;
    Preferences _prefs;

    void inicializarPadrao();
    bool carregarBanco();
    bool salvarBanco();

    static uint16_t calcularCrc16(const uint8_t *dados, size_t tamanho);
};
