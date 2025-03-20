#include "Map.h"
#include "Player.h"
#include <iomanip>
#include <iostream>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// default delegated constructor
Territory::Territory()
    : Territory("", 0, nullptr, "") {
}
// destructor
Territory::~Territory() {
    for (size_t i = 0; i < this->adjacent.size(); i++) {
        this->adjacent[i] = nullptr;
    }
}
// full parametrized constructor
Territory::Territory(std::string name, int armies, Player* owner, std::string continent)
    : name(name)
    , armies(armies)
    , owner(owner)
    , continent(continent)

{
} // no owner parametrized delegated constructor
Territory::Territory(std::string name, int armies, std::string continent)
    : Territory(name, armies, nullptr, continent) {
}
// no owner no armies parametrized delegated constructor
Territory::Territory(std::string name, std::string continent)
    : Territory(name, 0, nullptr, continent) {
}
// special constructor for parsing
Territory::Territory(std::string name)
    : Territory(name, -1, nullptr, "") {
}
// copy constructor
Territory::Territory(const Territory& other)
    : Territory(other.name, other.armies, other.owner, other.continent) {
}
// territory assignment operator
Territory& Territory::operator=(const Territory& other) {
    name = other.name;
    armies = other.armies;
    owner = other.owner;
    continent = other.continent;
    adjacent = other.adjacent;
    return *this;
}
// setter
void Territory::setName(std::string name) {
    this->name = name;
}
// setter
void Territory::setArmies(int armies) {
    this->armies = armies;
}
// setter
void Territory::setOwner(Player* owner) {
    this->owner = owner;
}
// setter
void Territory::setContinent(std::string continent) {
    this->continent = continent;
}
// getter
std::string Territory::getName() const {
    return this->name;
}
// getter
int Territory::getArmies() const {
    return this->armies;
}
// getter
Player* Territory::getOwner() const {
    return this->owner;
}
// getter
std::string Territory::getContinent() const {
    return this->continent;
}

const std::vector<Territory*>& Territory::getAdjacent() const {
    return this->adjacent;
}
// print the territory and adjacent territories
void Territory::prettyPrint() {
    std::cout << *this << std::endl;
    std::cout << "The adjacent territories are: ";
    for (size_t i = 0; i < this->adjacent.size(); i++) {
        std::cout << this->adjacent[i]->name << ", ";
    }
    std::cout << std::endl;
}

// territory stream insertion operator
std::ostream& operator<<(std::ostream& out, const Territory& territory) {
    return out
        << "This is " << territory.name
        << " containing " << territory.armies
        << " armies, owned by " << *territory.owner
        << " on the continent " << territory.continent;
}

// fully parametrized constructor
Continent::Continent(int armies, std::string name, std::vector<Territory*> territories)
    : armies(armies)
    , name(name)
    , territories(territories) {
}
// continent destructor
Continent::~Continent() {
    for (size_t i = 0; i < this->territories.size(); i++) {
        this->territories[i] = nullptr;
    }
}
// no vector constructor
Continent::Continent(int armies, std::string name)
    : armies(armies)
    , name(name) {
}
// no vector, no armies delegated constructor
Continent::Continent(std::string name)
    : Continent(0, name) {
}
// default delegated constructor
Continent::Continent()
    : Continent(0, "") {
}
Continent::Continent(const Continent& other)
    : Continent(other.armies, other.name, other.territories) {
}
// continent assignment operator
Continent& Continent::operator=(const Continent& other) {
    name = other.name;
    armies = other.armies;
    territories = other.territories;
    return *this;
}
// setter
void Continent::setName(std::string name) {
    this->name = name;
}
// setter
void Continent::setArmies(int armies) {
    this->armies = armies;
}
// getter
std::string Continent::getName() const {
    return this->name;
}
// getter
int Continent::getArmies() const {
    return this->armies;
}
// prints the continent and the territories in it
void Continent::prettyPrint() const {
    std::cout << *this << " It contains the following territories:" << std::endl;
    for (size_t i = 0; i < this->territories.size(); i++) {
        this->territories[i]->prettyPrint();
    }
    std::cout << std::endl;
}

