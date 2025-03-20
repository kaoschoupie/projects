#pragma once
#include <fstream>
#include <list>
#include <string>

const std::string logFile = "gamelog.txt";

class ILoggable {
public:
    virtual ~ILoggable();
    virtual std::string stringToLog() const = 0;
    ILoggable& operator=(const ILoggable& other);

protected:
    ILoggable();
    ILoggable(const ILoggable& other);
};

class Observer {
public:
    ~Observer();
    virtual void update(ILoggable*) = 0;
    Observer& operator=(const Observer& other);

protected:
    Observer();
    Observer(const Observer& other);
};

class Subject {
public:
    virtual void attach(Observer* o);
    virtual void detach(Observer* o);
    virtual void notify(ILoggable* loggable);
    Subject();
    ~Subject();
    Subject(const Subject& other);
    Subject& operator=(const Subject& other);

private:
    std::list<Observer*>* _observers;
};

class LogObserver : public Observer {
public:
    LogObserver();
    ~LogObserver();
    LogObserver(const LogObserver& other);
    LogObserver& operator=(const LogObserver& other);
    void update(ILoggable* loggable);
};
