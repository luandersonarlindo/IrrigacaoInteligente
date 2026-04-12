#pragma once

// ============================================================
//  Config.h — Configurações centralizadas do sistema
//  Altere aqui sem precisar tocar nos outros arquivos
// ============================================================

// --- Pinos: Encoder ---
#define PIN_ENCODER_CLK 19
#define PIN_ENCODER_DT 18
#define PIN_ENCODER_BTN 4

// --- Pinos: Display OLED (I2C) ---
#define PIN_OLED_SDA 21
#define PIN_OLED_SCL 22

// --- Pinos: RTC DS3231 (I2C — mesmo barramento do OLED) ---
// DS3231 compartilha SDA/SCL com o OLED
// Endereço I2C do OLED:  0x3C
// Endereço I2C do RTC:   0x68

// --- Pinos: Relés (2 módulos de 4 canais 12V, trigger HIGH) ---
#define PIN_RELAY_1 23
#define PIN_RELAY_2 25
#define PIN_RELAY_3 26
#define PIN_RELAY_4 27
#define PIN_RELAY_5 32
#define PIN_RELAY_6 33
#define PIN_RELAY_7 13
#define PIN_RELAY_8 14

// --- Display OLED ---
#define OLED_LARGURA 128
#define OLED_ALTURA 64

// --- Menu ---
#define MENU_ITENS_MAX 5      // Número máximo de itens no menu principal
#define MENU_TIMEOUT_MS 30000 // Tempo sem interação para voltar ao início (ms)

// --- Encoder ---
#define ENCODER_STEPS_POR_ITEM 2 // Pulsos necessários para mudar 1 item no menu

// --- Irrigação ---
#define NUM_VALVULAS 8                 // Fase atual: 2 módulos de 4 canais
#define MAX_AGENDAS_POR_SETOR 4        // Fase 5: limite de agendas por setor
#define MAX_AGENDAS_TOTAIS 4           // Fase 5 (refino): agendas globais com seleção de setores
#define DURACAO_PADRAO_MIN 10          // Duração padrão de irrigação em minutos
#define TIMEOUT_MANUAL_MS 600000       // 10 min — desliga válvula esquecida (modo manual)
#define MAX_SETOR_SIMULTANEOS_AGENDA 2 // Limite de setores simultaneos em agenda
#define INTERVALO_LOTE_AGENDA_MS 10000 // Intervalo entre lotes da agenda (ms)

// --- Wi-Fi AP + WebServer ---
#define WIFI_AP_SSID "IrrigacaoESP32"
#define WIFI_AP_PASSWORD "12345678"
#define WIFI_AP_CHANNEL 6
#define WIFI_AP_MAX_CONNECTIONS 4
#define WIFI_STA_ENABLED true
#define WIFI_STA_SSID "ARLINDO"
#define WIFI_STA_PASSWORD "96984949"
#define WIFI_STA_RETRY_MS 15000

// --- Web Dashboard (valores padrao enquanto sensores externos nao forem ligados) ---
#define WEB_FORCE_DEFAULT_SECONDS 20
#define WEB_TEMP_DEFAULT_C 28.4f
#define WEB_HUMIDADE_AR_DEFAULT_PCT 72.0f
#define WEB_CHUVA_1H_DEFAULT_MM 0.0f

// --- Debug ---
#define DEBUG_SERIAL false // false = silencia mensagens no monitor serial
#define BAUD_RATE 115200
