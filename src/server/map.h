/**
 * @file map.h
 * @brief Header file for map.cpp file.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */
#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <fstream>
#include <string>

/**
 * @Class Map 
 * @brief Class with map's variables and methods
 */
class Map
{
	public:
		~Map();
   		int chooseMap();
		int getMapContent(int mapNumber);
		int getMapXSize();
		int getMapYSize();
		char **getMapArray();
		void mapUpdate(int x, int y, char sign); // update map params: x,y -> position, sign -> symbol
		void printMap();
		char getMapSign(int x, int y); //get symbol on x,y position

	private:
		char **mapArray; ///< 2 dimensional array with map. >
		int mapXSize; ///< Map X size (rows). >
		int mapYSize; ///< Map Y size (cols). >
};



#endif // MAP_H
