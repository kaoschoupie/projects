#include "Orders.h"
#include "LoggingObserver.h"
#include "Player.h"
#include <sstream>

/// @brief Base class for orders
/// @param name The name of the order
Order::Order(const std::string& name)
    : Subject()
    , ILoggable()
    , executed(false)
    , effect("No effect")
    , orderName(name) { }

/// @brief Copy constructor implementation for Order object
/// @param other Order object
Order::Order(const Order& other)
    : Order(other.orderName) {
    this->executed = other.executed;
    this->effect = other.effect;
}

/// @brief Order destructor
Order::~Order() { }

// Override for assignment operator for Order
Order& Order::operator=(const Order& other) {
    if (this != &other) {
        this->executed = other.executed;
        this->effect = other.effect;
        this->orderName = other.orderName;
    }
    return *this;
}

void Order::attach(Observer* observer) {
    this->attach(observer);
}

std::string Order::stringToLog() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

// Override for printing out Order objects
std::ostream& operator<<(std::ostream& out, const Order& order) {
    out << "\t- Type: " << order.orderName << std::endl;
    order.print(out);
    return out;
}

// Derived classes for specific order types
// and their validation logic (to be implemented in the next part)

/// @brief Order for Deploy action
/// @param player The player issuing the order
/// @param target The target territory
/// @param amount The amount of units
DeployOrder::DeployOrder(Player* player, Territory* target, int amount)
    : Order("Deploy")
    , player(player)
    , target(target)
    , amount(amount) { }

/// @brief Copy constructor implementation for DeployOrder object
/// @param other DeployOrder object
DeployOrder::DeployOrder(const DeployOrder& other)
    : Order(other)
    , player(other.player)
    , target(other.target)
    , amount(other.amount) { }

/// @brief Validates the Deploy order
/// @return If the order was valid or not
bool DeployOrder::validate() {
    if (target->getOwner() != player) {
        std::cout << "Order Invalid. This territory does not belong to you!" << std::endl;
        return false;
    } else if (amount > player->getPool()) {
        std::cout << "Order Invalid. You do not have this many armies in your reinforcement pool!" << std::endl;
        return false;
    } else if (amount < 1) {
        std::cout << "Order Invalid. At least one army must be deployed!" << std::endl;
        return false;
    }

    std::cout << "Order Valid" << std::endl;
    return true;
}

/// @brief Validates and executes a Deploy Order
void DeployOrder::execute() {
    if (validate()) {
        target->setArmies(amount + target->getArmies());
        player->removeArmiesFromPool(amount);
        std::cout << "Deploy executed!" << std::endl;
        notify(this);
    }
}

/// @brief Makes a new instance of an existing Deploy Order
/// @return A new DeployOrder object
DeployOrder* DeployOrder::clone() const {
    return new DeployOrder(*this);
}

void DeployOrder::print(std::ostream& out) const {
    out << "\t- Player: " << *player << std::endl
        << "\t- Target: " << *target << std::endl
        << "\t- Amount: " << amount << std::endl;
}

// Override for assignment operator for DeployOrder
DeployOrder& DeployOrder::operator=(const DeployOrder& other) {
    if (this != &other)
        Order::operator=(other);

    return *this;
}

/// @brief Order for Advance action
/// @param player The player issuing the order
/// @param source The territory from where to start
/// @param target The target territory
/// @param amount The amount of units
AdvanceOrder::AdvanceOrder(Player* player, Territory* source, Territory* target, int amount, Deck* deck)
    : Order("Advance")
    , player(player)
    , source(source)
    , target(target)
    , amount(amount)
    , deck(deck) { }

/// @brief Copy constructor implementation for AdvanceOrder object
/// @param other AdvanceOrder object
AdvanceOrder::AdvanceOrder(const AdvanceOrder& other)
    : Order(other)
    , player(other.player)
    , source(other.source)
    , target(other.target)
    , amount(other.amount) { }

