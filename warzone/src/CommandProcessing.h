#pragma once

#include <exception>
#include <istream>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "CommandProcessing.fwd.h"
#include "GameEngine.h"
#include "LoggingObserver.h"

class Command : public Subject, public ILoggable {
public:
    Command(std::string command, std::string argument);
    Command(std::string command);
    Command(const Command& other);
    ~Command();

    void saveEffect(std::string effect);

    const std::string& getCommand() const;
    const std::optional<std::string>& getArgument() const;
    const std::optional<std::string>& getEffect() const;

    Command& operator=(const Command& other);

    std::string stringToLog() const override;

    friend std::ostream& operator<<(std::ostream& out, const Command& command);

private:
    std::string command;
    std::optional<std::string> argument;
    std::optional<std::string> effect;
};

class CommandException : public std::exception {
public:
    CommandException(std::string message);
    const char* what() const noexcept override;

private:
    std::string message;
};

class CommandProcessor : public Subject, public ILoggable {
public:
    CommandProcessor() = default;
    CommandProcessor(const CommandProcessor& other);
    virtual ~CommandProcessor();

    const Command* getCommand();
    bool validate(Game::GameState state);

    CommandProcessor& operator=(const CommandProcessor& other);

    std::string stringToLog() const override;

    friend std::ostream& operator<<(std::ostream& out, const CommandProcessor& commandProcessor);

protected:
    Command* parseCommand(std::string line);
    virtual Command* readCommand();
    void saveCommand(Command* command);

private:
    std::vector<Command*> commands;
};

class FileCommandProcessorAdapter : public CommandProcessor {
public:
    FileCommandProcessorAdapter(std::istream* stream);
    ~FileCommandProcessorAdapter();

    FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);
    friend std::ostream& operator<<(std::ostream& out, const FileCommandProcessorAdapter& commandProcessor);

protected:
    Command* readCommand() override;

private:
    std::istream* stream;
};
