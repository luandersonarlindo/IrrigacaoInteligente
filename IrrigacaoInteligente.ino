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
#include "input_driver.h"
#include "display_driver_lcd16x2.h"
#include "dht11_driver.h"
#include "rtc_driver_ds3231.h"
#include "menu_controller.h"
#include "display_manager.h"
#include "irrigation_controller.h"
#include "schedule_manager.h"
#include "runtime_config_manager.h"

// --- Instâncias globais ---
InputDriver           inputDriver;
DisplayDriverLcd16x2  lcd;
Dht11Driver           dht11;
RtcDriverDs3231       rtc;
RuntimeConfigManager  runtimeConfig;
ScheduleManager       scheduleManager(runtimeConfig);
MenuController        menu(scheduleManager, rtc, runtimeConfig);
IrrigationController  irrigacao(runtimeConfig);
DisplayManager        displayManager(lcd, menu, rtc, irrigacao);
bool                  rtcDisponivel = false;

struct ExecucaoAgendaSequencial {
    bool ativa;
    bool aguardandoIntervalo;
    unsigned long proximoLoteMs;
    bool setorPendente[NUM_VALVULAS];
    bool setorNoLote[NUM_VALVULAS];
    uint16_t duracaoPendenteMin[NUM_VALVULAS];
};

ExecucaoAgendaSequencial execAgenda = {};

bool haSetorPendenteAgenda() {
    for (int i = 0; i < NUM_VALVULAS; i++) {
        if (execAgenda.setorPendente[i]) {
            return true;
        }
    }
    return false;
}

int contarSetoresPendentesAgenda() {
    int total = 0;
    for (int i = 0; i < NUM_VALVULAS; i++) {
        if (execAgenda.setorPendente[i]) {
            total++;
        }
    }
    return total;
}

int contarSetoresNoLoteAgenda() {
    int total = 0;
    for (int i = 0; i < NUM_VALVULAS; i++) {
        if (execAgenda.setorNoLote[i]) {
            total++;
        }
    }
    return total;
}

uint16_t maskSetoresNoLoteAgenda() {
    uint16_t mask = 0;
    for (int i = 0; i < NUM_VALVULAS; i++) {
        if (execAgenda.setorNoLote[i]) {
            mask |= (uint16_t)(1U << i);
        }
    }
    return mask;
}

uint16_t maskSetoresPendentesAgenda() {
    uint16_t mask = 0;
    for (int i = 0; i < NUM_VALVULAS; i++) {
        if (execAgenda.setorPendente[i]) {
            mask |= (uint16_t)(1U << i);
        }
    }
    return mask;
}

bool loteAtualAindaAberto() {
    for (int i = 0; i < NUM_VALVULAS; i++) {
        if (execAgenda.setorNoLote[i] && irrigacao.estadoValvula(i) == EstadoValvula::ABERTA) {
            return true;
        }
    }
    return false;
}

void limparLoteAtual() {
    for (int i = 0; i < NUM_VALVULAS; i++) {
        execAgenda.setorNoLote[i] = false;
    }
}

void cancelarExecucaoAgendaAutomatica() {
    execAgenda.ativa = false;
    execAgenda.aguardandoIntervalo = false;
    execAgenda.proximoLoteMs = 0;

    for (int i = 0; i < NUM_VALVULAS; i++) {
        execAgenda.setorPendente[i] = false;
        execAgenda.setorNoLote[i] = false;
        execAgenda.duracaoPendenteMin[i] = 0;

        // Fecha apenas setores em origem automatica; manual permanece.
        if (irrigacao.valvulaEmAgendamento(i)) {
            irrigacao.fecharValvula(i);
        }
    }

    if (DEBUG_SERIAL) {
        Serial.println("[Agenda] Execucao automatica cancelada por exclusao de agenda.");
    }
}

void iniciarProximoLoteAgenda() {
    int iniciados = 0;
    for (int i = 0; i < NUM_VALVULAS; i++) {
        if (!execAgenda.setorPendente[i]) {
            continue;
        }

        irrigacao.iniciarAgendamento(i, execAgenda.duracaoPendenteMin[i]);
        execAgenda.setorPendente[i] = false;
        execAgenda.setorNoLote[i] = true;
        execAgenda.duracaoPendenteMin[i] = 0;
        iniciados++;

        if (iniciados >= MAX_SETOR_SIMULTANEOS_AGENDA) {
            break;
        }
    }

    execAgenda.ativa = (iniciados > 0) || haSetorPendenteAgenda();
    execAgenda.aguardandoIntervalo = false;
}

void enfileirarDisparosAgenda(const uint16_t duracoesMinPorSetor[NUM_VALVULAS]) {
    bool recebeuNovoDisparo = false;

    for (int i = 0; i < NUM_VALVULAS; i++) {
        uint16_t duracao = duracoesMinPorSetor[i];
        if (duracao == 0) {
            continue;
        }

        recebeuNovoDisparo = true;

        if (execAgenda.setorNoLote[i] && irrigacao.estadoValvula(i) == EstadoValvula::ABERTA) {
            // Se o setor ja esta em execucao, estende apenas se necessario.
            irrigacao.iniciarAgendamento(i, duracao);
            continue;
        }

        if (!execAgenda.setorPendente[i] || duracao > execAgenda.duracaoPendenteMin[i]) {
            execAgenda.setorPendente[i] = true;
            execAgenda.duracaoPendenteMin[i] = duracao;
        }
    }

    if (recebeuNovoDisparo) {
        execAgenda.ativa = true;
    }
}

