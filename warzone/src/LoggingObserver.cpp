#include "LoggingObserver.h"
#include <fstream>
#include <iostream>

ILoggable::ILoggable() {
}

ILoggable::~ILoggable() {
}
ILoggable::ILoggable(const ILoggable& other) {
}
ILoggable& ILoggable::operator=(const ILoggable& other) {
    return *this;
}
Observer::Observer() { }
Observer::~Observer() { }
Observer::Observer(const Observer& other) {
}
Observer& Observer::operator=(const Observer& other) {
    return *this;
}

Subject::Subject() {
    _observers = new std::list<Observer*>;
}
Subject::~Subject() {
    delete _observers;
}
Subject::Subject(const Subject& other) {
    *this = other;
}
Subject& Subject::operator=(const Subject& other) {
    if (this != &other) {

        for (size_t i = 0; i < this->_observers->size(); i++) {
            _observers[i] = other._observers[i];
        }
    }
    return *this;
}

void Subject::attach(Observer* o) {
    _observers->push_back(o);
}
void Subject::detach(Observer* o) {
    _observers->remove(o);
}
void Subject::notify(ILoggable* loggable) {
    std::list<Observer*>::iterator i = _observers->begin();
    for (; i != _observers->end(); ++i)
        (*i)->update(loggable);
}

LogObserver::LogObserver() {
}
LogObserver::~LogObserver() {
}
LogObserver::LogObserver(const LogObserver& other)
    : Observer(other) {
    *this = other;
}
LogObserver& LogObserver::operator=(const LogObserver& other) {
    if (this != &other)
        *this = other;
    return *this;
}
void LogObserver::update(ILoggable* loggable) {
    std::cout << "UPDATING" << std::endl;
    std::ofstream file;
    file.open(logFile, std::ios_base::app);
    file << loggable->stringToLog() << std::endl;
    file.close();
}
