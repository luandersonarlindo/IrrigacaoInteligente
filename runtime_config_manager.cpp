#include "runtime_config_manager.h"

RuntimeConfigManager::RuntimeConfigManager()
{
    inicializarPadrao();
}

bool RuntimeConfigManager::begin()
{
    if (!_prefs.begin(NAMESPACE_NVS, false))
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Config] ERRO: falha ao abrir NVS.");
        }
        return false;
    }

    if (!carregarBanco())
    {
        inicializarPadrao();
        if (!salvarBanco())
        {
            return false;
        }
    }

    if (DEBUG_SERIAL)
    {
        Serial.println("[Config] RuntimeConfigManager inicializado.");
    }

    return true;
}

uint32_t RuntimeConfigManager::timeoutManualMs() const
{
    return _banco.timeoutManualMs;
}

uint16_t RuntimeConfigManager::duracaoPadraoMin() const
{
    return _banco.duracaoPadraoMin;
}

bool RuntimeConfigManager::salvarTimeoutManualMs(uint32_t valorMs)
{
    if (valorMs < 60000UL)
        valorMs = 60000UL;
    if (valorMs > 7200000UL)
        valorMs = 7200000UL;

    _banco.timeoutManualMs = valorMs;
    return salvarBanco();
}

bool RuntimeConfigManager::salvarDuracaoPadraoMin(uint16_t valorMin)
{
    if (valorMin < 1)
        valorMin = 1;
    if (valorMin > 240)
        valorMin = 240;

    _banco.duracaoPadraoMin = valorMin;
    return salvarBanco();
}

void RuntimeConfigManager::inicializarPadrao()
{
    _banco.versao = VERSAO_BANCO;
    _banco.crc = 0;
    _banco.timeoutManualMs = TIMEOUT_MANUAL_MS;
    _banco.duracaoPadraoMin = DURACAO_PADRAO_MIN;
}

bool RuntimeConfigManager::carregarBanco()
{
    size_t esperado = sizeof(BancoConfigRuntime);
    size_t atual = _prefs.getBytesLength(KEY_BANCO);
    if (atual != esperado)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Config] Banco ausente/incompativel. Inicializando padrao.");
        }
        return false;
    }

    size_t lidos = _prefs.getBytes(KEY_BANCO, &_banco, sizeof(BancoConfigRuntime));
    if (lidos != sizeof(BancoConfigRuntime))
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Config] Falha ao ler banco da NVS.");
        }
        return false;
    }

    if (_banco.versao != VERSAO_BANCO)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Config] Versao de banco invalida.");
        }
        return false;
    }

    uint16_t crcSalvo = _banco.crc;
    _banco.crc = 0;
    uint16_t crcCalc = calcularCrc16(reinterpret_cast<const uint8_t *>(&_banco), sizeof(BancoConfigRuntime));
    _banco.crc = crcSalvo;

    if (crcSalvo != crcCalc)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[Config] CRC invalido. Banco resetado.");
        }
        return false;
    }

    return true;
}

bool RuntimeConfigManager::salvarBanco()
{
    _banco.versao = VERSAO_BANCO;
    _banco.crc = 0;
    _banco.crc = calcularCrc16(reinterpret_cast<const uint8_t *>(&_banco), sizeof(BancoConfigRuntime));

    size_t escritos = _prefs.putBytes(KEY_BANCO, &_banco, sizeof(BancoConfigRuntime));
    bool ok = (escritos == sizeof(BancoConfigRuntime));

    if (!ok && DEBUG_SERIAL)
    {
        Serial.println("[Config] ERRO: falha ao salvar configuracoes na NVS.");
    }

    return ok;
}

uint16_t RuntimeConfigManager::calcularCrc16(const uint8_t *dados, size_t tamanho)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < tamanho; i++)
    {
        crc ^= static_cast<uint16_t>(dados[i]) << 8;
        for (uint8_t b = 0; b < 8; b++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ 0x1021;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}
