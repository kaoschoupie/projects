#pragma once

#include "Cards.fwd.h"
#include "LoggingObserver.h"
#include "Map.h"
#include "Player.fwd.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <random>
#include <string>
#include <vector>

// Base class for orders
class Order : public Subject, public ILoggable {
protected:
    bool executed;
    std::string effect;
    std::string orderName;

public:
    Order(const std::string& name);
    Order(const Order& other);
    virtual ~Order() = 0;

    virtual bool validate() = 0;
    virtual void execute() = 0;
    virtual Order* clone() const = 0;
    virtual void print(std::ostream& out) const = 0;
    std::string stringToLog() const override;

    void attach(Observer* observer) override;

    Order& operator=(const Order& other);
    friend std::ostream& operator<<(std::ostream& out, const Order& order);
};

// Derived classes for specific order types
class DeployOrder : public Order {
public:
    DeployOrder(Player* player, Territory* target, int amount);
    DeployOrder(const DeployOrder& other);

    Player* player;
    Territory* target;
    int amount;

    bool validate() override;
    void execute() override;
    DeployOrder* clone() const override;
    void print(std::ostream& out) const override;

    DeployOrder& operator=(const DeployOrder& other);
};

class AdvanceOrder : public Order {
public:
    std::string name;
    AdvanceOrder(Player* player, Territory* source, Territory* target, int amount, Deck* deck);
    AdvanceOrder(const AdvanceOrder& other);

    Player* player;
    Territory* source;
    Territory* target;
    int amount;
    Deck* deck;

    bool validate() override;
    void execute() override;
    AdvanceOrder* clone() const override;
    void print(std::ostream& out) const override;

    void simulateAttack(Player* sPlayer, Territory* sSource, Territory* sTarget, int sAmount);

    AdvanceOrder& operator=(const AdvanceOrder& other);
};

class BombOrder : public Order {
public:
    std::string name;
    BombOrder(Player* player, Territory* target);
    BombOrder(const BombOrder& other);

    Player* player;
    Territory* target;

    bool validate() override;
    void execute() override;
    BombOrder* clone() const override;
    void print(std::ostream& out) const override;

    BombOrder& operator=(const BombOrder& other);
};

class BlockadeOrder : public Order {
public:
    std::string name;
    BlockadeOrder(Player* player, Territory* target);
    BlockadeOrder(const BlockadeOrder& other);

    Player* player;
    Territory* target;

    bool validate() override;
    void execute() override;
    BlockadeOrder* clone() const override;
    void print(std::ostream& out) const override;

    BlockadeOrder& operator=(const BlockadeOrder& other);
};

class AirliftOrder : public Order {
public:
    std::string name;
    AirliftOrder(Player* player, Territory* source, Territory* target, int amount);
    AirliftOrder(const AirliftOrder& other);

    Player* player;
    Territory* source;
    Territory* target;
    int amount;

    bool validate() override;
    void execute() override;
    AirliftOrder* clone() const override;
    void print(std::ostream& out) const override;

    AirliftOrder& operator=(const AirliftOrder& other);
};

class NegotiateOrder : public Order {
public:
    std::string name;
    NegotiateOrder(Player* currentPlayer, Player* targetPlayer);
    NegotiateOrder(const NegotiateOrder& other);

    Player* currentPlayer;
    Player* targetPlayer;

    bool validate() override;
    void execute() override;
    NegotiateOrder* clone() const override;
    void print(std::ostream& out) const override;

    NegotiateOrder& operator=(const NegotiateOrder& other);
};

// Class for managing a list of orders
class OrdersList : public Subject, public ILoggable {
private:
    std::vector<Order*> orders;

public:
    OrdersList();
    OrdersList(const OrdersList& other);
    ~OrdersList();
    void add(Order* order);
    Order* remove(size_t index);
    void move(size_t fromIndex, size_t toIndex);

    void observeAllOrders(Observer* observer);

    OrdersList& operator=(const OrdersList& other);

    std::string stringToLog() const override;

    friend std::ostream& operator<<(std::ostream& out, const OrdersList& ordersList);
};
