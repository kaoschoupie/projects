#pragma once

#include "Player.fwd.h"
#include <exception>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

extern std::regex TRIM_WHITESPACE;

/**
 * @class Territory
 * @brief a class to implement a Territory object
 * @param name string: the name of the territory
 * @param armies int: the number of armies currently in the territory
 * @param owner string: the name of the player owning the territory
 * @param continent string: the name of the continent the territory is on
 * @param adjacent vector<Territory*>: a vector containing pointers to adjacent territories
 */
class Territory {
    friend class Map;
    friend class MapLoader;

private:
    std::string name;
    int armies;
    Player* owner;
    std::string continent;
    std::vector<Territory*> adjacent;

public:
    Territory();
    ~Territory();
    Territory(std::string name, int armies, Player* owner, std::string continent);
    Territory(std::string name, int armies, std::string continent);
    Territory(std::string name, std::string continent);
    Territory(std::string name);
    Territory(const Territory& other);
    Territory& operator=(const Territory& other);

    void setName(std::string name);
    void setArmies(int armies);
    void setOwner(Player* owner);
    void setContinent(std::string continent);
    std::string getName() const;
    int getArmies() const;
    Player* getOwner() const;
    std::string getContinent() const;
    const std::vector<Territory*>& getAdjacent() const;

    void prettyPrint();
    friend std::ostream& operator<<(std::ostream& out, const Territory& territory);
};

/**
 * @class Continent
 * @brief a class to implement a Continent object
 * @param armies int: the number of armies the continent awards
 * @param name string: the name of the continent
 * @param territories vector<Territory*>: a vector containing pointers to the territories in the continent
 */
class Continent {
    friend class Map;

private:
    int armies;
    std::string name;
    std::vector<Territory*> territories;

public:
    Continent();
    ~Continent();
    Continent(int armies, std::string name, std::vector<Territory*> territories);
    Continent(int armies, std::string name); // This constructor creates a vector<Territory*>
    Continent(std::string name); // This constructor creates a vector<Territory*>
    Continent(const Continent& other);
    Continent& operator=(const Continent& other);

    void setName(std::string name);
    void setArmies(int armies);

    std::string getName() const;
    int getArmies() const;
    void prettyPrint() const;
    void addTerritory(Territory* territory); // Add a territory at the end of the vector

    friend std::ostream& operator<<(std::ostream& out, const Continent& continent);
};

/**
 * @class Map
 * @brief a class to implement a Map object
 * @param territories vector<Territory*>: a vector containing pointers to the territories in the map
 * @param continents vector<Continent*>: a vector containing pointers to the continents on the map
 */
class Map {
private:
    std::vector<Territory*> territories;
    std::vector<Continent*> continents;

public:
    Map();
    ~Map();
    Map(const Map& other);
    Map& operator=(Map* other);

    std::size_t getNumberTerritories() const;
    std::size_t getNumberContinents() const;

    void addTerritory(std::string name, std::string continent);
    void addContinent(int armies, std::string name);
    /**
     * @brief to add an owner to a territory by name
     */
    void setTerritoryOwner(std::string territory, Player* player);
    /**
     * @brief overloaded to accommodate for an index in AdjL
     */
    void setTerritoryOwner(Territory* territory, Player* player);
    void addTerritoryToContinent(std::string territory, std::string continent);
    /**
     * @brief To find a territory by name
     */
    Territory* findTerritory(std::string territory);
    /**
     * @brief To find a territory by index in AdjL
     */
    Territory* findTerritoryByIndex(size_t index);

    void prettyPrintTerritories() const;
    void prettyPrintContinents() const;
    /**
     * @brief This will return the continent rewards for all continents Player p has conquered
     */
    int continentRewards(Player* player);

    void associateTerritories();
    bool validate() const;

    friend std::ostream& operator<<(std::ostream& out, const Map& map);
};

/**
 * @class ParsingException
 *
 * @brief Error that occur during the parsing process,
 * along with information about the line where the error occurred
 * and the reason for the error.
 */
class ParsingException : public std::exception {
public:
    /**
     * @brief Construct with the given line and reason.
     *
     * @param line Line number (0-indexed) where the parsing error occurred.
     * @param reason Description of the reason for the parsing error.
     */
    ParsingException(unsigned int line, std::string reason);

    const char* what() const noexcept override;

    /**
     * @brief Generate a pretty message in the stream where error occurred.
     *
     * @param stream Input stream to which was parsed.
     * Attention! Will be reset for printing.
     *
     * @return Pretty message for the error.
     */
    std::string inStream(std::istream& stream);

private:
    /** @brief Line number (0-indexed) where the parsing error occurred. */
    unsigned int line;
    /** @brief Description of the reason for the parsing error. */
    std::string reason;
    /** @brief Message describing the error (without the input stream). */
    std::string whatMessage;
};

/**
 * @class MapLoader
 *
 * @brief Utility class for parsing map files.
 */
class MapLoader {
public:
    MapLoader(std::istream& stream);
    MapLoader& operator=(const MapLoader* loader);

    /**
     * @brief Construct with the given input stream.
     *
     * @param stream Input stream from which to read map data.
     */
    Map* parse();

    friend std::ostream& operator<<(std::ostream& out, const MapLoader& mapLoader);

private:
    /** @brief Input stream from which to read map data. */
    std::istream& stream;

    /**
     * @brief Current section of the INI-like map file.
     */
    enum class Section : char {
        /** @brief Not in a section. */
        None,
        /** @brief `[Map]`. */
        Map,
        /** @brief `[Continents]`. */
        Continents,
        /** @brief `[Territories]`. */
        Territories,
    } section;
};
