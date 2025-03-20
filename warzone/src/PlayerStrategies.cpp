#include "PlayerStrategies.h"
#include "Orders.h"
#include "Player.h"
#include <cctype>
#include <limits>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

static const int INITIAL_POOL_AMOUNT = 50;

// clang-format off

PlayerStrategy::PlayerStrategy(Map* map, Deck* deck, std::vector<Player*>* players)
    : map(map)
    , deck(deck)
    , players(players) { };

PlayerStrategy::PlayerStrategy() {}

PlayerStrategy::PlayerStrategy(const PlayerStrategy& other)
    : player(other.player)
    , map(other.map)
    , deck(other.deck)
    , players(other.players) { };

PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
    this->player = other.player;
    this->map = other.map;
    return *this;
}

std::ostream& operator<<(std::ostream& out, const PlayerStrategy& playerStrategy) {
    return out
        << playerStrategy.name()
        << " with "
        << *playerStrategy.player
        << " on "
        << *playerStrategy.map;
}

#define STRATEGY_BOILERPLATE(Type)                                                  \
    Type##Player::Type##Player(Map* map, Deck* deck, std::vector<Player*>* players) \
        : PlayerStrategy(map, deck, players) {};                                    \
                                                                                    \
    Type##Player::Type##Player()                                                    \
        : PlayerStrategy() {};                                                      \
                                                                                    \
    Type##Player::Type##Player(const Type##Player& other)                           \
        : PlayerStrategy(other) {};                                                 \
                                                                                    \
    PlayerStrategy* Type##Player::clone() const {                                   \
        return new Type##Player(*this);                                             \
    }                                                                               \
                                                                                    \
    std::string Type##Player::name() const {                                        \
        return #Type;                                                               \
    }                                                                               \
    Type##Player& Type##Player::operator=(const Type##Player & other) {             \
        PlayerStrategy::operator=(other);                                           \
        return *this;                                                               \
    }

// clang-format on

STRATEGY_BOILERPLATE(Human)
STRATEGY_BOILERPLATE(Aggressive)
STRATEGY_BOILERPLATE(Benevolent)
STRATEGY_BOILERPLATE(Neutral)
STRATEGY_BOILERPLATE(Cheater)

#undef STRATEGY_BOILERPLATE

static std::string readLine() {
    while (true) {
        std::flush(std::cout);
        std::cin.clear();
        std::cin.sync();
        std::string line = "";
        std::getline(std::cin, line);
        if (line != "") {
            line = std::regex_replace(line, TRIM_WHITESPACE, "");
            return line;
        }
    }
}

static size_t readInt(size_t max) {
    while (true) {
        std::string line = readLine();
        try {
            size_t integer = std::stoi(line);
            if (integer > max) {
                std::cout << "Larger than pool: " << integer << std::endl;
                continue;
            }

            return integer;
        } catch (const std::invalid_argument& e) {
            std::cout << "Invalid number: " << line << std::endl;
        } catch (const std::out_of_range& e) {
            std::cout << "Number out of range: " << line << std::endl;
        }
    }
}

static bool readBool() {
    while (true) {
        std::string line = readLine();
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        if (line == "yes" || line == "yep" || line == "mhm" || line == "true" || line == "1") {
            return true;
        }
        if (line == "no" || line == "nope" || line == "nu uh" || line == "false" || line == "0") {
            return false;
        }
        std::cout << "Invalid boolean: " << line << std::endl;
    }
}

template <class T>
static T* readOption(const std::vector<T*>& source) {
    for (size_t i = 0; i < source.size(); i++) {
        std::cout
            << "    "
            << i
            << ". "
            << *source[i]
            << std::endl;
    }
    if (source.size() == 0) {
        return nullptr;
    }
    if (source.size() == 1) {
        std::cout << "Auto selected your only option: " << *source[0] << std::endl;
        return source[0];
    }
    while (true) {
        return source[readInt(source.size() - 1)];
    }
}

