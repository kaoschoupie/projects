#include "CommandProcessingDriver.h"
#include "CommandProcessing.h"
#include "GameEngine.h"

#include <iostream>
#include <sstream>

void testCommandProcessing() {
    std::cout << "Creating a command without argument" << std::endl;
    std::cout << Command("win") << std::endl;

    std::cout << "Creating a command with argument" << std::endl;
    std::cout << Command("loadmap", "mymap") << std::endl;

    std::cout << "Creating a command to assign effect" << std::endl;
    Command c("loadmap", "mymap");
    std::cout << "Before effect " << c << std::endl;
    c.saveEffect("loaded");
    std::cout << "After effect " << c << std::endl;

    std::stringstream stream(
        "loadmap mymap\n"
        "validatemap\n"
        "invalidcommand\n"
        "loadmap\n");
    FileCommandProcessorAdapter fcp(&stream);

    const Command* c1 = fcp.getCommand();
    std::cout << "Should be 'loadmap mymap', got " << *c1 << std::endl;
    fcp.validate(Game::GameState::Start);
    std::cout << "    Validating in `start` " << c1->getEffect().value_or("<< NO EFFECT >>") << std::endl;
    fcp.validate(Game::GameState::Win);
    std::cout << "    Validating in `win` " << c1->getEffect().value_or("<< NO EFFECT >>") << std::endl;

    const Command* c2 = fcp.getCommand();
    std::cout << "Should be 'validatemap', got " << *c2 << std::endl;
    fcp.validate(Game::GameState::MapLoaded);
    std::cout << "    Validating in `maploaded` " << c2->getEffect().value_or("<< NO EFFECT >>") << std::endl;
    fcp.validate(Game::GameState::Win);
    std::cout << "    Validating in `win` " << c2->getEffect().value_or("<< NO EFFECT >>") << std::endl;

    try {
        std::cout << "Should be unknown, got ";
        fcp.getCommand();
    } catch (const CommandException& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        std::cout << "Should be without argument, got ";
        fcp.getCommand();
    } catch (const CommandException& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        std::cout << "Should be no command, got ";
        fcp.getCommand();
    } catch (const CommandException& e) {
        std::cout << e.what() << std::endl;
    }

    CommandProcessor cp;
    while (true) {
        try {
            const Command* c3 = cp.getCommand();
            std::cout << *c3 << std::endl;
            if (c3->getCommand() == "quit") {
                break;
            }
        } catch (const CommandException& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
