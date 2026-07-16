#include "menu_cards.h"

namespace MenuCards
{

int proximoIndice(int atual, int total)
{
    if (total <= 1)
    {
        return atual;
    }
    return (atual + 1) % total;
}

} // namespace MenuCards