template <class T>
static std::vector<T*> readOptions(const std::vector<T*>& source) {
    std::cout << "Comma separated, no spaces" << std::endl;
    for (size_t i = 0; i < source.size(); i++) {
        std::cout
            << "    "
            << i
            << ". "
            << *source[i]
            << std::endl;
    }
    if (source.size() == 0) {
        return std::vector<T*> {};
    }
    while (true) {
    restart:
        std::vector<T*> result;

        std::stringstream input(readLine());
        std::string token;
        while (std::getline(input, token, ',')) {
            try {
                size_t index = std::stoi(token);
                if (index >= source.size()) {
                    std::cout << "Index out of range: " << index << std::endl;
                    goto restart;
                }
                result.push_back(source[index]);
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid number: " << token << std::endl;
                goto restart;
            } catch (const std::out_of_range& e) {
                std::cout << "Number out of range: " << token << std::endl;
                goto restart;
            }
        }

        return result;
    }
}

static std::vector<Territory*> adjacentEnemyTerritories(Player* player) {
    std::set<Territory*> adjacent_set;
    for (auto& t : player->getOwnedTerritories()) {
        for (auto& a : t->getAdjacent()) {
            if (a->getOwner() != player) {
                adjacent_set.emplace(a);
            }
        }
    }
    std::vector<Territory*> adjacent_vec;
    adjacent_vec.assign(adjacent_set.begin(), adjacent_set.end());

    return adjacent_vec;
}

static int randomInt(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    return std::uniform_int_distribution<>(min, max)(gen);
}

void HumanPlayer::issueOrder() {
    // Defend
    for (auto t : this->player->toDefend()) {
        std::cout
            << "=== Your pool is " << this->player->getPool()
            << std::endl
            << "=== Enter how many armies to defend with:" << std::endl;
        std::cout << *t << std::endl;

        this->player->getOrders().add(new DeployOrder(
            this->player,
            t,
            readInt(this->player->getPool())));
    }

    // Attack
    for (auto t : this->player->toAttack()) {
        std::cout
            << "=== Attacking " << *t
            << std::endl;
        std::vector<Territory*> adjacent;
        for (auto p : this->player->getOwnedTerritories()) {
            const auto& adj = t->getAdjacent();
            auto it = std::find(adj.begin(), adj.end(), p);
            if (it != adj.end()) {
                adjacent.push_back(p);
            }
        }

        Territory* source = readOption(adjacent);

        std::cout << "=== Enter how many armies to attack with:" << std::endl;
        this->player->getOrders().add(new AdvanceOrder(
            this->player,
            source,
            t,
            readInt(std::numeric_limits<size_t>::max()),
            this->deck));
    }

    // Cards
    Card* card = this->player->getHand()->draw();
    if (card) {
        std::cout << "=== Do you want to play your card?: " << *card << std::endl;
        if (readBool()) {
            switch (card->getType()) {
            case CardType::BOMB: {
                std::cout << "Select a territory to bomb: " << std::endl;
                Territory* target = readOption(adjacentEnemyTerritories(this->player));
                this->player->getOrders().add(new BombOrder(this->player, target));
            } break;
            case CardType::REINFORCEMENT:
                std::cout << this->player->getName() << " played a Reinforcement card and has added 5 more units to their pool!" << std::endl;
                this->player->addReinforcementToPool(5);
                break;
            case CardType::BLOCKADE: {
                std::cout << "Select a territory to blockade: " << std::endl;
                Territory* target = readOption(this->player->getOwnedTerritories());
                this->player->getOrders().add(new BlockadeOrder(this->player, target));
            } break;
            case CardType::AIRLIFT: {
                std::cout << "Select the source territory to airlift from: " << std::endl;
                Territory* source = readOption(this->player->getOwnedTerritories());
                std::cout << "Select the target territory to airlift to: " << std::endl;
                Territory* target = readOption(this->player->getOwnedTerritories());
                std::cout << "Select the amount of armies" << std::endl;
                int amount = readInt(std::numeric_limits<int>::max());
                this->player->getOrders().add(new AirliftOrder(this->player, source, target, amount));
            } break;
            case CardType::DIPLOMACY: {
                std::vector<Player*> otherPlayers;
                for (auto p : *this->players) {
                    if (p != this->player) {
                        otherPlayers.push_back(p);
                    }
                }
                Player* target = readOption(otherPlayers);
                this->player->getOrders().add(new NegotiateOrder(this->player, target));
            } break;
            }
        } else {
            this->player->getHand()->addCard(card);
        }
    }
}

