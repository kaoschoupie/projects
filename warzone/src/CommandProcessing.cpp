#include "CommandProcessing.h"
#include "GameEngine.h"
#include "LoggingObserver.h"
#include "Map.h"

#include <cstddef>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>

Command::Command(std::string command, std::string argument)
    : Subject()
    , ILoggable()
    , command(command)
    , argument(argument) { }

Command::Command(std::string command)
    : Subject()
    , ILoggable()
    , command(command) { }

Command::Command(const Command& other)
    : Command(other.command) {
    this->argument = other.argument;
    this->effect = other.effect;
}
Command::~Command() { }
void Command::saveEffect(std::string effect) {
    this->effect = effect;
    notify(this);
}

const std::string& Command::getCommand() const {
    return this->command;
}

const std::optional<std::string>& Command::getArgument() const {
    return this->argument;
}

const std::optional<std::string>& Command::getEffect() const {
    return this->effect;
}

Command& Command::operator=(const Command& other) {
    this->command = other.command;
    this->argument = other.argument;
    this->effect = other.effect;
    return *this;
}

std::string Command::stringToLog() const {
    std::string argument = this->getArgument().value_or("");
    std::string effect = this->getEffect().value_or("");
    return "Command is " + this->getCommand() + " " + argument + " with the effect " + effect;
}

std::ostream& operator<<(std::ostream& out, const Command& command) {
    out << "Command " << command.command;
    if (command.argument.has_value()) {
        out << " with an argument " << command.argument.value();
    }
    if (command.effect.has_value()) {
        out << " with effect " << command.effect.value();
    }

    return out;
}

CommandException::CommandException(std::string message)
    : message(message) {
}

const char* CommandException::what() const noexcept {
    return this->message.c_str();
}

CommandProcessor::CommandProcessor(const CommandProcessor& other)
    : Subject()
    , ILoggable()

{
    for (size_t i = 0; i < other.commands.size(); i++) {
        this->commands.push_back(new Command(*other.commands[i]));
    }
}

CommandProcessor::~CommandProcessor() {
    for (size_t i = 0; i < this->commands.size(); i++) {
        delete this->commands[i];
    }
}

const Command* CommandProcessor::getCommand() {
    this->saveCommand(readCommand());
    return this->commands.back();
}

Command* CommandProcessor::parseCommand(std::string line) {
    line = std::regex_replace(line, TRIM_WHITESPACE, "");
    std::istringstream lineStream(line);
    std::string word;
    if (!(lineStream >> word)) {
        throw CommandException("No command received");
    }

    std::string command = word;
    if (
        command == "validatemap"
        || command == "gamestart"
        || command == "replay"
        || command == "quit") {
        return new Command(command);
    }
    if (
        command == "loadmap"
        || command == "addplayer"
        || command == "tournament") {
        word = "";
        std::getline(lineStream, word);
        word = std::regex_replace(word, TRIM_WHITESPACE, "");

        if (word == "") {
            throw CommandException("Command requires an argument");
        }

        return new Command(command, word);
    }
    throw CommandException("Unknown command");
}

Command* CommandProcessor::readCommand() {
    std::cout << "Type the command: ";
    std::flush(std::cout);
    std::cin.clear();
    std::cin.sync();
    std::string line = "";
    while (line == "") {
        std::getline(std::cin, line);
    }
    return this->parseCommand(line);
}

void CommandProcessor::saveCommand(Command* command) {
    this->commands.push_back(command);
    notify(this);
}

bool CommandProcessor::validate(Game::GameState state) {
    Command* command = this->commands.back();

    using S = Game::GameState;
    static const std::unordered_map<std::string, std::vector<S>> valid {
        { "loadmap", { S::Start, S::MapLoaded } },
        { "validatemap", { S::MapLoaded } },
        { "addplayer", { S::MapValidated, S::PlayersAdded } },
        { "gamestart", { S::PlayersAdded } },
        { "replay", { S::Win } },
        { "quit", { S::Win } },
        { "tournament", { S::Start } }
    };

    // Valid states for this command
    auto validStates = valid.at(command->getCommand());

    std::string argument = command->getArgument().value_or("");

    // Ugly C++ syntax to check if `state` is in `validStates`
    if (std::find(validStates.begin(), validStates.end(), state) != validStates.end()) {
        command->saveEffect("Command valid: " + command->getCommand() + " " + argument);
        return true;
    }

    command->saveEffect("Command is invalid in the current state");
    return false;
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    for (size_t i = 0; i < this->commands.size(); i++) {
        delete this->commands[i];
    }

    this->commands = {};
    for (size_t i = 0; i < other.commands.size(); i++) {
        this->commands.push_back(new Command(*other.commands[i]));
    }

    return *this;
}

std::string CommandProcessor::stringToLog() const {
    std::cout << "log commandprocessor" << std::endl;
    Command* command = this->commands.back();
    std::string output = "CommandProcessor has processed : " + command->stringToLog();
    return output;
}

std::ostream& operator<<(std::ostream& out, const CommandProcessor& commandProcessor) {
    out << "Command processor holding commands : [";
    for (size_t i = 0; i < commandProcessor.commands.size(); i++) {
        out << *commandProcessor.commands[i] << ", ";
    }
    return out << "]";
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(std::istream* stream)
    : stream(stream) { }

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete this->stream;
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
    this->stream->rdbuf(other.stream->rdbuf());
    return *this;
}

std::ostream& operator<<(std::ostream& out, const FileCommandProcessorAdapter& commandProcessor) {
    return out << "FILE" << static_cast<const CommandProcessor&>(commandProcessor);
}

Command* FileCommandProcessorAdapter::readCommand() {
    std::string line;
    std::getline(*this->stream, line);
    return this->parseCommand(line);
}
