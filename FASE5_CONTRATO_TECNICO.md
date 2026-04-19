# 📜 Fase 5 - Contrato Técnico de Agendamento

📅 Data original: 2026-04-10  
🔄 Revisão de alinhamento: 2026-04-19  
✅ Status: implementado (modelo global de agendas)

## 1. ✅ Decisões de produto fechadas (revisão atual)

1. D1: Agendas globais com seleção de setores por máscara de bits.
2. D2: Agendamento semanal por dias da semana.
3. D3: Persistência no ESP32 via NVS para agenda e configurações runtime.
4. D4: Limite de 4 slots de agenda no sistema.
5. D5: Edição por encoder com campos hora, minuto, duração, dias e setores.
6. D6: Execução automática sequencial por lotes, com limite de simultâneos e intervalo entre lotes.
7. D7: Dashboard com endpoint de alertas/eventos e histórico operacional.

## 2. 🎯 Escopo da Fase 5 (implementado)

Implementar agendamento automático semanal para até 8 setores físicos, com até 4 agendas globais.

Capacidade atual:

- 8 setores (relés)
- 4 agendas totais (slots)
- Cada agenda pode atuar em 1..8 setores via `setoresMask`

## 3. 🧾 Modelo de dados (contrato atual)

### 3.1 📌 Estrutura de uma agenda

```cpp
enum DiaSemanaBit {
    DIA_DOM = 1 << 0,
    DIA_SEG = 1 << 1,
    DIA_TER = 1 << 2,
    DIA_QUA = 1 << 3,
    DIA_QUI = 1 << 4,
    DIA_SEX = 1 << 5,
    DIA_SAB = 1 << 6
};

struct AgendaSetor {
    bool ativa;
    uint8_t hora;         // 0..23
    uint8_t minuto;       // 0..59
    uint16_t duracaoMin;  // >= 1 (UI limita em 240)
    uint8_t diasMask;     // bits DOM..SAB
    uint8_t setoresMask;  // bits setores 1..8
};
```

### 3.2 🗃️ Estrutura global de agenda

```cpp
constexpr uint8_t MAX_AGENDAS_TOTAIS = 4;

struct BancoAgendas {
    uint16_t versao;
    uint16_t crc;
    AgendaSetor agendas[MAX_AGENDAS_TOTAIS];
};
```

### 3.3 ⚙️ Configuração runtime persistida

```cpp
struct BancoConfigRuntime {
    uint16_t versao;
    uint16_t crc;
    uint32_t timeoutManualMs;
    uint16_t duracaoPadraoMin;
};
```

### 3.4 🗂️ Cache diário de execução

```cpp
struct CacheExecucaoDiaria {
    uint16_t versao;
    uint16_t crc;
    int32_t ultimaExecucaoDiaPorSlot[MAX_AGENDAS_TOTAIS];
};
```

Finalidade: impedir repetição indevida do mesmo slot no mesmo dia, inclusive após reboot.

## 4. 📏 Regras funcionais obrigatórias

1. Agenda válida: pelo menos 1 dia marcado.
2. Agenda válida: pelo menos 1 setor marcado.
3. Horário válido: hora 0..23 e minuto 0..59.
4. Duração mínima: 1 minuto (UI limita em 240).
5. Não permitir duplicidade exata entre slots:
   hora + minuto + diasMask + setoresMask iguais.
6. Limite de 4 slots de agenda no sistema.
7. Em reboot, recarregar dados persistidos automaticamente.
8. Se versão/CRC estiverem inválidos, resetar banco para padrão seguro.
9. Timeout manual runtime: 1..120 min.
10. Duração padrão runtime: 1..240 min.
11. Slot de agenda não deve disparar duas vezes no mesmo dia após execução confirmada.

## 5. 💾 Política de persistência

Meio: ESP32 NVS (`Preferences`).

Diretrizes:

1. Persistir apenas quando houver alteração relevante.
2. Escrever bloco completo com versão e CRC.
3. Leitura no boot:
   - Se versão e CRC OK: carregar banco.
   - Se inválido: inicializar padrão e salvar.
4. DS3231 não guarda agenda; DS3231 apenas fornece horário confiável.
5. Runtime config segue política equivalente em banco próprio.
6. Cache diário de execução segue versão e CRC, separado do banco principal de agendas.

## 6. ⚙️ Motor de execução de agendas

Avaliação no loop com gatilho por minuto + execução sequencial por lotes.

Regras:

1. Verificar disparo quando o minuto mudar.
2. Para cada agenda ativa e válida no dia atual:
   - Avaliar janela temporal da agenda.
   - Identificar lote atual e lotes pendentes.
   - Gerar durações por setor para enfileiramento.