/// @brief Validates the Advance order
/// @return If the order was valid or not
bool AdvanceOrder::validate() {
    if (source->getOwner() != player) {
        std::cout << "Order Invalid. The source territory does not belong to you!" << std::endl;
        return false;
    }
    if (amount > source->getArmies()) {
        std::cout << "Order Invalid. You do not have this many armies in this territory!" << std::endl;
        return false;
    }
    if (amount < 1) {
        std::cout << "Order Invalid. At least one army must be deployed!" << std::endl;
        return false;
    }
    const auto& adjacentToSource = source->getAdjacent();
    if (std::find(adjacentToSource.begin(), adjacentToSource.end(), target) == adjacentToSource.end()) {
        std::cout << "Order Invalid. The target territory is not adjacent to yours!" << std::endl;
        return false;
    }
    if (player->isFriendsWith(target->getOwner())) {
        std::cout << "Order Invalid. You cannot attack this player for this turn!" << std::endl;
        return false;
    }

    std::cout << "Order Valid." << std::endl;
    return true;
}

/// @brief Validates and executes an Advance Order
void AdvanceOrder::execute() {
    if (validate()) {
        if (source->getOwner() == target->getOwner()) {
            source->setArmies(source->getArmies() - amount);
            target->setArmies(target->getArmies() + amount);
        }
        // Attack enemy territory instead
        else {
            simulateAttack(player, source, target, amount);
        }
        std::cout << "Advance order executed!" << std::endl;
        notify(this);
    }
}

/// @brief Helper function for simulating an attack when an AdvanceOrder is called
/// @param player The player issuing a successful Advance order
/// @param source Source territory
/// @param target Target territory
/// @param amount The amount of units
void AdvanceOrder::simulateAttack(Player* sPlayer, Territory* sSource, Territory* sTarget, int sAmount) {
    int successfulAttacks = 0;
    int successfulDefends = 0;
    std::mt19937 rng(std::random_device {}());
    std::uniform_int_distribution<std::mt19937::result_type> range(0, 100);

    // Remove attackers from source territory
    sSource->setArmies(sSource->getArmies() - sAmount);

    // Attackers
    for (int i = 1; i <= sAmount; i++) {
        int roll = range(rng) % 100 + 1;
        if (roll <= 60)
            successfulAttacks++;
    }

    // Defenders
    for (int i = 1; i <= sTarget->getArmies(); i++) {
        int roll = range(rng) % 100 + 1;
        if (roll <= 70)
            successfulDefends++;
    }

    int remainingAttackers = std::max(sAmount - successfulDefends, 0);
    int remainingDefenders = std::max(sTarget->getArmies() - successfulAttacks, 0);

    if (remainingAttackers > 0 && remainingDefenders == 0) {
        // Victory
        std::cout << "Territory conquered! " << sPlayer->getName() << " won the battle for " << sTarget->getName() << "!" << std::endl;
        if (sTarget->getOwner())
            sTarget->getOwner()->removeTerritory(sTarget);
        sPlayer->addTerritory(sTarget);
        sTarget->setArmies(remainingAttackers);

        // TODO: See how to give the player a card
        std::cout << player->getName() << " won a new card!" << std::endl;
        player->getHand()->addCard(deck->draw());
    } else {
        // Loss
        std::cout << "Territory was not conquered! " << sPlayer->getName() << " lost the battle for " << sTarget->getName() << "!" << std::endl;
        sSource->setArmies(sSource->getArmies() + remainingAttackers);
        sTarget->setArmies(remainingDefenders);
    }

    // Update territory ownership if there's no remaining armies
    if (source->getArmies() == 0) {
        std::cout << sPlayer->getName() << " has lost the " << sSource->getName() << " territory in the process!";
        if (sSource->getOwner())
            sSource->getOwner()->removeTerritory(sSource);
    }
    if (target->getArmies() == 0) {
        std::cout << sTarget->getOwner()->getName() << " has lost the " << sTarget->getName() << " territory in the process!";
        if (sTarget->getOwner())
            sTarget->getOwner()->removeTerritory(sTarget);
    }
}