// add a territory to a continent
void Continent::addTerritory(Territory* territory) {
    this->territories.push_back(territory);
}
// continent stream insertion operator
std::ostream& operator<<(std::ostream& out, const Continent& continent) {
    return out
        << "This is the continent " << continent.name
        << " awarding " << continent.armies << " armies.";
}
// only map constructor
Map::Map() {
}
// map destructor
Map::~Map() {
    for (size_t i = 0; i < this->territories.size(); i++) {
        delete this->territories[i];
    }
    territories.clear();

    for (size_t i = 0; i < this->continents.size(); i++) {
        delete this->continents[i];
    }
    continents.clear();
}
// copy constructor
Map::Map(const Map& other)
    : Map() {
    for (auto t : other.territories) {
        this->territories.push_back(new Territory(*t));
    }
    for (auto c : other.continents) {
        this->continents.push_back(new Continent(c->getArmies(), c->getName()));
    }
    this->associateTerritories();
    for (size_t i = 0; i < this->getNumberTerritories(); i++) {
        this->addTerritoryToContinent(this->territories[i]->getName(), this->territories[i]->getContinent());
    }
}
// map assignment operator
Map& Map::operator=(Map* map) {
    if (this != map) {
        territories = map->territories;
        continents = map->continents;
    }
    return *this;
}

std::size_t Map::getNumberTerritories() const {
    return territories.size();
}
std::size_t Map::getNumberContinents() const {
    return continents.size();
}

// adds a territory to the map
void Map::addTerritory(std::string name, std::string continent) {
    this->territories.push_back(new Territory(name, continent)); // TODO: add parameters in the constructor
}

// adds a continent to the map
void Map::addContinent(int armies, std::string name) {
    this->continents.push_back(new Continent(armies, name)); // TODO: add parameters in the constructor
}
// modify a territory's owner in the map
void Map::setTerritoryOwner(std::string territory, Player* owner) {
    Territory* territoryPtr = findTerritory(territory);
    if (territoryPtr) {
        territoryPtr->setOwner(owner);
    }
}
/**
 * @overload setTerritoryOwner
 */
void Map::setTerritoryOwner(Territory* territory, Player* owner) {
    territory->setOwner(owner);
}
// adds a territory to a continent in the map, the territory is supposed to already exists in Map's territory vector
void Map::addTerritoryToContinent(std::string territory, std::string continent) {
    if (!continents.front() || !territories.front()) {
        std::cout << "No continent or territory exists, cannot add territory to a continent" << std::endl;
        return;
    }
    Territory* territoryPtr = findTerritory(territory);
    if (!territoryPtr) {
        std::cout << "Did not find territory " << territory << " in the list of territories" << std::endl;
        return;
    }
    for (size_t j = 0; j < this->continents.size(); ++j) {
        if (continents[j]->getName() == continent) {
            continents[j]->addTerritory(territoryPtr);
            std::cout << "Successfully added " << territoryPtr->getName() << " territory to " << continent << " continent to the map" << std::endl;
            std::cout << std::endl;
            return;
        }
    }
    std::cout << "Did not find continent " << continent << " in the list of continents" << std::endl; // TODO: this should be throwing an error instead of printing
    // error should be handled to announce that the .map file is not forming a valid map
}
// find a territory in the map, returns a ptr to the territory(nullptr if not)
Territory* Map::findTerritory(std::string territory) {
    for (size_t i = 0; i < this->territories.size(); i++) {
        if (this->territories[i]->getName() == territory) {
            Territory* ptr = territories[i];
            return ptr;
        }
    }
    return nullptr;
}

Territory* Map::findTerritoryByIndex(size_t index) {
    return this->territories[index];
}

// print all territories in the map
void Map::prettyPrintTerritories() const {
    for (size_t i = 0; i < this->territories.size(); i++) {
        if (this->territories[i])
            this->territories[i]->prettyPrint();
    }
}
// print all continents in the map
void Map::prettyPrintContinents() const {
    for (size_t i = 0; i < this->continents.size(); i++) {
        if (this->continents[i])
            this->continents[i]->prettyPrint();
    }
}

int Map::continentRewards(Player* p) {
    int reinforcements = 0;
    for (size_t i = 0; i < this->getNumberContinents(); i++) {
        bool condition = true;
        for (size_t j = 0; j < this->continents[i]->territories.size(); j++) {
            if (this->continents[i]->territories[j]->getOwner() != p) {
                condition = false;
            }
        }
        if (condition) {
            reinforcements += this->continents[i]->getArmies();
        }
    }
    return reinforcements;
}

