#pragma once

#include "Cards.h"
#include "Map.h"
#include "Orders.h"
#include "Player.fwd.h"
#include "PlayerStrategies.h"
#include <ostream>
#include <vector>

/**
 * @class Player
 *
 * @brief Player in the Warzone game.
 */
class Player {
private:
    /** @brief Name of the player. */
    std::string name;
    /** @brief Collection of territories owned by the player. */
    std::vector<Territory*> territories;
    /** @brief Collection of cards in the player's hand. */
    Hand* cards;
    /** @brief the number of reinforcements the player has in his pool(reinforcements not in territories) */
    int pool;
    /** @brief Collection of player that are "friends" for this round and cannot be attacked. */
    std::vector<Player*> friends;

    OrdersList* orders;

    PlayerStrategy* strategy;

public:
    /**
     * @brief Default constructor for the Player class.
     */
    Player(std::string name, PlayerStrategy* strategy);

    /**
     * @brief Default constructor for the Player class.
     */
    Player(std::string name, Map* map, Deck* deck, std::vector<Player*>& otherPlayers);
    /**
     * @brief Copy constructor for the Player class.
     *
     * @param other Player object to copy from.
     */
    Player(const Player& other);
    /**
     * @brief Destructor for the Player class.
     */
    ~Player();

    /**
     * @brief Get a list of territories to defend.
     *
     * This method returns an arbitrary empty value.
     * It will be implemented later in the assignment.
     *
     * @return A vector of pointers to the territories to defend.
     */
    std::vector<Territory*> toDefend();
    /**
     * @brief Get a list of territories to attack.
     *
     * @return A vector of pointers to the territories to attack.
     */
    std::vector<Territory*> toAttack();
    /**
     * @brief Issue an order for the current turn.
     *
     * This method creates an order and places it in the player's list of orders to be executed in the current turn.
     *
     * This method adds an arbitrary order to the list.
     * It will be implemented later in the assignment.
     */
    void issueOrder();
    Order* getNextOrder();

    /**
     * @brief Get name.
     */
    std::string getName() const;
    OrdersList& getOrders();
    const std::vector<Player*>& getFriends() const;
    size_t getTerritories();
    const std::vector<Territory*>& getOwnedTerritories() const;
    int getPool();
    Hand* getHand();
    void addTerritory(Territory* territory);
    void addReinforcementToPool(int i);
    void addCardToHand(Card* card);

    bool hasReinforcementsInPool() const { return pool > 0; }
    bool wantsToIssueOrder() const;
    bool hasTerritories() const { return !territories.empty(); }

    void removeArmiesFromPool(int i);
    void removeTerritory(Territory* territory);
    void addFriend(Player* player);
    void clearFriends();
    bool isFriendsWith(Player* player);

    void setStrategy(PlayerStrategy* strategy);
    void initStrategy(Map* map, Deck* deck, std::vector<Player*>* players);

    void observer(Observer* observer);
    Player& operator=(const Player& other);
    friend std::ostream& operator<<(std::ostream& out, const Player& player);
};