void atualizarExecucaoAgendaSequencial() {
    if (!execAgenda.ativa) {
        return;
    }

    bool tinhaLoteAtual = false;
    for (int i = 0; i < NUM_VALVULAS; i++) {
        if (execAgenda.setorNoLote[i]) {
            tinhaLoteAtual = true;
            break;
        }
    }

    if (loteAtualAindaAberto()) {
        return;
    }

    if (tinhaLoteAtual) {
        limparLoteAtual();

        if (!haSetorPendenteAgenda()) {
            execAgenda.ativa = false;
            execAgenda.aguardandoIntervalo = false;
            return;
        }

        execAgenda.aguardandoIntervalo = true;
        execAgenda.proximoLoteMs = millis() + INTERVALO_LOTE_AGENDA_MS;
        return;
    }

    if (!haSetorPendenteAgenda()) {
        execAgenda.ativa = false;
        execAgenda.aguardandoIntervalo = false;
        return;
    }

    if (!execAgenda.aguardandoIntervalo) {
        iniciarProximoLoteAgenda();
        return;
    }

    if ((long)(millis() - execAgenda.proximoLoteMs) >= 0) {
        iniciarProximoLoteAgenda();
    }
}

// ============================================================
void setup() {
    if (DEBUG_SERIAL) {
        Serial.begin(BAUD_RATE);
        Serial.println("=== Sistema de Irrigacao Inteligente ===");
        Serial.println("Inicializando modulos...");
    }

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);   // I2C compartilhado: LCD + RTC

    inputDriver.begin();
    lcd.begin();
    dht11.begin();

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

    execAgenda.ativa = false;
    execAgenda.aguardandoIntervalo = false;
    execAgenda.proximoLoteMs = 0;
    for (int i = 0; i < NUM_VALVULAS; i++) {
        execAgenda.setorPendente[i] = false;
        execAgenda.setorNoLote[i] = false;
        execAgenda.duracaoPendenteMin[i] = 0;
    }

    if (DEBUG_SERIAL) {
        Serial.println("[SETUP] Todos os modulos inicializados.");
        Serial.println("========================================");
    }
}

// ============================================================
void loop() {
    // 1. Atualiza estado da entrada local (3 botoes)
    inputDriver.atualizar();

    // 2. Lê eventos de direção/seleção/voltar
    DirecaoNavegacao direcao = inputDriver.lerDirecao();
    bool             botaoCurto = inputDriver.botaoPressionado();
    bool             botaoLongo = inputDriver.botaoLongoPressionado();

    bool houveInteracao = (direcao != DirecaoNavegacao::NENHUMA) || botaoCurto || botaoLongo;
    if (houveInteracao)
    {
        displayManager.notificarInteracao();
    }

    bool emTesteValvulasAntes =
        (menu.estadoAtual() == EstadoMenu::CONFIGURACOES &&
         menu.etapaConfiguracao() == EtapaConfiguracao::TESTE_VALVULAS);

    // 3. Processa navegação no menu
    //    Se está na tela de irrigação e botão pressionado → toggle do setor
    if (menu.estadoAtual() == EstadoMenu::IRRIGACAO_MANUAL) {
        if (botaoLongo) {
            // Clique longo: voltar para tela inicial
            menu.processar(direcao, false, true);
        } else if (botaoCurto) {
            if (menu.opcaoVoltarIrrigacaoSelecionada()) {
                // Clique curto no item Voltar: deixa o MenuController retornar ao status
                menu.processar(direcao, true, false);
            } else {
                // Clique curto: alterna estado da válvula do setor selecionado
                int setor = menu.setorAtual();
                irrigacao.toggleValvula(setor);
                // Não passa o botão curto ao menu — foi consumido aqui
                menu.processar(direcao, false, false);
            }
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

    // Exclusao de agenda interrompe imediatamente a execucao automatica em andamento.
    if (menu.consumirEventoAgendaExcluida()) {
        cancelarExecucaoAgendaAutomatica();
    }

    bool emTesteValvulasDepois =
        (menu.estadoAtual() == EstadoMenu::CONFIGURACOES &&
         menu.etapaConfiguracao() == EtapaConfiguracao::TESTE_VALVULAS);

    if (emTesteValvulasAntes && !emTesteValvulasDepois)
    {
        irrigacao.fecharTodas();
    }

    // 4. Verifica timeout de segurança nas válvulas
    int setorFechado = irrigacao.atualizar();
    if (setorFechado >= 0) {
        menu.notificarTimeout(setorFechado);
    }

    // 5. Avalia disparos de agendamento por minuto e enfileira execucao sequencial
    if (rtcDisponivel) {
        DateTime agora = rtc.agora();
        uint16_t duracoesMinPorSetor[NUM_VALVULAS] = {0};
        scheduleManager.avaliarDisparos(agora, duracoesMinPorSetor);
        enfileirarDisparosAgenda(duracoesMinPorSetor);
    }

    // 6. Processa lotes da agenda (max simultaneos + intervalo entre lotes)
    atualizarExecucaoAgendaSequencial();

    // 6.1 Publica estado da agenda sequencial para o display de status
    displayManager.atualizarEstadoAgendaSequencial(
        execAgenda.ativa,
        execAgenda.aguardandoIntervalo,
        (uint8_t)contarSetoresNoLoteAgenda(),
        (uint8_t)contarSetoresPendentesAgenda(),
        maskSetoresNoLoteAgenda(),
        maskSetoresPendentesAgenda());

    // 6.2 Atualiza leitura do DHT11
    if (dht11.atualizar())
    {
        float temperaturaC = 0.0f;
        float umidade = 0.0f;
        bool ok = dht11.ultimaLeitura(temperaturaC, umidade);
        displayManager.atualizarLeituraDht(ok, temperaturaC, umidade);
    }

    // 7. Renderiza display
    displayManager.atualizar();
}
