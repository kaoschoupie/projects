#pragma once

#include "CommandProcessing.fwd.h"
#include "GameEngineDriver.h"
#include "Map.h"
#include "Orders.h"
#include "Player.fwd.h"
#include "Player.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

class Game : public Subject, public ILoggable {
public:
    enum class GameState : char {
        Start,
        MapLoaded,
        MapValidated,
        PlayersAdded,
        FirstReinforcements,
        AssignReinforcements,
        IssueOrders,
        ExecuteOrders,
        Win,
        Tournament,
    };

private:
    Map* map;
    GameState state;
    std::vector<Player*> players;
    Deck* deck;
    CommandProcessor* cp;
    int calculateReinforcements(Player* player);

public:
    Game();
    Game(CommandProcessor* cp);
    Game(Map* map);
    ~Game();
    Game(const Game& other);

    void startupPhase();
    void mainGameLoop();
    Player* mainGameLoop(size_t turns);

    /**
     * @brief wrapper method to transition states and trigger notify()
     */
    void transition(GameState state);
    void observe(Observer* observer);
    void observerPlayers(Observer* observer);
    void loadmap(std::ifstream file);
    bool validatemap();
    void addplayer(Player* p);
    void gamestart();
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    bool gameEnded();
    void removeDefeatedPlayers();
    void tournament(std::string argument);
    Game& operator=(const Game& other);

    std::string stateString() const;

    std::string stringToLog() const override;

    friend std::ostream& operator<<(std::ostream& out, const Game& game);
    friend void testMainGameLoop();
};

class Tournament : public Subject, public ILoggable {
private:
    std::vector<Map*> maps;
    std::vector<Player*> players;
    size_t nbGames;
    size_t nbTurns;
    std::vector<std::string> winners;

public:
    Tournament(std::string argument);
    Tournament(const Tournament& other);
    ~Tournament();
    void executeTournament();

    Tournament& operator=(const Tournament& other);

    std::string stringToLog() const override;
};
