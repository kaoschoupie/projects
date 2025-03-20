#include "GameEngine.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "CommandProcessing.h"
#include "Map.h"
#include "Orders.h"
#include "Player.fwd.h"
#include "Player.h"
#include "PlayerStrategies.h"

using GameState = Game::GameState;

Game::Game(CommandProcessor* cp)
    : Subject()
    , ILoggable()
    , map(new Map)
    , state(GameState::Start)
    , deck(new Deck)
    , cp(cp) {
}

Game::Game()
    : Game(new CommandProcessor) {
}

Game::Game(Map* map)
    : Subject()
    , ILoggable()
    , map(map)
    , state(GameState::Start)
    , deck(new Deck)
    , cp(new CommandProcessor) {
}

Game::~Game() {
    delete this->map;
    delete this->deck;
    delete this->cp;
    for (auto p : players) {
        delete p;
    }
}

Game::Game(const Game& other)
    : Subject()
    , ILoggable() {
    map = new Map(*other.map);
    for (Player* p : other.players) {
        this->players.push_back(new Player(*p));
    }
    deck = new Deck(*other.deck);
    state = other.state;
    cp = other.cp;
}

void Game::transition(GameState state) {
    this->state = state;
    notify(this);
}

void Game::observe(Observer* observer) {
    this->cp->attach(observer);
}
void Game::observerPlayers(Observer* observer) {
    for (size_t i = 0; i < this->players.size(); i++) {
        this->players[i]->observer(observer);
    }
}

void Game::loadmap(std::ifstream file) {
    this->map = MapLoader(file).parse();
}

bool Game::validatemap() {
    if (!this->map->validate()) {
        std::cout << "The loaded map is invalid. Please choose a valid map." << std::endl;
        delete this->map;
        return false;
    }
    std::cout << "Map validated." << std::endl;
    return true;
}

void Game::addplayer(Player* p) {
    this->players.push_back(p);
    p->initStrategy(this->map, this->deck, &this->players);
}

void Game::gamestart() {
    std::default_random_engine gen(1);
    // shuffle player list
    std::shuffle(this->players.begin(), this->players.end(), gen);
    std::cout << "The player order is:" << std::endl;
    // print the player order and add reinforcement pool
    for (size_t i = 0; i < this->players.size(); i++) {
        std::cout << *this->players[i] << std::endl;
        this->players[i]->addReinforcementToPool(50);
    }
    std::cout << "Players have been awarded 50 reinforcements each" << std::endl;
    // give territories to players
    for (size_t i = 0; i < this->map->getNumberTerritories(); i++) {
        Territory* tempTerr = this->map->findTerritoryByIndex(i);
        Player* tempPlayer = this->players[i % (this->players.size())];
        tempPlayer->addTerritory(tempTerr);
    }
    std::cout << "Players have been awarded territories" << std::endl;
    // Have each player draw 2 cards
    for (size_t i = 0; i < this->players.size(); i++) {
        for (size_t j = 0; j < 2; j++) {
            Card* card = this->deck->draw();
            this->players[i]->addCardToHand(card);
        }
    }
    std::cout << "Players have been awarded 2 cards each" << std::endl;
    std::cout << std::flush;
}

void Game::startupPhase() {
    while (this->state != GameState::FirstReinforcements) {
        const Command* command;
        try {
            command = cp->getCommand();
        } catch (CommandException& e) {
            std::cout << "Please enter a valid command. " << e.what() << std::endl;
            continue;
        }
        if (!cp->validate(this->state)) {
            std::cout << "Please enter a valid command. " << command->getEffect().value_or("") << std::endl;
            continue;
        }
        if (command->getCommand() == "tournament") {
            tournament(command->getArgument().value());
        }

        if ((command->getCommand() == "loadmap")) {

            loadmap(std::ifstream(command->getArgument().value()));
            transition(GameState::MapLoaded);
        }
        if (command->getCommand() == "validatemap") {
            if (!this->validatemap()) {
                std::cout << "The loaded map is not valid. Please load another map." << std::endl;
                transition(GameState::Start);
            }
            transition(GameState::MapValidated);
        }
        if (command->getCommand() == "addplayer") {
            std::string name = command->getArgument().value();
            this->players.push_back(new Player(name, map, deck, this->players));
            transition(GameState::PlayersAdded);
        }
        if (command->getCommand() == "gamestart") {
            this->gamestart();
            transition(GameState::FirstReinforcements);
            command = nullptr;
            return;
        }
        command = nullptr;
    }
}

void Game::mainGameLoop() {
    mainGameLoop(std::numeric_limits<size_t>::max());
}