/// @brief Make a new instance of an existing Advance Order
/// @return A new AdvanceOrder object
AdvanceOrder* AdvanceOrder::clone() const {
    return new AdvanceOrder(*this);
}

void AdvanceOrder::print(std::ostream& out) const {
    out << "\t- Player: " << *player << std::endl
        << "\t- Source: " << *source << std::endl
        << "\t- Target: " << *target << std::endl
        << "\t- Amount: " << amount << std::endl;
}

// Override for assignment operator for AdvanceOrder
AdvanceOrder& AdvanceOrder::operator=(const AdvanceOrder& other) {
    if (this != &other)
        Order::operator=(other);

    return *this;
}

/// @brief Order for Bomb action
/// @param player The player issuing the order
/// @param target The target territory
BombOrder::BombOrder(Player* player, Territory* target)
    : Order("Bomb")
    , player(player)
    , target(target) { }

/// @brief Copy constructor implementation for BombOrder object
/// @param other BombOrder object
BombOrder::BombOrder(const BombOrder& other)
    : Order(other)
    , player(other.player)
    , target(other.target) { }

/// @brief Validates the Bomb order
/// @return If the order was valid or not
bool BombOrder::validate() {
    if (target->getOwner() == player) {
        std::cout << "Order Invalid. You cannot bomb your own territory!" << std::endl;
        return false;
    }

    bool isAdjacent = false;
    for (auto t : player->getOwnedTerritories()) {
        const auto& adjacentToT = t->getAdjacent();

        if (std::find(adjacentToT.begin(), adjacentToT.end(), target) != adjacentToT.end()) {
            isAdjacent = true;
            break;
        }
    }
    if (!isAdjacent) {
        std::cout << "Order Invalid. The target territory is not adjacent to yours!" << std::endl;
        return false;
    }

    if (player->isFriendsWith(target->getOwner())) {
        std::cout << "Order Invalid. You cannot attack this player for this turn!" << std::endl;
        return false;
    }
    std::cout << "Order Valid" << std::endl;
    return true;
}

/// @brief Validates and executes a Bomb Order
void BombOrder::execute() {
    if (validate()) {
        if (target->getArmies() > 0)
            target->setArmies(target->getArmies() / 2);

        // Remove ownership of territory if the owner has no more forces
        if (target->getArmies() == 0) {
            if (target->getOwner())
                target->getOwner()->removeTerritory(target);
        }
        std::cout << "Bomb order executed!" << std::endl;
        notify(this);
    }
}

/// @brief Make a new instance of an existing Bomb Order
/// @return A new BombOrder object
BombOrder* BombOrder::clone() const {
    return new BombOrder(*this);
}

void BombOrder::print(std::ostream& out) const {
    out << "\t- Player: " << *player << std::endl
        << "\t- Target: " << *target << std::endl;
}

// Override for assignment operator for AdvanceOrder
BombOrder& BombOrder::operator=(const BombOrder& other) {
    if (this != &other)
        Order::operator=(other);

    return *this;
}

/// @brief Order for Blockade action
/// @param player The player issuing the order
/// @param target The target territory
BlockadeOrder::BlockadeOrder(Player* player, Territory* target)
    : Order("Blockade")
    , player(player)
    , target(target) { }

/// @brief Copy constructor implementation for BlockadeOrder object
/// @param other BlockadeOrder object
BlockadeOrder::BlockadeOrder(const BlockadeOrder& other)
    : Order(other)
    , player(other.player)
    , target(other.target) { }

/// @brief Validates the Blockade order
/// @return If the order was valid or not
bool BlockadeOrder::validate() {
    if (target->getOwner() != player) {
        std::cout << "Order Invalid. This order can only be played on your own territory!" << std::endl;
        return false;
    }
    std::cout << "Order Valid" << std::endl;
    return true;
}

