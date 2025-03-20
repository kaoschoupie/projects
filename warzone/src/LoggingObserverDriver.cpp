#include "LoggingObserverDriver.h"
#include "CommandProcessing.h"
#include "GameEngine.h"
#include "GameEngineDriver.h"
#include "LoggingObserver.h"
#include "Orders.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

template <typename T>
bool isLoggable() {
    return std::is_base_of<ILoggable, T>::value;
}

template <typename T>
bool isSubject() {
    return std::is_base_of<Subject, T>::value;
}

void testLoggingObserver() {
    Game* game = new Game();
    LogObserver* observer = new LogObserver();
    game->attach(observer);

    std::cout << "Does Command inherit ILoggable?: " << isLoggable<Command>() << std::endl
              << "Does Command inherit Subject?: " << isSubject<Command>() << std::endl
              << "Does CommandProcessor inherit ILoggable?: " << isLoggable<CommandProcessor>() << std::endl
              << "Does CommandProcessor inherit Subject?: " << isSubject<CommandProcessor>() << std::endl
              << "Does Order inherit ILoggable?: " << isLoggable<Order>() << std::endl
              << "Does Order inherit Subject?: " << isSubject<Order>() << std::endl
              << "Does OrdersList inherit ILoggable?: " << isLoggable<OrdersList>() << std::endl
              << "Does OrdersList inherit Subject?: " << isSubject<OrdersList>() << std::endl
              << "Does GameEngine inherit ILoggable?: " << isLoggable<Game>() << std::endl
              << "Does GameEngine inherit Subject?: " << isSubject<Game>() << std::endl;

    std::cout << "\n\n"
              << std::endl;

    std::cout << "Making GameEngine transition" << std::endl;
    game->transition(Game::GameState::IssueOrders);
    std::cout << "The log is : " << std::endl;
    std::ifstream file(logFile);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
    }
    std::cout << line << std::endl;
    file.close();
    testMainGameLoop();
}