Player* Game::mainGameLoop(size_t turns) {
    // allow all turns to execute or game to end
    while (!gameEnded() && turns > 0) {
        for (auto& p : players) {
            p->clearFriends();
        }
        removeDefeatedPlayers();
        if (state != GameState::AssignReinforcements && state != GameState::FirstReinforcements)
            transition(GameState::AssignReinforcements);
        reinforcementPhase();
        transition(GameState::IssueOrders);
        issueOrdersPhase();
        transition(GameState::ExecuteOrders);
        executeOrdersPhase();
        turns--;
    }

    if (!players.empty()) {
        // if a player has won
        if (players.size() == 1) {
            std::cout << "Game Over! Player " << players[0]->getName() << " wins!" << std::endl;
        }
        // if 2 or more players are still in the game when it ends
        else {
            return nullptr;
        }
    }
    return players[0];
}

int Game::calculateReinforcements(Player* player) {
    int reinforcements = player->getTerritories() / 3;
    reinforcements += map->continentRewards(player);
    if (reinforcements < 3)
        reinforcements = 3;
    return reinforcements;
}

void Game::reinforcementPhase() {
    std::cout << "\n=== Reinforcement Phase ===" << std::endl;

    for (Player* player : players) {
        int reinforcements = calculateReinforcements(player);
        std::cout << "calculated reinforcements" << std::endl;
        player->addReinforcementToPool(reinforcements);
        std::cout << "Player " << player->getName()
                  << " receives " << reinforcements
                  << " reinforcements" << std::endl;
    }
}

void Game::issueOrdersPhase() {
    std::cout << "\n=== Issue Orders Phase ===" << std::endl;

    for (Player* player : players) {
        player->issueOrder();

        Order* order = nullptr;
        while ((order = player->getNextOrder()) != nullptr) {
            order->execute();
        }
    }
}

void Game::executeOrdersPhase() {
    std::cout << "\n=== Execute Orders Phase ===" << std::endl;

    // Execute all deploy orders first
    bool deployOrdersRemaining;
    do {
        deployOrdersRemaining = false;
        for (Player* player : players) {
            Order* order = player->getNextOrder();
            if (order && dynamic_cast<DeployOrder*>(order)) {
                order->execute();
                delete order;
                deployOrdersRemaining = true;
            }
        }
    } while (deployOrdersRemaining);

    // Execute remaining orders
    bool ordersRemaining;
    do {
        ordersRemaining = false;
        for (Player* player : players) {
            Order* order = player->getNextOrder();
            if (order) {
                order->execute();
                delete order;
                ordersRemaining = true;
            }
        }
    } while (ordersRemaining);
}

bool Game::gameEnded() {
    if (players.size() <= 1)
        return true;

    for (Player* player : players) {
        if (player->getTerritories() == map->getNumberTerritories()) {
            return true;
        }
    }
    return false;
}

void Game::removeDefeatedPlayers() {
    auto it = std::remove_if(players.begin(), players.end(), [](Player* player) { return player->getTerritories() == 0; });

    for (auto toDelete = it; toDelete != players.end(); ++toDelete) {
        std::cout << "Player " << (*toDelete)->getName()
                  << " has been eliminated!" << std::endl;
        delete *toDelete;
    }
    players.erase(it, players.end());
}

void Game::tournament(std::string argument) {
    Tournament* tournament = new Tournament(argument);
    tournament->executeTournament();
    delete tournament;
    exit(0);
}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        delete map;
        map = new Map(*other.map);

        for (auto p : players) {
            delete p;
        }
        players.clear();

        for (Player* p : other.players) {
            players.push_back(new Player(*p));
        }

        delete deck;
        deck = new Deck(*other.deck);
        state = other.state;
    }
    return *this;
}

std::string Game::stateString() const {
    switch (state) {
    case Game::GameState::Start:
        return "Start";
    case Game::GameState::MapLoaded:
        return "Map loaded";
    case Game::GameState::MapValidated:
        return "Map validated";
    case Game::GameState::PlayersAdded:
        return "Players added";
    case Game::GameState::FirstReinforcements:
        return "Assign reinforcements";
    case Game::GameState::AssignReinforcements:
        return "Assign reinforcements";
    case Game::GameState::IssueOrders:
        return "Issue orders";
    case Game::GameState::ExecuteOrders:
        return "Execute orders";
    case Game::GameState::Win:
        return "Win";
    case Game::GameState::Tournament:
        return "Tournament";
    default:
        return "Unknown state";
    }
}

std::string Game::stringToLog() const {
    std::string output = "Game has transitioned to state " + this->stateString();
    return output;
}

std::ostream& operator<<(std::ostream& out, const Game& game) {
    return out << "The game's state is " << game.stateString();
}

static std::vector<std::string> splitString(std::string input, char delimiter) {
    std::istringstream stream(input);
    std::string output;
    std::vector<std::string> vec;
    while (getline(stream, output, delimiter)) {
        vec.push_back(output);
    }
    return vec;
}

