#pragma once

// ============================================================
//  Config.h — Configurações centralizadas do sistema
//  Altere aqui sem precisar tocar nos outros arquivos
// ============================================================

// --- Pinos: Navegação local (3 botões) ---
// Botões em INPUT_PULLUP: pressionado = LOW
#define PIN_BTN_UP 19
#define PIN_BTN_DOWN 18
#define PIN_BTN_SELECT 4

// Compatibilidade com codigo legado que ainda referencia encoder rotativo.
// Mapeia os sinais antigos para a navegacao atual por botoes.
#define PIN_ENCODER_CLK PIN_BTN_UP
#define PIN_ENCODER_DT PIN_BTN_DOWN
#define PIN_ENCODER_BTN PIN_BTN_SELECT

// --- Pinos: Barramento I2C compartilhado (LCD + RTC) ---
#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22

// --- Sensor DHT11 (temperatura e umidade) ---
#define PIN_DHT11 15
#define DHT11_INTERVALO_MS 2000

// --- Pinos: RTC DS3231 (I2C — mesmo barramento do LCD) ---
// DS3231 compartilha SDA/SCL com o LCD
// Endereco I2C padrao do LCD: 0x27 (pode variar para 0x3F)
// Endereco I2C do RTC:        0x68

// --- Pinos: Relés (2 módulos de 4 canais 12V, trigger HIGH) ---
#define PIN_RELAY_1 23
#define PIN_RELAY_2 25
#define PIN_RELAY_3 26
#define PIN_RELAY_4 27
#define PIN_RELAY_5 32
#define PIN_RELAY_6 33
#define PIN_RELAY_7 13
#define PIN_RELAY_8 14

// --- Display LCD 16x2 I2C ---
#define LCD_I2C_ENDERECO 0x27
#define LCD_COLUNAS 16
#define LCD_LINHAS 2

// --- Menu ---
#define MENU_ITENS_MAX 4      // Número máximo de itens no menu principal
#define MENU_TIMEOUT_MS 30000 // Tempo sem interação para voltar ao início (ms)

// --- Navegação por botões ---
#define BUTTON_DEBOUNCE_MS 50
#define BUTTON_LONG_PRESS_MS 1200

// --- Irrigação ---
#define NUM_VALVULAS 8                 // Fase atual: 2 módulos de 4 canais
#define MAX_AGENDAS_POR_SETOR 4        // Fase 5: limite de agendas por setor
#define MAX_AGENDAS_TOTAIS 4           // Fase 5 (refino): agendas globais com seleção de setores
#define DURACAO_PADRAO_MIN 10          // Duração padrão de irrigação em minutos
#define TIMEOUT_MANUAL_MS 600000       // 10 min — desliga válvula esquecida (modo manual)
#define MAX_SETOR_SIMULTANEOS_AGENDA 2 // Limite de setores simultaneos em agenda
#define INTERVALO_LOTE_AGENDA_MS 10000 // Intervalo entre lotes da agenda (ms)

// --- Debug ---
#define DEBUG_SERIAL false // false = silencia mensagens no monitor serial
#define BAUD_RATE 115200