/// @brief Validates and executes a Blockade Order
void BlockadeOrder::execute() {
    if (validate()) {
        target->setArmies(target->getArmies() * 2);

        // Remove player ownership of the territory
        if (target->getOwner())
            target->getOwner()->removeTerritory(target);

        std::cout << "Blockade executed!" << std::endl;
        notify(this);
    }
}

/// @brief Make a new instance of an existing Blockade Order
/// @return A new BlockadeOrder
BlockadeOrder* BlockadeOrder::clone() const {
    return new BlockadeOrder(*this);
}

void BlockadeOrder::print(std::ostream& out) const {
    out << "\t- Player: " << *player << std::endl
        << "\t- Target: " << *target << std::endl;
}

// Override for assignment operator for AdvanceOrder
BlockadeOrder& BlockadeOrder::operator=(const BlockadeOrder& other) {
    if (this != &other)
        Order::operator=(other);

    return *this;
}

/// @brief Order for Airlift action
/// @param player The player issuing the order
/// @param source The territory from where to start
/// @param target The target territory
/// @param amount The amount of units
AirliftOrder::AirliftOrder(Player* player, Territory* source, Territory* target, int amount)
    : Order("Airlift")
    , player(player)
    , source(source)
    , target(target)
    , amount(amount) { }

/// @brief Copy constructor implementation for AirliftOrder object
/// @param other AirliftOrder object
AirliftOrder::AirliftOrder(const AirliftOrder& other)
    : Order(other)
    , player(other.player)
    , source(other.source)
    , target(other.target)
    , amount(other.amount) { }

/// @brief Validates the Airlift order
/// @return If the order was valid or not
bool AirliftOrder::validate() {
    if (source->getOwner() != player) {
        std::cout << "Order Invalid. The starting territory is not yours!" << std::endl;
        return false;
    } else if (target->getOwner() != player) {
        std::cout << "Order Invalid. The destination territory is not yours!" << std::endl;
        return false;
    } else if (amount > source->getArmies()) {
        std::cout << "Order Invalid. You do not have this many armies in this territory!" << std::endl;
        return false;
    } else if (amount < 1) {
        std::cout << "Order Invalid. At least one army must be deployed!" << std::endl;
        return false;
    }
    std::cout << "Order Valid." << std::endl;
    return true;
}

/// @brief Validates and executes an Airlift Order
void AirliftOrder::execute() {
    if (validate()) {
        if (source->getOwner() == target->getOwner()) {
            source->setArmies(source->getArmies() - amount);
            target->setArmies(target->getArmies() + amount);
        }
        // Remove ownership of territory if the player has no more forces
        if (source->getArmies() == 0) {
            if (source->getOwner())
                source->getOwner()->removeTerritory(source);
        }
        std::cout << "Airlift executed!" << std::endl;
        notify(this);
    }
}

/// @brief Make a new instance of an existing Airlift Order
/// @return A new AirliftOrder object
AirliftOrder* AirliftOrder::clone() const {
    return new AirliftOrder(*this);
}

void AirliftOrder::print(std::ostream& out) const {
    out << "\t- Player: " << *player << std::endl
        << "\t- Source: " << *source << std::endl
        << "\t- Target: " << *target << std::endl
        << "\t- Amount: " << amount << std::endl;
}

// Override for assignment operator for AirliftOrder
AirliftOrder& AirliftOrder::operator=(const AirliftOrder& other) {
    if (this != &other)
        Order::operator=(other);

    return *this;
}

/// @brief Order for Negotiate action
NegotiateOrder::NegotiateOrder(Player* currentPlayer, Player* targetPlayer)
    : Order("Negotiate")
    , currentPlayer(currentPlayer)
    , targetPlayer(targetPlayer) { }

