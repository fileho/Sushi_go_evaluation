#include "card.h"


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