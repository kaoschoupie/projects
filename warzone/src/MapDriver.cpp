#include "Map.h"

#include <fstream>
#include <iostream>
#include <regex>

void testLoadMaps() {
    std::ifstream files("./res/map/files.txt");
    std::string line;
    while (std::getline(files, line)) {
        line = std::regex_replace(line, TRIM_WHITESPACE, "");
        std::ifstream map(line);
        Map* obj;
        if (!map.is_open()) {
            std::cerr << "File " << line << " not found" << std::endl;
            return;
        }

        try {
            obj = MapLoader(map).parse();
        } catch (ParsingException& e) {
            std::cout << e.inStream(map) << std::endl;
        }
        if (!obj->validate()) {
            std::cout << "Invalid map, getting rid of it." << std::endl;
            delete obj;
        }
    }
}
