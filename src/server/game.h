/**
 * @file game.h
 * @brief Header file for game logic part.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */

#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <ctime>
#include "map.h"
#include "server.h"
#include "boost/date_time/posix_time/posix_time.hpp"

#define MAX_PLAYERS 4
#define MAX_PLAYERS_WITH_BAD_BOY 5

// struct for store player actual information
/**
 * @struct TPlayerInfo
 * @brief Struct for store player actual information
 */
typedef struct {
	bool active; // if is player active set 1 else 0
 	int direction; ///< Player's direction. (1 - up, 2 - right, 3 - down, 4 - left) >
	int posX;  // X player position 
	int posY; // Y player position
 	int keyNumber; // if player own key set 1 else 0
	int stepNumber; // step num
    boost::posix_time::ptime lastMove;
} TPlayerInfo;

/**
 * @Class Game 
 * @brief Class with games's variables and methods
 */
class Game
{
	public:
		Game();
		~Game();
		
		int loadMap(int mapNumber); //load map param. mapNumber(1,2,3)
		int getMapXSize(); 
		int getMapYSize();
        char getMapSign(int x, int y); // get symbol from map. x,y => position
        void printMap();
        int getMapNumber(); //get Map ID

		int getAvailablePlayerNumber(); // get first free number for player
		int initPlayer(); 
        int initBadBoy(); // init "hlidac"
		TCommands deletePlayer(int playerNumber);
        TPlayerInfo getPlayer(int playerNumber);

        int getGameNumber();
        void setGameNumber(int gameNumber);

        char setPlayerPosition(int playerNumber, int posX, int posY);
        TCommands movePlayer(int playerNumber);
        TCommands turnPlayer(int playerNumber, int side);
        TCommands takeKey(int playerNumber);
        TCommands openGate(int playerNumber);

		void startGame();
		void endGame();
        TPlayerInfo playerInfo[MAX_PLAYERS_WITH_BAD_BOY];

        double stepTime;

	private:
        int gameNumber; // actial game ID
		int mapNumber; // actual map ID
		Map *map; // map pointer
};



#endif // GAME_H