// used in parsing, replaces each temporary territory present in each actual territory's adjacency vector with the proper territory
void Map::associateTerritories() {
    for (size_t i = 0; i < this->territories.size(); i++) {
        if (this->territories[i] != nullptr) {
            for (size_t j = 0; j < this->territories[i]->adjacent.size(); ++j) {
                for (size_t k = 0; k < territories.size(); ++k) {
                    if (territories[k]->getName() == territories[i]->adjacent[j]->getName()) {
                        delete territories[i]->adjacent[j];
                        territories[i]->adjacent[j] = territories[k];
                        break;
                    }
                }
            }
        }
    }
}
// validation method for a completed map object, checks territory adjacency and ownership by continent
// because of the parser's implementation, it is useless to check for a territory in multiple continents
bool Map::validate() const {
    size_t adjLsize = this->territories.size();
    if (adjLsize == 0) {
        std::cout << "This map is empty, it is not validated." << std::endl;
        return false;
    }
    std::cout << "Validating main graph structure" << std::endl;
    std::vector<size_t> visitedTerrs;
    visitedTerrs.assign(adjLsize, 0);
    for (size_t i = 0; i < adjLsize; i++) {
        if (!this->territories[i])
            return false;
        for (size_t j = 0; j < this->territories[i]->adjacent.size(); ++j) {
            for (size_t k = 0; k < adjLsize; ++k) {
                if (territories[k] == territories[i]->adjacent[j]) {
                    visitedTerrs.at(k)++;
                    break;
                }
            }
        }
    }

    for (size_t l = 0; l < visitedTerrs.size(); ++l) {
        std::cout << "Territory" << this->territories[l]->getName() << ": " << visitedTerrs[l] << " visits vs " << territories[l]->adjacent.size() << " expected visits" << std::endl;
        if (visitedTerrs[l] != territories[l]->adjacent.size() || visitedTerrs[l] == 0) {
            return false;
        }
    }
    std::cout << "Map's main graph is connected" << std::endl
              << std::endl;

    std::cout << "Validating continent subgraphs" << std::endl;
    size_t conTerritories = 0;
    for (size_t i = 0; i < this->continents.size(); i++) {
        if (!this->continents[i])
            return false;
        conTerritories += this->continents[i]->territories.size();
    }
    std::cout << "Continents have a total of " << conTerritories << " territories linked and expected a total of " << adjLsize << " territories in the main graph" << std::endl;
    if (conTerritories != adjLsize) {
        return false;
    }
    std::cout << "Map's continent subgraphs are validated" << std::endl;
    std::cout << "Because of the parser's implementation, it is impossible for any territory to have more than one continent." << std::endl;
    return true;
}
// map stream operator
std::ostream& operator<<(std::ostream& out, const Map& map) {
    return out
        << "this map contains " << map.getNumberTerritories()
        << " territories and " << map.getNumberContinents() << " continents.";
}

std::regex TRIM_WHITESPACE = std::regex("^\\s+|\\s+$");

ParsingException::ParsingException(unsigned int line, std::string reason)
    : line(line)
    , reason(reason) {
    std::ostringstream whatMessage;
    whatMessage << this->reason << " on line " << this->line;
    this->whatMessage = whatMessage.str();
}

std::string ParsingException::inStream(std::istream& stream) {
    stream.clear();
    stream.seekg(0);

    unsigned int lineNumber = 0;
    for (std::string line; std::getline(stream, line);) {
        if (this->line == lineNumber++) {
            // Found line with correct line number
            std::ostringstream message;
            message
                << this->reason
                << ":\n"
                << std::setfill(' ')
                << std::setw(4)
                << this->line
                << " | "
                << line;

            return message.str();
        }
    }

    throw ParsingException(this->line, "Line not found");
}

const char* ParsingException::what() const noexcept {
    return this->whatMessage.c_str();
}

MapLoader::MapLoader(std::istream& stream)
    : stream(stream)
    , section(Section::None) { }

MapLoader& MapLoader::operator=(const MapLoader* loader) {
    if (this != loader) {
        this->stream.rdbuf(loader->stream.rdbuf());
        this->section = loader->section;
    }
    return *this;
}

