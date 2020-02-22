#include "deck.h"

#include <random>
#include <ctime>

deck::deck()
{
    for (int i{}; i < 14; ++i)
        cards.emplace_back(std::make_unique<tempura>());

    for (int i{}; i < 4; ++i)
        cards.emplace_back(std::make_unique<chopsticks>());

    for (int i{}; i < 6; ++i)
        cards.emplace_back(std::make_unique<maki_roll_1>());

    for (int i{}; i < 12; ++i)
        cards.emplace_back(std::make_unique<maki_roll_2>());

    for (int i{}; i < 8; ++i)
        cards.emplace_back(std::make_unique<maki_roll_3>());

    for (int i{}; i < 6; ++i)
        cards.emplace_back(std::make_unique<wasabi>());

    for (int i{}; i < 14; ++i)
        cards.emplace_back(std::make_unique<sashimi>());

    for (int i{}; i < 10; ++i)
        cards.emplace_back(std::make_unique<pudding>());

    for (int i{}; i < 5; ++i)
        cards.emplace_back(std::make_unique<nigiri_1>());

    for (int i{}; i < 10; ++i)
        cards.emplace_back(std::make_unique<nigiri_2>());

    for (int i{}; i < 5; ++i)
        cards.emplace_back(std::make_unique<nigiri_3>());

    for (int i{}; i < 14; ++i)
        cards.emplace_back(std::make_unique<dumplings>());

    std::random_device rd;
    std::mt19937 gen{ static_cast<unsigned int>(std::time(nullptr)) };
    
    std::shuffle(cards.begin(), cards.end(), gen);
}


std::unique_ptr<base_card> deck::draw()
{
    auto ret = std::move(cards[cards.size() - 1]);
    cards.pop_back();

    return ret;
}
