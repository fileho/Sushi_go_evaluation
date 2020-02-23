#include "card.h"

std::string card_list::print() const
{
    std::string ret{};
    if (tempura)
        ret += "tempura: " + std::to_string(tempura) + "\n";
    if (maki_roll_1)
        ret += "maki roll 1: " + std::to_string(maki_roll_1) + "\n";
    if (maki_roll_2)
        ret += "maki roll 2: " + std::to_string(maki_roll_2) + "\n";
    if (maki_roll_3)
        ret += "maki roll 3: " + std::to_string(maki_roll_3) + "\n";
    if (sashimi)
        ret += "sashimi: " + std::to_string(sashimi) + "\n";
    if (chopsticks)
        ret += "chopsticks: " + std::to_string(chopsticks) + "\n";
    if (pudding)
        ret += "pudding: " + std::to_string(pudding) + "\n";
    if (nigiri_1)
        ret += "nigiri 1: " + std::to_string(nigiri_1) + "\n";
    if (nigiri_2)
        ret += "nigiri 2: " + std::to_string(nigiri_2) + "\n";
    if (nigiri_3)
        ret += "nigiri 3: " + std::to_string(nigiri_3) + "\n";
    if (wasabi)
        ret += "wasabi: " + std::to_string(wasabi) + "\n";
    if (dumplings)
        ret += "dumplings: " + std::to_string(dumplings) + "\n";
    if (wasabi_nigiri_1)
        ret += "wasabi & nigiri 1: " + std::to_string(wasabi_nigiri_1) + "\n";
    if (wasabi_nigiri_2)
        ret += "wasabi & nigiri 2: " + std::to_string(wasabi_nigiri_2) + "\n";
    if (wasabi_nigiri_3)
        ret += "wasabi & nigiri 3: " + std::to_string(wasabi_nigiri_3) + "\n";

    return ret;
}


void tempura::play(card_list& cards)
{
    ++cards.tempura;
}

card_type tempura::get_card() const
{
    return card_type::tempura;
}


void chopsticks::play(card_list& cards)
{
    ++cards.chopsticks;
}

card_type chopsticks::get_card() const
{
    return card_type::chopsticks;
}


void maki_roll_1::play(card_list& cards)
{
    ++cards.maki_roll_1;
}

card_type maki_roll_1::get_card() const
{
    return card_type::maki_roll_1;
}


void maki_roll_2::play(card_list& cards)
{
    ++cards.maki_roll_2;
}

card_type maki_roll_2::get_card() const
{
    return card_type::maki_roll_2;
}


void maki_roll_3::play(card_list& cards)
{
    ++cards.maki_roll_3;
}

card_type maki_roll_3::get_card() const
{
    return card_type::maki_roll_3;
}


void wasabi::play(card_list& cards)
{
    ++cards.wasabi;
}

card_type wasabi::get_card() const
{
    return card_type::wasabi;
}


void sashimi::play(card_list& cards)
{
    ++cards.sashimi;
}

card_type sashimi::get_card() const
{
    return card_type::sashimi;
}


void pudding::play(card_list& cards)
{
    ++cards.pudding;
}

card_type pudding::get_card() const
{
    return card_type::pudding;
}


void nigiri_1::play(card_list& cards)
{
    if (cards.wasabi)
    {
        --cards.wasabi;
        ++cards.wasabi_nigiri_1;
        return;
    }
    ++cards.nigiri_1;
}

card_type nigiri_1::get_card() const
{
    return card_type::nigiri_1;
}


void nigiri_2::play(card_list& cards)
{
    if (cards.wasabi)
    {
        --cards.wasabi;
        ++cards.wasabi_nigiri_2;
        return;
    }
    ++cards.nigiri_2;
}

card_type nigiri_2::get_card() const
{
    return card_type::nigiri_2;
}


void nigiri_3::play(card_list& cards)
{
    if (cards.wasabi)
    {
        --cards.wasabi;
        ++cards.wasabi_nigiri_3;
        return;
    }
    ++cards.nigiri_3;
}

card_type nigiri_3::get_card() const
{
    return card_type::nigiri_3;
}


void dumplings::play(card_list& cards)
{
    ++cards.dumplings;
}

card_type dumplings::get_card() const
{
    return card_type::dumplings;
}