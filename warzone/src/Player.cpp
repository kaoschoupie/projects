#include "Player.h"
#include "Orders.h"
#include "PlayerStrategies.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>

Player::Player(std::string name, PlayerStrategy* strategy)
    : name(name)
    , cards(new Hand())
    , pool(0)
    , orders(new OrdersList())
    , strategy(strategy) {
    strategy->player = this;
}

Player::Player(std::string name, Map* map, Deck* deck, std::vector<Player*>& otherPlayers)
    : Player(name, new HumanPlayer(map, deck, &otherPlayers)) { }

Player::Player(const Player& other) {
    for (auto t : other.territories) {
        // Map handles the territories, no need to copy
        territories.push_back(t);
    }
    for (auto f : other.friends) {
        // Friends should point to the same object, no need to copy
        friends.push_back(f);
    }
    this->name = other.name;
    this->cards = new Hand(*other.cards);
    this->pool = other.pool;
    this->orders = new OrdersList(*other.orders);
    this->strategy = other.strategy->clone();
    this->strategy->player = this;
}

// Destructor
Player::~Player() {
    // Map handles the territories, no need to clean them
    // Player owns cards, we need to clean
    // Hand destructor is called automatically
    delete cards;
    delete orders;
    delete strategy;
}

std::vector<Territory*> Player::toDefend() {
    return this->strategy->toDefend();
}

std::vector<Territory*> Player::toAttack() {
    return this->strategy->toAttack();
}

void Player::issueOrder() {
    return this->strategy->issueOrder();
}

Order* Player::getNextOrder() {
    return orders->remove(0);
}

bool Player::wantsToIssueOrder() const {
    return !territories.empty();
}

std::string Player::getName() const {
    return this->name;
}

OrdersList& Player::getOrders() {
    return *this->orders;
}

const std::vector<Player*>& Player::getFriends() const {
    return this->friends;
}

Hand* Player::getHand() {
    return this->cards;
}

size_t Player::getTerritories() {
    return this->territories.size();
}

const std::vector<Territory*>& Player::getOwnedTerritories() const {
    return this->territories;
}

int Player::getPool() {
    return this->pool;
}

void Player::addTerritory(Territory* territory) {
    auto it = std::find(territories.begin(), territories.end(), territory);
    if (it == territories.end()) {
        territories.push_back(territory);
        this->strategy->map->setTerritoryOwner(territory, this);
    }
}

void Player::removeTerritory(Territory* territory) {
    auto it = std::find(territories.begin(), territories.end(), territory);
    if (it != territories.end()) {
        territories.erase(it);
        this->strategy->map->setTerritoryOwner(territory, nullptr);
    }
}

void Player::addReinforcementToPool(int i) {
    this->pool += i;
}

void Player::removeArmiesFromPool(int i) {
    this->pool -= i;
    if (pool < 0) {
        throw std::runtime_error("ReinforcementPool became less than 1 somehow");
    }
}

void Player::addCardToHand(Card* card) {
    if (card) {
        cards->addCard(card);
    }
}

void Player::addFriend(Player* player) {
    if (!player) {
        return;
    }
    if (!this->isFriendsWith(player)) {
        return;
    }

    std::cout << "########" << std::endl;
    for (auto f : this->friends) {
        std::cout << *f << std::endl;
    }
    this->friends.push_back(player);
}

void Player::clearFriends() {
    this->friends.clear();
}

bool Player::isFriendsWith(Player* player) {
    auto it = std::find(friends.begin(), friends.end(), player);
    return it != friends.end();
}

void Player::setStrategy(PlayerStrategy* strategy) {
    this->strategy = strategy->clone();
    this->strategy->player = this;
}
void Player::initStrategy(Map* map, Deck* deck, std::vector<Player*>* players) {
    this->strategy->map = map;
    this->strategy->deck = deck;
    this->strategy->players = players;
}

Player& Player::operator=(const Player& other) {
    this->friends.clear();
    for (auto f : other.friends) {
        this->friends.push_back(f);
    }
    this->territories.clear();
    for (auto t : other.territories) {
        this->territories.push_back(t);
    }
    this->cards = { other.cards };
    this->name = other.name;
    this->pool = other.pool;

    return *this;
}

void Player::observer(Observer* observer) {
    this->orders->attach(observer);
    orders->observeAllOrders(observer);
}

std::ostream& operator<<(std::ostream& out, const Player& player) {
    if (!&player) {
        return out << "NULL";
    }
    return out << "Player " << player.name;
}
