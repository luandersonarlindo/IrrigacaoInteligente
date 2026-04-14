# 📜 Fase 5 - Contrato Tecnico de Agendamento

📅 Data original: 2026-04-10
🔄 Revisao de alinhamento: 2026-04-12
✅ Status: implementado (modelo global de agendas)

## 1. ✅ Decisoes de produto fechadas (revisao atual)

1. D1: agendas globais com selecao de setores por mascara de bits.
2. D2: agendamento semanal por dias da semana (nao apenas diario).
3. D3: persistencia no ESP32 via NVS para agenda e configuracoes runtime.
4. D4: limite de 4 slots de agenda no sistema.
5. D5: edicao por 4 botoes com campos hora, minuto, duracao, dias e setores.
6. D6: execucao automatica sequencial por lotes, com limite de simultaneos e intervalo entre lotes.

## 2. 🎯 Escopo da Fase 5 (implementado)

Implementar agendamento automatico semanal para ate 8 setores fisicos, com ate 4 agendas globais.

Capacidade atual:

- 8 setores (reles)
- 4 agendas totais (slots)
- cada agenda pode atuar em 1..8 setores via setoresMask

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

### 3.3 ⚙️ Configuracao runtime persistida

```cpp
struct BancoConfigRuntime {
    uint16_t versao;
    uint16_t crc;
    uint32_t timeoutManualMs;
    uint16_t duracaoPadraoMin;
};
```

## 4. 📏 Regras funcionais obrigatorias

1. Agenda valida: pelo menos 1 dia marcado.
2. Agenda valida: pelo menos 1 setor marcado.
3. Horario valido: hora 0..23 e minuto 0..59.
4. Duracao minima: 1 minuto (UI aplica limite superior de 240).
5. Nao permitir duplicidade exata entre slots:
   hora igual + minuto igual + diasMask igual + setoresMask igual.
6. Limite de 4 slots de agenda no sistema.
7. Em reboot, recarregar dados persistidos automaticamente.
8. Se versao/CRC estiverem invalidos, resetar banco para padrao seguro.

## 5. 💾 Politica de persistencia

Meio: ESP32 NVS (Preferences).

Diretrizes:

1. Persistir apenas quando houver alteracao (salvar/editar/excluir/restaurar).
2. Escrever bloco completo com versao e CRC.
3. Leitura no boot:
   - se versao e CRC ok: carregar banco;
   - se invalido: inicializar padrao e salvar.
4. DS3231 nao guarda agenda; DS3231 apenas fornece horario confiavel.
5. Runtime config (timeout e duracao padrao) segue politica equivalente em banco proprio.

## 6. ⚙️ Motor de execucao de agendas

Avaliacao no loop com gatilho por minuto + execucao sequencial por lotes.

Regras:

1. Verificar disparo quando o minuto mudar.
2. Para cada agenda ativa e valida no dia atual:
   - avaliar janela temporal da agenda;
   - identificar lote atual e lotes pendentes;
   - gerar duracoes por setor para enfileiramento.
3. Respeitar limite de simultaneos por lote (MAX_SETOR_SIMULTANEOS_AGENDA).
4. Respeitar intervalo entre lotes (INTERVALO_LOTE_AGENDA_MS).
5. Em conflito no mesmo setor, manter maior duracao.
6. Exclusao de agenda durante execucao cancela rotina automatica em andamento.
7. Se setor manual estiver aberto e chegar comando automatico, manter coerencia por maior deadline.

## 7. 🔌 Contrato entre modulos

## 7.1 🧩 Modulos envolvidos

- schedule_manager.h/.cpp
- runtime_config_manager.h/.cpp
- irrigation_controller.h/.cpp
- menu_controller.h/.cpp
- display_manager.h/.cpp
- web_ap_manager.h/.cpp

Responsabilidades principais:

1. schedule_manager: CRUD, validacao, persistencia e avaliacao de disparo.
2. runtime_config_manager: persistencia de timeout e duracao padrao.
3. irrigation_controller: acionamento de reles e deadlines.
4. menu_controller/display_manager: UX local no OLED com 4 botoes.
5. web_ap_manager: dashboard web e API para status/comandos.

## 7.2 🔄 Integracao com o loop atual

No loop principal:

1. Ler botoes e processar menu.
2. Atualizar irrigacao (timeouts/deadlines).
3. Avaliar disparos por minuto (RTC).
4. Enfileirar e executar lotes sequenciais.
5. Atualizar estado visual no display.
6. Processar servidor web.

## 8. 🖥️ UX da programacao no OLED (implementado)

Fluxo de programacao:

1. Selecionar slot de agenda (1..4).
2. Abrir submenu da agenda.
3. Opcoes do submenu:
   - editar hora
   - editar minuto
   - editar duracao
   - editar dias
   - editar setores
   - salvar
   - excluir
   - voltar

Interacoes:

1. Botao Cima/Baixo: altera valor do campo ou cursor.
2. Botao Selecionar (clique curto):
   - alterna dia (em EDIT_DIAS)
   - alterna setor (em EDIT_SETORES)
   - executa acao no submenu
3. Botao Voltar (ou clique longo no Selecionar): voltar/cancelar para etapa anterior conforme contexto.

## 9. 🧠 Maquina de estados da programacao (resumo)

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

## 10. ✅ Criterios de aceite da Fase 5 (revisados)

1. Usuario consegue criar/editar/excluir ate 4 agendas via 4 botoes.
2. Usuario consegue marcar/desmarcar dias e setores por agenda.
3. Agendas persistem apos desligar e ligar novamente.
4. Agendas disparam no horario correto com base no RTC.
5. Sistema evita duplicidade exata de agenda.
6. Execucao por lotes respeita simultaneidade e intervalo.
7. Nao ha regressao da irrigacao manual existente.
8. Dashboard web apresenta status e aplica comandos sem erro.

## 11. 🚧 Limites atuais

1. Modelo atual e global por slot (nao por setor).
2. Limite de 4 agendas totais.
3. Sem excecoes por data especifica (calendario).
4. Sem orquestracao avancada multi-zona alem da regra de lotes.

## 12. 🛣️ Plano de evolucao sugerido

1. Definir formalmente modelo final (global vs por setor) para proxima fase.
2. Se necessario, migrar estrutura para 16 agendas (4x4) com estrategia de compatibilidade.
3. Adicionar testes automatizados para motor de lotes e APIs web.
4. Consolidar criterios de UX para reduzir profundidade de navegacao no OLED.

---

Este contrato foi revisado para refletir o comportamento real do firmware no estado atual do projeto.