std::vector<Territory*> HumanPlayer::toDefend() {
    std::cout
        << "=== Enter territories to defend";
    return readOptions(this->player->getOwnedTerritories());
}

std::vector<Territory*> HumanPlayer::toAttack() {
    std::cout
        << "=== Enter territories to attack"
        << std::endl;

    return readOptions(adjacentEnemyTerritories(this->player));
}

void AggressivePlayer::issueOrder() {
    // Defend
    for (auto t : this->player->toDefend()) {
        if (player->getPool() <= 0)
            return;

        int unitsToDeploy = randomInt(1, player->getPool());

        this->player->getOrders().add(new DeployOrder(
            this->player,
            t,
            unitsToDeploy));
    }
    // Attack
    for (auto t : this->player->toAttack()) {
        std::cout
            << "=== Attacking " << *t
            << std::endl;
        std::vector<Territory*> adjacent;
        for (auto p : this->player->getOwnedTerritories()) {
            const auto& adj = t->getAdjacent();
            auto it = std::find(adj.begin(), adj.end(), p);
            if (it != adj.end()) {
                adjacent.push_back(p);
            }
        }

        Territory* source = adjacent[0];
        if (adjacent.size() > 1) {
            int randomIndex = randomInt(0, adjacent.size() - 1);

            source = adjacent[randomIndex];
        }

        int unitsToDeploy = randomInt(1, player->getPool());

        this->player->getOrders().add(new AdvanceOrder(
            this->player,
            source,
            t,
            unitsToDeploy,
            this->deck));
    }

    Card* card = this->player->getHand()->draw();
    if (card) {
        std::srand(std::time(0));
        int willPlayCard = std::rand() % 2;

        // Leave it to chance to decide if they will use the card or keep it
        if (willPlayCard) {
            switch (card->getType()) {
            case CardType::BOMB: {
                auto territoriesToAttack = this->player->toAttack();
                if (territoriesToAttack.empty()) {
                    std::cout << this->player->getName() << " tried to play a Bomb card, but there's no territories to attack!" << std::endl;
                    return;
                }

                // Choose the last territory from the list to attack
                Territory* target = territoriesToAttack[territoriesToAttack.size() - 1];
                this->player->getOrders().add(new BombOrder(this->player, target));
            } break;
            case CardType::REINFORCEMENT:
                std::cout << this->player->getName() << " played a Reinforcement card and has added 5 more units to their pool!" << std::endl;
                this->player->addReinforcementToPool(5);
                break;
            case CardType::BLOCKADE: {
                auto territoriesToDefend = this->player->toDefend();

                // Pick the territory surrounded by the most enemies
                Territory* target = territoriesToDefend[0];
                int enemyCount = 0;
                for (auto t : territoriesToDefend) {
                    int enemies = 0;
                    for (auto adjTerritory : t->getAdjacent()) {
                        if (adjTerritory->getOwner() != player)
                            enemies++;
                    }

                    if (enemies > enemyCount) {
                        enemyCount = enemies;
                        target = t;
                    }
                }

                this->player->getOrders().add(new BlockadeOrder(this->player, target));
            } break;
            case CardType::AIRLIFT: {
                auto territoriesToDefend = this->player->toDefend();
                if (territoriesToDefend.size() < 2) {
                    std::cout << this->player->getName() << " tried to play an Airlift card, but doesn't have enough territories to do so!" << std::endl;
                    return;
                }

                // Airlift the armies from the strongest territory to the territory with the most enemies surrounding it
                Territory* source = territoriesToDefend[0];
                Territory* target = territoriesToDefend[1];
                int enemyCount = 0;
                for (auto t : territoriesToDefend) {
                    if (t == source)
                        continue;

                    int enemies = 0;
                    for (auto adjTerritory : t->getAdjacent()) {
                        if (adjTerritory->getOwner() != player)
                            enemies++;
                    }
                    if (enemies > enemyCount) {
                        enemyCount = enemies;
                        target = t;
                    }
                }

                // Leave at least one unit behind
                this->player->getOrders().add(new AirliftOrder(this->player, source, target, source->getArmies() - 1));
            } break;
            case CardType::DIPLOMACY: {
                // Make friends with the player with the most territories
                Player* friendly = nullptr;
                for (size_t i = 0; i < players->size(); i++) {
                    Player* p = (*players)[i];
                    if (p == this->player)
                        continue;
                    if (this->player->isFriendsWith(p))
                        continue;

                    if (!friendly || p->getOwnedTerritories().size() > friendly->getOwnedTerritories().size()) {
                        friendly = p;
                    }
                }
                if (friendly) {
                    this->player->getOrders().add(new NegotiateOrder(this->player, friendly));
                }
            } break;
            }
        } else {
            this->player->getHand()->addCard(card);
        }
    }
}

