#include "PlayerStrategiesDriver.h"
#include "Cards.fwd.h"
#include "Player.h"
#include "PlayerStrategies.h"

void testHumanStrategy() {
    std::ifstream file("./res/map/lp.map");
    Map* map = MapLoader(file).parse();
    Deck* deck = new Deck();
    std::vector<Player*> players;

    PlayerStrategy* _friendStrategy = new HumanPlayer(map, deck, &players);
    Player* _friend = new Player("Willem Dafriend", _friendStrategy);
    players.push_back(_friend);

    PlayerStrategy* strategy = new HumanPlayer(map, deck, &players);
    Player* player = new Player("John Warzone", strategy);
    players.push_back(player);

    player->getHand()->addCard(new Card(CardType::DIPLOMACY));

    player->addTerritory(map->findTerritory("1L"));
    player->addTerritory(map->findTerritory("8LS"));

    player->issueOrder();
    std::cout << player->getOrders() << std::endl;
    std::cout << *map << std::endl;

    delete player;
    delete _friend;
    delete deck;
    delete map;
    file.close();
}

void testAggressiveStrategy() {
    std::ifstream file("./res/map/lp.map");
    Map* map = MapLoader(file).parse();
    Deck* deck = new Deck();
    std::vector<Player*> players;

    PlayerStrategy* aggroStrategy = new AggressivePlayer(map, deck, &players);
    Player* foe = new Player("Willem Dafoe", aggroStrategy);
    players.push_back(foe);

    PlayerStrategy* humanStrategy = new HumanPlayer(map, deck, &players);
    Player* player = new Player("John Warzone", humanStrategy);
    players.push_back(player);

    // May or may not be played given that playing a card is 50/50
    foe->getHand()->addCard(new Card(CardType::BOMB));

    player->addTerritory(map->findTerritory("1L"));
    foe->addTerritory(map->findTerritory("2L"));

    foe->addReinforcementToPool(100);
    player->addReinforcementToPool(5);

    foe->issueOrder();
    std::cout << foe->getOrders() << std::endl;
    std::cout << *map << std::endl;

    delete player;
    delete foe;
    delete deck;
    delete map;
    file.close();
}

void testBenevolentStrategy() {
    std::ifstream file("./res/map/lp.map");
    Map* map = MapLoader(file).parse();
    Deck* deck = new Deck();
    std::vector<Player*> players;

    PlayerStrategy* benStrategy = new BenevolentPlayer(map, deck, &players);
    Player* ben = new Player("Willem DaBenevolent", benStrategy);
    players.push_back(ben);

    PlayerStrategy* aggroStrategy = new AggressivePlayer(map, deck, &players);
    Player* foe = new Player("Willem Dafoe", aggroStrategy);
    players.push_back(foe);

    foe->addTerritory(map->findTerritory("1L"));
    ben->addTerritory(map->findTerritory("2L"));

    foe->addReinforcementToPool(10);
    ben->addReinforcementToPool(10);

    ben->issueOrder();
    std::cout << ben->getOrders() << std::endl;
    std::cout << *map << std::endl;

    foe->issueOrder();
    std::cout << foe->getOrders() << std::endl;
    std::cout << *map << std::endl;

    delete foe;
    delete ben;
    delete deck;
    delete map;
    file.close();
}

void testNeutralStrategy() {
    std::ifstream file("./res/map/lp.map");
    Map* map = MapLoader(file).parse();
    Deck* deck = new Deck();
    std::vector<Player*> players;

    PlayerStrategy* aggroStrategy = new AggressivePlayer(map, deck, &players);
    Player* foe = new Player("Willem Dafoe", aggroStrategy);
    players.push_back(foe);

    PlayerStrategy* neutralStrategy = new NeutralPlayer(map, deck, &players);
    Player* neutral = new Player("Willem DaAcquaintance", neutralStrategy);
    players.push_back(neutral);

    // May or may not be played given that playing a card is 50/50
    foe->getHand()->addCard(new Card(CardType::BOMB));

    neutral->addTerritory(map->findTerritory("1L"));
    foe->addTerritory(map->findTerritory("2L"));

    foe->addReinforcementToPool(100);
    neutral->addReinforcementToPool(50);

    neutral->issueOrder();
    std::cout << neutral->getOrders() << std::endl;
    std::cout << *map << std::endl;

    foe->issueOrder();
    std::cout << foe->getOrders() << std::endl;
    std::cout << *map << std::endl;

    neutral->removeArmiesFromPool(5);
    neutral->issueOrder();
    std::cout << neutral->getOrders() << std::endl;
    std::cout << *map << std::endl;

    delete neutral;
    delete foe;
    delete deck;
    delete map;
    file.close();
}

void testCheaterStrategy() {
    std::ifstream file("./res/map/lp.map");
    Map* map = MapLoader(file).parse();
    Deck* deck = new Deck();
    std::vector<Player*> players;
    PlayerStrategy* strategy = new CheaterPlayer(map, deck, &players);
    Player* player = new Player("John Warzone", strategy);
    players.push_back(player);

    player->addTerritory(map->findTerritory("1L"));
    player->addTerritory(map->findTerritory("8LS"));

    player->issueOrder();
    std::cout << player->getOrders() << std::endl;
    std::cout << *map << std::endl;

    delete player;
    delete deck;
    delete map;
    file.close();
}