3. Se o horário atual cair no meio da janela da agenda, retomar no lote-alvo com duração remanescente do lote corrente.
4. Respeitar limite de simultâneos por lote (`MAX_SETOR_SIMULTANEOS_AGENDA`).
5. Respeitar intervalo entre lotes (`INTERVALO_LOTE_AGENDA_MS`).
6. Em conflito no mesmo setor, manter a maior duração.
7. Exclusão de agenda durante execução cancela rotina automática em andamento.
8. Se setor manual estiver aberto e chegar comando automático, manter coerência por maior deadline.

## 7. 🔌 Contrato entre módulos

### 7.1 🧩 Módulos envolvidos

- `schedule_manager.h/.cpp`
- `runtime_config_manager.h/.cpp`
- `irrigation_controller.h/.cpp`
- `menu_controller.h/.cpp`
- `display_manager.h/.cpp`
- `web_ap_manager.h/.cpp`

Responsabilidades principais:

1. `schedule_manager`: CRUD, validação, persistência e avaliação de disparo.
2. `runtime_config_manager`: persistência de timeout e duração padrão.
3. `irrigation_controller`: acionamento de relés e deadlines.
4. `menu_controller`/`display_manager`: UX local no OLED com encoder.
5. `web_ap_manager`: dashboard web e API para status/comandos.

### 7.2 🔄 Integração com o loop atual

No loop principal:

1. Ler encoder e processar menu.
2. Atualizar irrigação (timeouts/deadlines).
3. Avaliar disparos por minuto (RTC).
4. Enfileirar e executar lotes sequenciais.
5. Atualizar estado visual no display.
6. Processar servidor web.

### 7.3 🌐 Contrato mínimo da API web

Rotas principais implementadas:

1. `GET /status`
2. `GET /api/status`
3. `GET /api/schedules`
4. `GET /api/events`
5. `POST /api/valve/toggle`
6. `POST /api/valve/set`
7. `POST /api/valves/off-all`
8. `POST /api/schedule/save`
9. `POST /api/schedule/delete`
10. `POST /api/schedule/clear`
11. `POST /api/config/runtime`
12. `POST /api/rtc/set`

Regras:

1. Parâmetro `index` (válvula) aceita base 1 e base 0.
2. Parâmetro `slot` (agenda) aceita base 1 e base 0.
3. Em `/api/valve/set`, `state` aceita `1/0`, `true/false`, `on/off`, `ligado/desligado`.
4. Erros em `/api/*` retornam JSON com `ok=false` e descrição do erro.
5. Histórico de eventos usa buffer circular de até 40 registros.

## 8. 🖥️ UX da programação no OLED (implementado)

Fluxo de programação:

1. Selecionar slot de agenda (1..4).
2. Abrir submenu da agenda.
3. Opções do submenu:
   - editar hora
   - editar minuto
   - editar duração
   - editar dias
   - editar setores
   - salvar
   - excluir
   - voltar

Interações:

1. Giro: altera valor do campo ou cursor.
2. Clique curto:
   - alterna dia (em `EDIT_DIAS`)
   - alterna setor (em `EDIT_SETORES`)
   - executa ação no submenu
3. Clique longo: atalho implementado na etapa `EDIT_DIAS` para retorno ao submenu.

## 9. 🧠 Máquina de estados da programação (resumo)

```text
SELECIONAR_AGENDA
  -> SUBMENU_AGENDA

SUBMENU_AGENDA
  -> EDIT_HORA
  -> EDIT_MINUTO
  -> EDIT_DURACAO
  -> EDIT_DIAS
  -> EDIT_SETORES
  -> SALVAR
  -> CONFIRMAR_EXCLUSAO
  -> VOLTAR
```

## 10. ✅ Critérios de aceite da Fase 5 (revisados)

1. Usuário consegue criar/editar/excluir até 4 agendas via encoder.
2. Usuário consegue marcar/desmarcar dias e setores por agenda.
3. Agendas persistem após desligar e ligar novamente.
4. Agendas disparam no horário correto com base no RTC.
5. Sistema evita duplicidade exata de agenda.
6. Execução por lotes respeita simultaneidade e intervalo.
7. Não há regressão da irrigação manual existente.
8. Dashboard web apresenta status e aplica comandos sem erro.
9. Endpoint `/api/events` retorna alertas ativos e histórico operacional.
10. Runtime config respeita limites de timeout e duração após salvar.

## 11. 🚧 Limites atuais

1. Modelo atual é global por slot (não por setor).
2. Limite de 4 agendas totais.
3. Sem exceções por data específica (calendário).
4. Sem orquestração avançada multi-zona além da regra de lotes.

## 12. 🛣️ Plano de evolução sugerido

1. Definir formalmente modelo final (global vs por setor) para próxima fase.
2. Se necessário, migrar estrutura para 16 agendas (4x4) com estratégia de compatibilidade.
3. Adicionar testes automatizados para motor de lotes e APIs web.
4. Consolidar critérios de UX para reduzir profundidade de navegação no OLED.

---

Este contrato foi revisado para refletir o comportamento real do firmware no estado atual do projeto.