Map* MapLoader::parse() {
    int lineNumber = -1;

    /** @brief Throw an exception with current line number. */
    auto fail = [&lineNumber](std::string message) {
        throw ParsingException(lineNumber, message);
    };
    Map* mapObj = new Map();
    for (std::string line; std::getline(this->stream, line);) {
        lineNumber++;

        // Trim spaces
        line = std::regex_replace(line, TRIM_WHITESPACE, "");

        // Skip empty
        if (line.empty()) {
            continue;
        }

        // Sections
        if (line.front() == '[' && line.back() == ']') {
            if (line == "[Map]") {
                this->section = Section::Map;
            } else if (line == "[Continents]") {
                this->section = Section::Continents;
            } else if (line == "[Territories]") {
                this->section = Section::Territories;
            } else {
                fail("Unknown section");
            }

            continue;
        }
        // Values
        std::istringstream lineStream(line);

        /** @brief Get some string of len > 0 until a character in the line. */
        auto getUntil = [&lineStream, &fail](char c) {
            std::string s;
            std::getline(lineStream, s, c);
            if (lineStream.eof()) {
                std::stringstream message;
                message << "Expected delimiter `" << c << "`";
                fail(message.str());
            }
            if (s.empty()) {
                std::stringstream message;
                message << "Expected something before delimiter `" << c << "`";
                fail(message.str());
            }
            return s;
        };
        /** @brief Get some string of len > 0 until the end of the line. */
        auto getRest = [&lineStream, &fail]() {
            std::string s;
            std::getline(lineStream, s);
            if (s.empty()) {
                fail("Unexpected end of line");
            }
            return s;
        };
        /** @brief Check the validity and skip the next character in the line. */
        auto expectCharacter = [&lineStream, &fail](char c) {
            char found;
            if (!(lineStream >> found)) {
                std::stringstream message;
                message << "Expected `" << c << "` found end of line";
                fail(message.str());
            } else if (found != c) {
                std::stringstream message;
                message << "Expected `" << c << "` found `" << found << "`";
                fail(message.str());
            }
        };

        if (this->section == Section::Map) {
            std::string key = getUntil('=');
            std::string value = getRest();

            if (key == "image") {
                std::cout << "Found image: " << value << std::endl;
            } else if (key == "author") {
                std::cout << "Found author: " << value << std::endl;
            } else if (key == "wrap") {
                if (value == "yes") {
                    std::cout << "Found wrap: " << value << std::endl;
                } else if (value == "no") {
                    std::cout << "Found wrap: " << value << std::endl;
                } else {
                    fail("Unexpected value for `wrap`");
                }
            } else if (key == "scroll") {
                if (value == "horizontal") {
                    std::cout << "Found scroll: " << value << std::endl;
                } else if (value == "vertical") {
                    std::cout << "Found scroll: " << value << std::endl;
                } else if (value == "none") {
                    std::cout << "Found scroll: " << value << std::endl;
                } else {
                    fail("Unexpected value for `scroll`");
                }
            } else if (key == "warn") {
                if (value == "yes") {
                    std::cout << "Found warn: " << value << std::endl;
                } else if (value == "no") {
                    std::cout << "Found warn: " << value << std::endl;
                } else {
                    fail("Unexpected value for `warn`");
                }
            } else {
                fail("Unexpected key for `[Map]` section");
            }
        } else if (this->section == Section::Continents) {
            std::string continent = getUntil('=');
            int armies;
            if (!(lineStream >> armies)) {
                fail("Expected number of armies after `=`");
            }
            if (!lineStream.eof()) {
                fail("Expected end of line after number of armies");
            }
            mapObj->addContinent(armies, continent);
            std::cout
                << "Found continent " << continent
                << " with " << armies << " armies"
                << std::endl;
        } else if (this->section == Section::Territories) {
            std::string territory = getUntil(',');
            int x;
            if (!(lineStream >> x)) {
                fail("Expected X coordinate");
            }
            expectCharacter(',');
            int y;
            if (!(lineStream >> y)) {
                fail("Expected Y coordinate");
            }
            expectCharacter(',');
            std::string country = getUntil(',');
            std::vector<std::string> adjacent;
            adjacent.reserve(10);
            for (std::string a; std::getline(lineStream, a, ',');) {
                adjacent.push_back(a);
            }
            if (adjacent.empty()) {
                fail("Expected list of adjacent territories");
            }
            mapObj->addTerritory(territory, country);
            mapObj->addTerritoryToContinent(territory, country);
            Territory* temp = mapObj->findTerritory(territory);
            std::cout
                << "Found territory " << territory
                << " at (" << x << ", " << y << ")"
                << " in country " << country
                << " adjacent to ";
            for (auto a : adjacent) {
                temp->adjacent.push_back(new Territory(a));
                std::cout << a << ", ";
            }
            std::cout << std::endl;
            temp = nullptr;
        } else {
            fail("Values started without a section");
        }
    }
    mapObj->associateTerritories();
    return mapObj;
}

std::ostream& operator<<(std::ostream& strm, const MapLoader* maploader) {
    return strm << "This MapLoader loads a file.";
}