std::vector<Territory*> AggressivePlayer::toDefend() {
    std::vector<std::tuple<Territory*, int>> territoriesWithEnemies;

    // Aggressive players prioritize defending territories connected to enemies
    for (Territory* territory : player->getOwnedTerritories()) {
        int enemyTerritories = 0;
        std::vector<Territory*> adjacentTerritories = territory->getAdjacent();
        for (auto adjTerritory : adjacentTerritories) {
            Player* territoryOwner = adjTerritory->getOwner();
            if (territoryOwner != player && territoryOwner != nullptr && !player->isFriendsWith(territoryOwner))
                enemyTerritories++;
        }
        // Add the territory and how many enemy territories surround it
        territoriesWithEnemies.emplace_back(territory, enemyTerritories);
    }

    // Sort the territories based on how many enemy territories surround it
    // Prioritize the most armies first
    std::sort(territoriesWithEnemies.begin(), territoriesWithEnemies.end(), [](auto const tup1, auto const tup2) {
        return std::get<1>(tup1) < std::get<1>(tup2);
    });

    std::vector<Territory*> territories;
    for (auto territory : territoriesWithEnemies) {
        // Get the territory back from the tuple
        territories.push_back(std::get<0>(territory));
    }

    return territories;
}

std::vector<Territory*> AggressivePlayer::toAttack() {
    std::vector<Territory*> territories;

    // Aggressive players prioritize attacking territories with fewer enemies
    for (Territory* territory : player->getOwnedTerritories()) {
        std::vector<Territory*> adjacentTerritories = territory->getAdjacent();
        for (auto adjTerritory : adjacentTerritories) {
            Player* territoryOwner = adjTerritory->getOwner();
            if (territoryOwner != player && territoryOwner != nullptr && !player->isFriendsWith(territoryOwner))
                territories.push_back(adjTerritory);
        }
    }

    // Sort by least amount of enemies (Easier targets)
    std::sort(territories.begin(), territories.end(), [](const Territory* territory1, const Territory* territory2) {
        return territory1->getArmies() < territory2->getArmies();
    });

    return territories;
}

