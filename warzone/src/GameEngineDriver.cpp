#include "GameEngineDriver.h"
#include "GameEngine.h"
#include <iostream>
#include <stdexcept>

void testGameStates() {
    Game* game = new Game();

    LogObserver* observer = new LogObserver();
    game->attach(observer);
    game->startupPhase();
    game->observerPlayers(observer);

    game->mainGameLoop();
    delete game;
}
void testStartupPhase() {
    Game* game = new Game();
    LogObserver* observer = new LogObserver();
    game->attach(observer);
    game->startupPhase();
    game->observerPlayers(observer);
    delete game;
}

void testMainGameLoop() {
    // Create test map with continents
    Game* game = new Game();
    LogObserver* observer = new LogObserver();
    game->attach(observer);
    game->observerPlayers(observer);

    game->loadmap(std::ifstream("res/map/lp.map"));
    game->validatemap();
    // Create test players
    Player* p1 = new Player("Player 1", game->map, game->deck, game->players);
    Player* p2 = new Player("Player 2", game->map, game->deck, game->players);
    game->addplayer(p1);
    game->addplayer(p2);

    // Initial territory distribution
    game->gamestart();

    // Give players some cards

    std::cout << "=== Starting Main Game Loop Test ===" << std::endl;

    // Test reinforcement calculation
    std::cout << "\nTesting reinforcement calculation:" << std::endl;
    game->reinforcementPhase();
    game->transition(Game::GameState::IssueOrders);
    // Test order issuing
    std::cout << "\nTesting order issuing:" << std::endl;
    game->issueOrdersPhase();
    game->transition(Game::GameState::ExecuteOrders);

    // Test order execution
    std::cout << "\nTesting order execution:" << std::endl;
    game->executeOrdersPhase();

    // Test player elimination
    std::cout << "\nTesting player elimination:" << std::endl;
    game->removeDefeatedPlayers();

    std::cout << "\n=== Main Game Loop Test Complete ===" << std::endl;
    delete game;
    delete p1;
    delete p2;
}

void testTournament() {
    std::string argument = "-M res/map/lp.map,res/map/Cobra.map -P neutral,aggressive -G 3 -D 30";
    Tournament* tournament;
    try {
        tournament = new Tournament(argument);
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
    LogObserver* observer = new LogObserver();
    tournament->attach(observer);
    tournament->executeTournament();
    delete tournament;
}
