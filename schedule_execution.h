#pragma once

#include <cstdint>

// Decisões puras do motor de execução sequencial por lotes, extraídas de
// IrrigacaoInteligente.ino, sem dependência de hardware (millis(), relés).
namespace ScheduleExecution
{

// Seleciona até `limiteSimultaneo` setores marcados como pendentes (em ordem
// de índice, 0..totalSetores-1) para entrar no lote atual. Os setores
// selecionados são marcados em `selecionados` e removidos de `pendentes`
// (efeito colateral nos dois arrays, do tamanho `totalSetores`).
// Retorna a quantidade de setores selecionados para este lote.
int selecionarLote(bool pendentes[], bool selecionados[], int totalSetores, int limiteSimultaneo);

// Equivalente a selecionarLote, mas operando sobre máscaras de bits: os
// setores selecionados são setados em `selecionados` (deve começar em 0) e
// removidos de `pendentes` (efeito colateral no parâmetro passado por
// referência). Retorna a quantidade de setores selecionados para este lote.
int selecionarLoteMask(uint16_t &pendentes, uint16_t &selecionados, int totalSetores, int limiteSimultaneo);

// Decide se a duração pendente de um setor deve ser atualizada para
// `duracaoNova`: verdadeiro se o setor ainda não está pendente, ou se a
// nova duração é maior que a pendente atual (mantém sempre a maior).
bool deveAtualizarDuracaoPendente(bool jaPendente, uint16_t duracaoAtual, uint16_t duracaoNova);

} // namespace ScheduleExecution