void BenevolentPlayer::issueOrder() {
    for (auto t : this->player->toDefend()) {
        if (player->getPool() <= 0)
            return;

        int unitsToDeploy = randomInt(1, player->getPool());

        this->player->getOrders().add(new DeployOrder(
            this->player,
            t,
            unitsToDeploy));
    }

    Card* card = this->player->getHand()->draw();
    if (card) {
        int willPlayCard = randomInt(0, 1);

        if (willPlayCard) {
            switch (card->getType()) {

            case CardType::BOMB:
                break;

            case CardType::BLOCKADE:
                break;

            case CardType::REINFORCEMENT: {
                std::cout << this->player->getName() << " played a Reinforcement card and has added 5 more units to their pool!" << std::endl;
                this->player->addReinforcementToPool(5);
            } break;

            case CardType::AIRLIFT: {
                auto territoriesToDefend = this->player->toDefend();
                if (territoriesToDefend.size() < 2) {
                    std::cout << this->player->getName() << " tried to play an Airlift card, but doesn't have enough territories to do so!" << std::endl;
                    return;
                }

                Territory* source = territoriesToDefend[0];
                Territory* target = territoriesToDefend[1];
                int enemyCount = 0;
                for (auto t : territoriesToDefend) {
                    if (t == source)
                        continue;

                    int enemies = 0;
                    for (auto adjTerritory : t->getAdjacent()) {
                        if (adjTerritory->getOwner() != player)
                            enemies++;
                    }
                    if (enemies > enemyCount) {
                        enemyCount = enemies;
                        target = t;
                    }
                }

                this->player->getOrders().add(new AirliftOrder(this->player, source, target, source->getArmies() - 1));

            } break;

            case CardType::DIPLOMACY: {
                Player* friendly = nullptr;
                for (size_t i = 0; i < players->size(); i++) {
                    Player* p = (*players)[i];
                    if (p == this->player)
                        continue;
                    if (this->player->isFriendsWith(p))
                        continue;

                    if (!friendly || p->getOwnedTerritories().size() > friendly->getOwnedTerritories().size()) {
                        friendly = p;
                    }
                }
                if (friendly) {
                    this->player->getOrders().add(new NegotiateOrder(this->player, friendly));
                }
            } break;
            }
        } else {
            this->player->getHand()->addCard(card);
        }
    }
}

std::vector<Territory*> BenevolentPlayer::toDefend() {
    std::vector<std::tuple<Territory*, int>> territoriesWithEnemies;

    for (Territory* territory : player->getOwnedTerritories()) {
        int enemyTerritories = 0;
        std::vector<Territory*> adjacentTerritories = territory->getAdjacent();
        for (auto adjTerritory : adjacentTerritories) {
            Player* territoryOwner = adjTerritory->getOwner();
            if (territoryOwner != player && territoryOwner != nullptr && !player->isFriendsWith(territoryOwner))
                enemyTerritories++;
        }
        territoriesWithEnemies.emplace_back(territory, enemyTerritories);
    }

    std::sort(territoriesWithEnemies.begin(), territoriesWithEnemies.end(), [](auto const tup1, auto const tup2) {
        return std::get<1>(tup1) < std::get<1>(tup2);
    });

    std::vector<Territory*> territories;
    for (auto territory : territoriesWithEnemies) {
        territories.push_back(std::get<0>(territory));
    }

    return territories;
}

std::vector<Territory*> BenevolentPlayer::toAttack() {
    return {};
}

/// @brief Makes the Neutral Player issue an order. Changes them to an Aggressive Player if attacked
void NeutralPlayer::issueOrder() {
    // Neutral players don't actually issue orders, but become aggressive when attacked
    if (this->player->getPool() != INITIAL_POOL_AMOUNT) {
        std::cout << this->player->getName() << " got attacked! They are now aggressive!" << std::endl;
        this->player->setStrategy(new AggressivePlayer(this->map, this->deck, this->players));
        this->player->issueOrder();
    } else {
        std::cout << this->player->getName() << " does nothing!" << std::endl;
    }
}

/// @brief Gets the territories a Neutral Player can defend. By definition, they don't defend
/// @return Nothing, this shouldn't do anything
std::vector<Territory*> NeutralPlayer::toDefend() {
    return {};
}

/// @brief Gets the territories a Neutral Player can attack. By definition, they don't attack
/// @return Nothing, this shouldn't do anything
std::vector<Territory*> NeutralPlayer::toAttack() {
    return {};
}

void CheaterPlayer::issueOrder() {
    for (auto& t : this->player->toDefend()) {
        t->setArmies(9999999);
    }

    for (auto& t : this->player->toAttack()) {
        for (auto& p : this->player->getOwnedTerritories()) {
            const auto& adj = t->getAdjacent();
            auto it = std::find(adj.begin(), adj.end(), p);
            if (it != adj.end()) {
                this->player->getOrders().add(new AdvanceOrder(this->player, p, t, 999, nullptr));
            }
        }
    }
}

std::vector<Territory*> CheaterPlayer::toDefend() {
    return this->player->getOwnedTerritories();
}

std::vector<Territory*> CheaterPlayer::toAttack() {
    return adjacentEnemyTerritories(this->player);
}
