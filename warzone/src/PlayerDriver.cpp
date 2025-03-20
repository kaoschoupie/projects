#include "PlayerDriver.h"
#include "Player.h"
#include <iostream>

void testPlayer() {
    std::cout << "This section is from PlayerDriver" << std::endl;
    std::ifstream file("./res/map/lp.map");
    Map* map = MapLoader(file).parse();
    Deck* deck = new Deck();
    std::vector<Player*> players;

    // Create player
    Player player("John Warzone", map, deck, players);
    players.push_back(&player);
    std::cout << "Player:" << std::endl
              << player << std::endl;

    // Defend
    std::vector<Territory*> toDefend = player.toDefend();
    std::cout << "To defend: (length " << toDefend.size() << ")" << std::endl;
    for (auto t : toDefend) {
        std::cout << *t << std::endl;
    }

    // Attack
    std::vector<Territory*> toAttack = player.toAttack();
    std::cout << "To attack: (length " << toAttack.size() << ")" << std::endl;
    for (auto t : toAttack) {
        std::cout << *t << std::endl;
    }

    // Issue order
    std::cout << "Before issue order:" << std::endl
              << player.getOrders() << std::endl;
    player.issueOrder();
    std::cout << "After issue order:" << std::endl
              << player.getOrders() << std::endl;

    // Get name
    std::cout << "Name:" << std::endl
              << player.getName() << std::endl;
}
