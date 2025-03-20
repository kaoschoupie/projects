#include "CardsDriver.h"
#include "CommandProcessingDriver.h"
#include "GameEngineDriver.h"
#include "LoggingObserverDriver.h"
#include "MapDriver.h"
#include "OrdersDriver.h"
#include "PlayerDriver.h"

#include "CommandProcessing.h"
#include "PlayerStrategiesDriver.h"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

/**
 * @brief Displays the menu options for the different tests.
 */
void displayMenu() {
    std::cout << "Select a section to test:" << std::endl
              << "1. Map" << std::endl
              << "2. Players" << std::endl
              << "3. Orders" << std::endl
              << "4. Cards" << std::endl
              << "5. Game Engine" << std::endl
              << "6. Command Processor" << std::endl
              << "7. Logging" << std::endl
              << "8. Strategy" << std::endl
              << "0. Quit" << std::endl
              << "Enter your choice: ";
}

/**
 * @brief Main function that runs the quick menu for tests.
 */
int main(int argc, char* argv[]) {
    // Set the seed for Random on machine current time
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (argc < 2) {
        std::cerr << "No arguments provided. Usage:" << std::endl
                  << "-console          -- Play with console input" << std::endl
                  << "-file <filename>  -- Play with file input" << std::endl
                  << "-test             -- Run the test suite" << std::endl;
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "-console" || mode == "-file") {
        // Play the game

        CommandProcessor* cp;

        if (mode == "-console") {
            // Console
            cp = new CommandProcessor();
        } else {
            // File
            if (argc < 3) {
                std::cerr << "-file requires a filename. Run without arguments to see help." << std::endl;
                return 1;
            }

            std::ifstream* commands = new std::ifstream(argv[2]);

            if (!commands) {
                std::cerr << "Given file does not exist. Run without arguments to see help." << std::endl;
                return 1;
            }

            cp = new FileCommandProcessorAdapter(commands);
        }

        Game* game = new Game(cp);
        LogObserver* observer = new LogObserver();
        game->attach(observer);
        game->startupPhase();
        game->observerPlayers(observer);
        game->mainGameLoop();
        delete game;
    } else if (mode == "-test") {
        // Test the functionality
        int choice;

        do {
            displayMenu();
            std::cin >> choice;

            switch (choice) {
            case 1:
                testLoadMaps();
                break;
            case 2:
                testPlayer();
                break;
            case 3:
                testOrdersList();
                break;
            case 4:
                testCards();
                break;
            case 5:
                std::cout << "Which test on GameEngine?" << std::endl;
                std::cout << "1. test gameStates" << std::endl;
                std::cout << "2. test startupPhase" << std::endl;
                std::cout << "3. test main play loop" << std::endl;
                std::cout << "4. test tournament" << std::endl;
                std::cin >> choice;
                if (choice == 1)
                    testGameStates();
                else if (choice == 2)
                    testStartupPhase();
                else if (choice == 3)
                    testMainGameLoop();
                else if (choice == 4)
                    testTournament();
                else
                    std::cout << "Invalid choice" << std::endl;
                break;
            case 6:
                testCommandProcessing();
                break;
            case 7:
                testLoggingObserver();
                break;
            case 8:
                std::cout << "Which strategy" << std::endl;
                std::cout << "1. Human" << std::endl;
                std::cout << "2. Aggressive" << std::endl;
                std::cout << "3. Benevolent" << std::endl;
                std::cout << "4. Neutral" << std::endl;
                std::cout << "5. Cheater" << std::endl;
                std::cin >> choice;
                if (choice == 1)
                    testHumanStrategy();
                else if (choice == 2)
                    testAggressiveStrategy();
                else if (choice == 3)
                    testNeutralStrategy();
                else if (choice == 4)
                    testNeutralStrategy();
                else if (choice == 5)
                    testCheaterStrategy();
                else
                    std::cout << "Invalid choice" << std::endl;
                break;
            case 0:
                std::cout << "Exiting the tests." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
            }
        } while (choice != 0);
    } else {
        std::cerr << "Unknown argument. Run without arguments to see help." << std::endl;
        return 1;
    }
}
