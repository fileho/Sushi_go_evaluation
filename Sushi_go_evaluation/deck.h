#ifndef DECK_H
#define DECK_H

#include "card.h"

#include <vector>
#include <memory>

class deck
{
public:
    deck();
    std::unique_ptr<base_card> draw();
private:
    std::vector<std::unique_ptr<base_card>> cards{};
};


#endif // !DECK_H