/// @brief Copy constructor implementation for NegotiateOrder object
/// @param other NegotiateOrder object
NegotiateOrder::NegotiateOrder(const NegotiateOrder& other)
    : Order(other)
    , currentPlayer(other.currentPlayer)
    , targetPlayer(other.targetPlayer) { }

/// @brief Validates the Negotiate order
/// @return If the order was valid or not
bool NegotiateOrder::validate() {
    if (currentPlayer == targetPlayer) {
        std::cout << "Order Invalid. You cannot issue this order on yourself!" << std::endl;
        return false;
    }
    std::cout << "Order Valid." << std::endl;
    return true;
}

void NegotiateOrder::execute() {
    if (validate()) {
        currentPlayer->addFriend(targetPlayer);
        targetPlayer->addFriend(currentPlayer);
        std::cout << "A negotiation was made between " << currentPlayer->getName() << " and " << targetPlayer->getName() << "!" << std::endl;
        notify(this);
    }
}

NegotiateOrder* NegotiateOrder::clone() const {
    return new NegotiateOrder(*this);
}

void NegotiateOrder::print(std::ostream& out) const {
    out << "\t- Current: " << *currentPlayer << std::endl
        << "\t- Target: " << *targetPlayer << std::endl;
}

// Override for assignment operator for AdvanceOrder
NegotiateOrder& NegotiateOrder::operator=(const NegotiateOrder& other) {
    if (this != &other)
        Order::operator=(other);

    return *this;
}

/// @brief Class for managing a list of orders
OrdersList::OrdersList()
    : Subject()
    , ILoggable()
    , orders() { }

/// @brief Copy constructor for OrdersList object
/// @param other OrdersList object
OrdersList::OrdersList(const OrdersList& other)
    : Subject()
    , ILoggable() {
    this->orders.clear();

    for (const auto& order : other.orders) {
        this->orders.push_back(order->clone());
    }
}

/// @brief OrdersList destructor
OrdersList::~OrdersList() {
    for (auto order : this->orders) {
        delete order;
    }
    this->orders.clear();
}

/// @brief Add an order to the list, if possible
/// @param order The order to be added
void OrdersList::add(Order* order) {
    if (order) {
        this->orders.push_back(order);
        notify(this);
    } else {
        throw std::invalid_argument("Order is null somehow");
    }
}

/// @brief Remove an order from the list
/// @param index The index of the order to remove
Order* OrdersList::remove(size_t index) {
    if (index >= this->orders.size()) {
        return nullptr;
    }

    Order* order = this->orders[index];
    this->orders.erase(this->orders.begin() + index);
    return order;
}

/// @brief Swaps the position of 2 orders within the list
/// @param fromIndex The first order you wish to move
/// @param toIndex The second order you wish to move
void OrdersList::move(size_t fromIndex, size_t toIndex) {
    if (fromIndex >= this->orders.size() || toIndex >= orders.size()) {
        // throw error
    }
    std::swap(this->orders[fromIndex], this->orders[toIndex]);
}

// Override for printing out OrdersList contents
std::ostream& operator<<(std::ostream& out, const OrdersList& ordersList) {
    for (size_t i = 0; i < ordersList.orders.size(); i++) {
        out << "Order #" << (i + 1) << ":\n"
            << *ordersList.orders[i];
    }
    return out;
}

void OrdersList::observeAllOrders(Observer* observer) {
    for (auto order : this->orders) {
        order->attach(observer);
    }
}

std::string OrdersList::stringToLog() const {
    std::string order = this->orders.back()->stringToLog();
    std::string output = "Order has been added to OrderList : " + order;
    return output;
}

// Override for assignment operator for OrdersList
OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this != &other) {
        // Clear existing orders
        for (auto& order : this->orders) {
            delete order;
        }
        this->orders.clear();

        // Copy of the orders from the other OrdersList object
        for (const auto& order : other.orders) {
            if (order)
                this->orders.push_back(order->clone());
            else
                this->orders.push_back(nullptr);
        }
    }
    return *this;
}
