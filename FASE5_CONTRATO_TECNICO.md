# Fase 5 - Contrato Tecnico de Agendamento

Data: 2026-04-10
Status: aprovado para implementacao

## 1. Decisoes de produto fechadas

1. D1: multiplos agendamentos por setor.
2. D2: agendamento semanal por dias da semana (nao apenas diario).
3. D3: persistencia no ESP32 (flash/NVS). RTC DS3231 apenas para data/hora.
4. D4: limite de 4 agendamentos por setor.
5. D5: edicao via encoder com campos sequenciais: hora, minuto, duracao e dias.

## 2. Escopo da Fase 5

Implementar agendamento automatico semanal para ate 4 setores, com ate 4 agendas por setor.

Total maximo de agendas no sistema:
- 4 setores x 4 agendas = 16 agendas.

## 3. Modelo de dados (contrato)

### 3.1 Estrutura de uma agenda

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
    bool ativa;             // slot em uso
    uint8_t hora;           // 0..23
    uint8_t minuto;         // 0..59
    uint16_t duracaoMin;    // 1..999 (pode limitar em UI)
    uint8_t diasMask;       // bits DOM..SAB
};
```

### 3.2 Estrutura global de agenda

```cpp
constexpr uint8_t MAX_SETORES = 4;
constexpr uint8_t MAX_AGENDAS_POR_SETOR = 4;

struct BancoAgendas {
    uint16_t versao;   // controle de compatibilidade
    uint16_t crc;      // integridade
    AgendaSetor agendas[MAX_SETORES][MAX_AGENDAS_POR_SETOR];
};
```

## 4. Regras funcionais obrigatorias

1. Uma agenda e valida somente se tiver pelo menos 1 dia marcado.
2. Duracao minima: 1 minuto.
3. Horario valido: 00:00 ate 23:59.
4. Nao permitir duplicidade exata dentro do mesmo setor:
   hora igual + minuto igual + diasMask igual.
5. Ao atingir 4 agendas no setor, bloquear criacao de nova agenda.
6. Em reboot, recarregar agendas persistidas automaticamente.
7. Se dados persistidos estiverem invalidos (CRC/versao), resetar para padrao seguro (tudo inativo).

## 5. Politica de persistencia

Meio escolhido: ESP32 (NVS/Preferences ou EEPROM emulada em flash).

Diretrizes:
1. Persistir apenas quando houver alteracao (salvar/editar/excluir), nao a cada loop.
2. Escrever bloco completo do BancoAgendas com versao e crc.
3. Leitura no boot:
   - se versao e crc ok: carregar banco;
   - se invalido: inicializar banco vazio e salvar.
4. DS3231 nao guarda agenda; DS3231 apenas fornece horario confiavel.

## 6. Motor de execucao de agendas

Checagem no loop principal com gatilho por minuto.

Regras:
1. Rodar verificacao quando o minuto mudar (evitar disparos repetidos no mesmo minuto).
2. Para cada setor e cada agenda ativa:
   - validar se dia atual esta marcado em diasMask;
   - validar se hora/minuto atuais batem;
   - se bater, solicitar abertura do setor por duracaoMin.
3. Em conflito de multiplas agendas do mesmo setor no mesmo minuto:
   - aplicar maior duracao resultante (comportamento monotono e seguro).
4. Se setor estiver ligado manualmente e entrar agenda automatica:
   - manter ligado e atualizar termino para o maior tempo restante.

## 7. Contrato entre modulos

## 7.1 Novo modulo recomendado

Criar modulo dedicado de agenda, por exemplo:
- schedule_manager.h
- schedule_manager.cpp

Responsabilidades:
1. CRUD de agendas por setor/slot.
2. Validacoes.
3. Persistencia.
4. Avaliacao de disparo por horario atual.

## 7.2 Integracao com o loop atual

No loop principal:
1. Ler horario atual do RTC.
2. Chamar scheduleManager.atualizar(agora).
3. Se houver eventos de abertura, delegar ao IrrigationController.
4. Manter funcionamento da irrigacao manual sem regressao.

## 7.3 Integracao com menu/display

Na tela PROGRAMAR:
1. Escolher setor (S1..S4).
2. Listar agendas do setor (1..4).
3. Acoes por agenda: criar/editar/excluir/ativar.
4. Entrar em tela de edicao descrita no item 8.

## 8. UX da tela de edicao no OLED (contrato de navegacao)

Cabecalho:
- "PROGRAMAR Sx"

Corpo:
- "Agenda N de 4"
- Campo HORA (ativo = barra)
- Campo MINUTO
- Campo DURACAO
- Linha de dias:
  DOM SEG TER QUA QUI SEX SAB
  [ ] [X] [ ] [X] [ ] [X] [ ]

Interacao com encoder:
1. Giro em HORA/MINUTO/DURACAO: ajusta valor do campo ativo.
2. Clique curto: avanca para proximo campo.
3. Em DIAS:
   - giro: move cursor entre DOM..SAB;
   - clique curto: marca/desmarca dia no cursor.
4. Confirmacao final:
   - clique longo em DIAS: valida, salva e sai.

Comportamento visual:
1. Campo ativo em destaque forte (barra grande).
2. Campos inativos em texto simples.
3. No modo DIAS, destacar o dia sob cursor.

## 9. Maquina de estados da programacao

```text
PROGRAMAR_HOME
  -> selecionar setor
  -> selecionar agenda
  -> EDIT_HORA

EDIT_HORA -> (click) EDIT_MINUTO
EDIT_MINUTO -> (click) EDIT_DURACAO
EDIT_DURACAO -> (click) EDIT_DIAS
EDIT_DIAS -> (long click confirmar) VALIDAR_E_SALVAR -> PROGRAMAR_HOME

(cancelar por long click fora de DIAS, opcional na primeira entrega)
```

## 10. Criterios de aceite da Fase 5

1. Usuario consegue criar ate 4 agendas por setor via encoder.
2. Usuario consegue marcar/desmarcar dias individuais DOM..SAB.
3. Agendas persistem apos desligar e ligar novamente.
4. Agendas disparam no horario correto com base no RTC.
5. Sistema evita duplicidade exata por setor.
6. Nao ha regressao da irrigacao manual ja existente.
7. Navegacao no OLED permanece fluida e legivel.

## 11. Limites da primeira entrega

Para reduzir risco na Fase 5 inicial:
1. Sem copia de agenda entre setores.
2. Sem excecoes por data especifica.
3. Sem sobreposicao avancada entre setores (cada setor independente).
4. Sem interface de calendario mensal.

## 12. Plano de implementacao sugerido

1. Criar ScheduleManager (dados + validacao + persistencia).
2. Integrar motor de disparo no loop.
3. Implementar menu PROGRAMAR: selecionar setor/agenda.
4. Implementar tela EDIT_HORA/MINUTO/DURACAO/DIAS.
5. Testar cenarios:
   - reboot com dados validos;
   - reboot com dados invalidos;
   - disparo em dia marcado;
   - nao disparo em dia nao marcado;
   - limite de 4 agendas.