Tournament::Tournament(std::string argument)
    : Subject()
    , ILoggable() {
    // get all parts from the argument in vector of strings
    std::vector<std::string> strings = splitString(argument, ' ');
    if (strings.size() != 8) {
        std::cout << strings.size();
        throw std::invalid_argument("Invalid command argument");
    }
    for (size_t i = 1; i < strings.size(); i += 2) {
        switch (i) {
            // process maps
        case 1: {
            std::vector<std::string> mapString = splitString(strings[i], ',');
            std::cout << strings[i] << std::endl;
            if (mapString.size() > 5 || mapString.size() < 1) {
                throw std::invalid_argument("Invalid number of maps");
            }
            for (size_t j = 0; j < mapString.size(); j++) {
                std::ifstream file = std::ifstream(mapString[j]);
                maps.push_back(MapLoader(file).parse());
                if (!maps[j]->validate())
                    throw std::invalid_argument("Invalid map loaded");
            }
        } break;
        // process players
        case 3: {
            std::vector<std::string> playerStrings = splitString(strings[i], ',');
            if (playerStrings.size() > 4 || playerStrings.size() < 1) {
                throw std::invalid_argument("Invalid number of players");
            }
            for (size_t j = 0; j < playerStrings.size(); j++) {
                std::string playerString = playerStrings[j];
                PlayerStrategy* strategy;
                if (playerString == "human") {
                    strategy = new HumanPlayer();
                } else if (playerString == "aggressive") {
                    strategy = new AggressivePlayer();
                } else if (playerString == "benevolent") {
                    strategy = new BenevolentPlayer();
                } else if (playerString == "neutral") {
                    strategy = new NeutralPlayer();
                } else if (playerString == "cheater") {
                    strategy = new CheaterPlayer();
                } else {
                    throw std::runtime_error((std::stringstream {} << "Invalid strategy " << playerString).str());
                }

                players.push_back(new Player(playerString, strategy));
            }
        } break;
        // process number of games
        case 5: {
            int games = std::stoi(strings[i], 0, 10);
            if (games < 1 || games > 5)
                throw std::invalid_argument("Invalid number of games");
            nbGames = games;
        } break;
        // process number of turns
        case 7: {
            int turns = std::stoi(strings[i], 0, 10);
            if (turns < 10 || turns > 50)
                throw std::invalid_argument("Invalid number of games");
            nbTurns = turns;
        }
        }
    }
}

Tournament::Tournament(const Tournament& other)
    : Subject()
    , ILoggable() {
    this->maps = other.maps;
    this->players = other.players;
    this->nbGames = other.nbGames;
    this->nbTurns = other.nbTurns;
}
Tournament::~Tournament() {
    for (auto m : maps) {
        delete m;
    }
    maps.clear();
    for (auto p : players) {
        delete p;
    }
    players.clear();
}

void Tournament::executeTournament() {
    for (size_t i = 0; i < maps.size(); i++) {
        for (size_t j = 0; j < nbGames; j++) {
            // create a copy of map so game doesn'T delete the main map
            Game* game = new Game(new Map(*maps[i]));
            game->transition(Game::GameState::MapLoaded);
            game->transition(Game::GameState::MapValidated);
            for (auto p : players) {
                game->addplayer(new Player(*p));
                game->transition(Game::GameState::PlayersAdded);
            }
            game->gamestart();
            game->transition(Game::GameState::FirstReinforcements);
            // get game winner
            Player* winner = game->mainGameLoop(nbTurns);
            if (winner == nullptr) {
                winners.push_back("draw");
            } else
                winners.push_back(winner->getName());
            delete game;
        }
    }
    notify(this);
}

Tournament& Tournament::operator=(const Tournament& other) {
    if (this != &other) {
        for (auto m : maps) {
            delete m;
        }
        maps.clear();
        for (auto p : players) {
            delete p;
        }
        players.clear();
        for (auto m : other.maps) {
            this->maps.push_back(new Map(*m));
        }
        for (auto p : other.players) {
            this->players.push_back(new Player(*p));
        }
        nbGames = other.nbGames;
        nbTurns = other.nbTurns;
    }
    return *this;
}
std::string Tournament::stringToLog() const {
    std::stringstream result;

    result << "Tournament mode\n "
           << this->maps.size() << " maps \n"
           << this->players.size() << " players "
           << this->nbGames << " games "
           << this->nbTurns << " turns"
           << "\n\t";

    for (size_t i = 1; i <= nbGames; i++) {
        result << "Game " << i << "\t";
    }
    result << "\n";
    for (size_t i = 1; i <= maps.size(); i++) {
        result << "Map " << i << "\t";
        for (size_t j = 0; j < nbGames; j++) {
            result << winners[i * j] << "\t";
        }
        result << "\n";
    }
    return result.str();
}
