#include <fstream>
#include <iostream>

#include "Cards.h"
#include "Orders.h"
#include "OrdersDriver.h"
#include "Player.h"

void testOrdersList() {
    std::cout << "This section is from OrdersDriver" << std::endl;

    // Game stuff
    std::ifstream file("./res/map/lp.map");
    Map* map = MapLoader(file).parse();
    Deck* deck = new Deck();
    for (size_t i = 0; i < 4; i++)
        deck->addCard(Card(CardType::BOMB));

    std::vector<Player*> players;
    Player* john = new Player("John Warzone", map, deck, players);
    players.push_back(john);
    Player* _friend = new Player("Willem Dafriend", map, deck, players);
    players.push_back(_friend);
    Player* foe = new Player("Willem Dafoe", map, deck, players);
    players.push_back(foe);
    Territory* owned = map->findTerritory("1L");
    Territory* ownedOther = map->findTerritory("6RS");
    Territory* friends = map->findTerritory("9LS");
    Territory* adjacent = map->findTerritory("2L");
    Territory* far = map->findTerritory("5RS");

    // Create a list or orders
    OrdersList ordersList;

    // Add orders to the list
    ordersList.add(new BombOrder(john, owned));
    ordersList.add(new BombOrder(john, ownedOther));
    ordersList.add(new BombOrder(john, friends));
    ordersList.add(new BombOrder(john, adjacent));
    ordersList.add(new BombOrder(john, far));

    // Display the orders
    std::cout << "Initial list of orders:" << std::endl;
    std::cout << ordersList;

    // Move orders
    ordersList.move(0, 3);
    std::cout << "\nAfter moving first order to the fourth position:" << std::endl;
    std::cout << ordersList;

    // Remove an order
    ordersList.remove(2);
    std::cout << "\nAfter removing the third order:" << std::endl;
    std::cout << ordersList;

    // Execute some orders
    std::cout << "# Executing orders" << std::endl;
    Order* orders[] = {
        // Deploy
        new DeployOrder(john, adjacent, 1),
        new DeployOrder(john, owned, 10000),
        new DeployOrder(john, owned, 0),
        new DeployOrder(john, owned, 1),
        // Advance
        new AdvanceOrder(john, adjacent, adjacent, 1, deck),
        new AdvanceOrder(john, owned, adjacent, 10000, deck),
        new AdvanceOrder(john, owned, adjacent, 0, deck),
        new AdvanceOrder(john, owned, far, 1, deck),
        new AdvanceOrder(john, owned, adjacent, 1, deck),
        // Bomb
        new BombOrder(john, far),
        new BombOrder(john, owned),
        new BombOrder(john, friends),
        new BombOrder(john, adjacent),
        // Blockade
        new BlockadeOrder(john, far),
        new BlockadeOrder(john, owned),
        // Airlift
        new AirliftOrder(john, adjacent, adjacent, 1),
        new AirliftOrder(john, owned, friends, 1),
        new AirliftOrder(john, owned, ownedOther, 1000),
        new AirliftOrder(john, owned, ownedOther, 0),
        new AirliftOrder(john, owned, ownedOther, 1),
        // Negotiate
        new NegotiateOrder(john, john),
        new NegotiateOrder(john, _friend),
    };
    for (auto order : orders) {
        // Reset the game
        john->addReinforcementToPool(5);
        john->addFriend(_friend);

        john->addTerritory(owned);
        owned->setArmies(5);
        john->addTerritory(ownedOther);

        john->removeTerritory(adjacent);
        _friend->addTerritory(friends);

        order->execute();
        delete order;
    }

    delete map;
    delete john;
    delete _friend;
    delete foe;
    delete deck;
}
