#ifndef CARD_H
#define CARD_H


#include <string>

enum class card_type { tempura, chopsticks, maki_roll_1, maki_roll_2, maki_roll_3, wasabi, sashimi, pudding, nigiri_1, nigiri_2, nigiri_3, dumplings };

class card_list
{
public:
    int tempura{};
    int maki_roll_1{};
    int maki_roll_2{};
    int maki_roll_3{};
    int sashimi{};
    int chopsticks{};
    int pudding{};
    int nigiri_1{};
    int nigiri_2{};
    int nigiri_3{};
    int wasabi{};
    int dumplings{};
    int wasabi_nigiri_1{};
    int wasabi_nigiri_2{};
    int wasabi_nigiri_3{};

    std::string print() const;
};

class base_card
{
public:
    virtual ~base_card() = default;
    virtual void play(card_list& cards) = 0;
    virtual unsigned MCTS() const = 0;
    virtual card_type get_card() const = 0;
};

class tempura : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class chopsticks : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class maki_roll_1 : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class maki_roll_2 : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class maki_roll_3 : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class wasabi : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class sashimi : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class pudding : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class nigiri_1 : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class nigiri_2 : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class nigiri_3 : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};

class dumplings : public base_card
{
public:
    void play(card_list& cards) override;
    unsigned MCTS() const override;
    card_type get_card() const override;
};


#endif // !CARD_H