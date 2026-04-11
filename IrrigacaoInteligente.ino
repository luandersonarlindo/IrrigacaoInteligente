// ============================================================
//  IrrigacaoInteligente.ino — Ponto de entrada do sistema
//
//  Responsabilidade: inicializar todos os módulos e
//  manter o loop principal limpo e legível.
//
//  Regra: main.ino não tem lógica de negócio.
//  Só instancia, inicializa e delega.
// ============================================================

#include "Config.h"
#include "encoder_driver.h"
#include "display_driver_oled.h"
#include "rtc_driver_ds3231.h"
#include "menu_controller.h"
#include "display_manager.h"
#include "irrigation_controller.h"
#include "schedule_manager.h"
#include "runtime_config_manager.h"

// --- Instâncias globais ---
EncoderDriver         encoder;
DisplayDriverOled     oled;
RtcDriverDs3231       rtc;
RuntimeConfigManager  runtimeConfig;
ScheduleManager       scheduleManager(runtimeConfig);
MenuController        menu(scheduleManager, rtc, runtimeConfig);
IrrigationController  irrigacao(runtimeConfig);
DisplayManager        displayManager(oled, menu, rtc, irrigacao);
bool                  rtcDisponivel = false;

// ============================================================
void setup() {
    if (DEBUG_SERIAL) {
        Serial.begin(BAUD_RATE);
        Serial.println("=== Sistema de Irrigacao Inteligente ===");
        Serial.println("Inicializando modulos...");
    }

    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);   // I2C compartilhado: OLED + RTC

    encoder.begin();
    oled.begin();

    rtcDisponivel = rtc.begin();
    if (!rtcDisponivel) {
        if (DEBUG_SERIAL) {
            Serial.println("[SETUP] Aviso: RTC ausente. Continuando sem hora real.");
        }
    } else if (DEBUG_SERIAL) {
        DateTime agora = rtc.agora();
        Serial.printf("[RTC] Data/Hora atual: %02d/%02d/%04d %02d:%02d:%02d\n",
                      agora.day(), agora.month(), agora.year(),
                      agora.hour(), agora.minute(), agora.second());
    }

    menu.begin();
    runtimeConfig.begin();
    scheduleManager.begin();
    irrigacao.begin();     // configura GPIOs e garante relés desligados
    displayManager.begin();

    if (DEBUG_SERIAL) {
        Serial.println("[SETUP] Todos os modulos inicializados.");
        Serial.println("========================================");
    }
}

// ============================================================
void loop() {
    // 1. Atualiza estado do encoder (debounce do botão)
    encoder.atualizar();

    // 2. Lê eventos do encoder
    DirecaoEncoder direcao = encoder.lerDirecao();
    bool           botaoCurto = encoder.botaoPressionado();
    bool           botaoLongo = encoder.botaoLongoPressionado();

    // 3. Processa navegação no menu
    //    Se está na tela de irrigação e botão pressionado → toggle do setor
    if (menu.estadoAtual() == EstadoMenu::IRRIGACAO_MANUAL) {
        if (botaoLongo) {
            // Clique longo: voltar para tela inicial
            menu.processar(direcao, false, true);
        } else if (botaoCurto) {
            // Clique curto: alterna estado da válvula do setor selecionado
            int setor = menu.setorAtual();
            irrigacao.toggleValvula(setor);
            // Não passa o botão curto ao menu — foi consumido aqui
            menu.processar(direcao, false, false);
        } else {
            menu.processar(direcao, false, false);
        }
    } else if (menu.estadoAtual() == EstadoMenu::CONFIGURACOES &&
               menu.etapaConfiguracao() == EtapaConfiguracao::TESTE_VALVULAS) {
        if (botaoLongo) {
            menu.processar(direcao, false, true);
        } else if (botaoCurto) {
            irrigacao.toggleValvula(menu.configSetorTeste());
            menu.processar(direcao, false, false);
        } else {
            menu.processar(direcao, false, false);
        }
    } else {
        menu.processar(direcao, botaoCurto, botaoLongo);
    }

    // 4. Verifica timeout de segurança nas válvulas
    int setorFechado = irrigacao.atualizar();
    if (setorFechado >= 0) {
        menu.notificarTimeout(setorFechado);
    }

    // 5. Avalia disparos de agendamento por minuto
    if (rtcDisponivel) {
        DateTime agora = rtc.agora();
        uint16_t duracoesMinPorSetor[NUM_VALVULAS] = {0};
        scheduleManager.avaliarDisparos(agora, duracoesMinPorSetor);
        for (int setor = 0; setor < NUM_VALVULAS; setor++) {
            if (duracoesMinPorSetor[setor] > 0) {
                irrigacao.iniciarAgendamento(setor, duracoesMinPorSetor[setor]);
            }
        }
    }

    // 6. Renderiza display
    displayManager.atualizar();
}
