#pragma once

#include <ostream>
#include <tuple>
#include <vector>

#include "Cards.h"
#include "Map.h"
#include "Player.fwd.h"

class PlayerStrategy {
protected:
    Player* player;
    Map* map;
    Deck* deck;
    std::vector<Player*>* players;

public:
    PlayerStrategy(Map* map, Deck* deck, std::vector<Player*>* players);
    PlayerStrategy();
    PlayerStrategy(const PlayerStrategy& other);
    virtual ~PlayerStrategy() = default;

    virtual PlayerStrategy* clone() const = 0;
    virtual std::string name() const = 0;
    virtual void issueOrder() = 0;
    virtual std::vector<Territory*> toDefend() = 0;
    virtual std::vector<Territory*> toAttack() = 0;

    PlayerStrategy& operator=(const PlayerStrategy& other);
    friend std::ostream& operator<<(std::ostream& out, const PlayerStrategy& playerStrategy);
    friend class Player;
};

#define STRATEGY_BOILERPLATE(Type)                                         \
    class Type##Player : public PlayerStrategy {                           \
    public:                                                                \
        Type##Player(Map* map, Deck* deck, std::vector<Player*>* players); \
        Type##Player();                                                    \
        Type##Player(const Type##Player& other);                           \
        ~Type##Player() override = default;                                \
                                                                           \
        PlayerStrategy* clone() const override;                            \
        std::string name() const override;                                 \
        void issueOrder() override;                                        \
        std::vector<Territory*> toDefend() override;                       \
        std::vector<Territory*> toAttack() override;                       \
                                                                           \
        Type##Player& operator=(const Type##Player& other);                \
    };

STRATEGY_BOILERPLATE(Human)
STRATEGY_BOILERPLATE(Aggressive)
STRATEGY_BOILERPLATE(Benevolent)
STRATEGY_BOILERPLATE(Neutral)
STRATEGY_BOILERPLATE(Cheater)

#undef STRATEGY_BOILERPLATE
