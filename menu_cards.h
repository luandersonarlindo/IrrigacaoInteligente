#pragma once

// Lógica pura do layout de cards do Menu Principal (seleção + próximo item
// com wrap circular). Sem dependência de hardware — testável em host.
namespace MenuCards
{

// Retorna o índice do item a mostrar na linha inferior (dimmed), dado o
// índice atualmente selecionado e o total de itens do menu. Faz wrap
// circular: se `atual` for o último item, retorna 0.
int proximoIndice(int atual, int total);

} // namespace MenuCards
