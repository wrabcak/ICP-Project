/**
 * @file map.cpp
 * @brief Source code with methods used for managing map.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */
#include "map.h"

/**
 * Map's destructor
 */
Map::~Map(){
    for (int i = 0; i < Map::mapYSize; i++){
        free(mapArray[i]);
    }
    free(mapArray);
}

/**
 * Method which prints wolcome message with maps to choose and waits for player reply.
 *
 * @return Number wich indikates map (1-3) or exit (0) or bad input (-1).
 */
int Map::chooseMap(){
    std::cout << "Welcom to ICP maze.\nType:\n\"1\" for Map 1\n\"2\" for Map 2\n\"3\" for Map 3\n\"q\" for quit application" << std::endl;
    std::cout << "_________________________" << std::endl;

    int c = std::cin.get();
    switch (c) {
        case '1':
            std::cout << "Map 1" << std::endl;
            return 1;

        case '2':
            std::cout << "Map 2" << std::endl;
            return 2;

        case '3':
            std::cout << "Map 3" << std::endl;
            return 3;

        case 'e':
            std::cout << "Exit" << std::endl;
            return 0;

        default:
            std::cout << "ERROR: Unknown command." << std::endl;
            return -1;
    }
}

/**
 * Method which reads map from file and put it into 2D array.
 *
 * @param mapNumber Specifies map.
 *
 * @return Number that indikates state: OK (0) error (everything else)
 */
int Map::getMapContent(int mapNumber){
    std::string mapPath = "";
    switch (mapNumber){
        case 1:
            mapPath = "src/maps/map1.map";
            break;

        case 2:
            mapPath = "src/maps/map2.map";
            break;

        case 3:
            mapPath = "src/maps/map3.map";
            break;

        default:
            std::cout << "ERROR: Unknown map." << std::endl;
            return -1;
    }

    // Open file stream from map file
    std::ifstream fileStream;
    fileStream.open(mapPath, std::ifstream::in);

    // Temporary buffer for map dimensions
    char *tmp = (char *) malloc(sizeof(char) * 2);
    
    // Get X size
    fileStream.read(tmp, 2);
    mapXSize =  atoi(tmp);

    // Skip ":"
    fileStream.read(tmp, 1);

    // Get Y size
    fileStream.read(tmp, 2);
    mapYSize =  atoi(tmp);

    // Skip end of line
    fileStream.read(tmp, 1);



    // Allocate memory for map content
    mapArray = (char **) malloc(sizeof(char*) * mapYSize);

    for (int i = 0; i < mapXSize; i++){
        mapArray[i] = (char *) malloc(sizeof(char) * mapXSize);
    }

    for (int i = 0; i < mapYSize; i++){            
        fileStream.read(mapArray[i], mapXSize);
        fileStream.read(tmp,1); // skip EOL
    }

    // Release temporary buffer
    free(tmp);

    // close file stream
    fileStream.close();
    return 0;
}

/**
 *  Get X size of map.
 */
int Map::getMapXSize(){
    return this->mapXSize;
}
/**
 *  Get Y size of map.
 */
int Map::getMapYSize(){
    return this->mapYSize;
}
/**
 *  Load map.
 */
char **Map::getMapArray(){
    return this->mapArray;
}

/**
 * Print map content.
 */
void Map::printMap(){
    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << std::endl;
    
    for (int i = 0; i < mapYSize; i++)
        std::cout << Map::mapArray[i] << std::endl;
}

/**
 * Update sign on specific position of map
 * 
 * @param x X position.
 * @param y Y position.
 * @param sigh New sign.
 */
void Map::mapUpdate(int x, int y, char sign){
    mapArray[x][y] = sign;
}

/**
 * Return sign from specific position
 *
 * @param x X position.
 * @param y Y position.
 */
char Map::getMapSign(int x, int y){
    if (x >= mapXSize || y >= mapYSize)
        std::cerr << "MAP: out od map array" << std::endl;
    else
        return mapArray[x][y];
    return 1;
}