/**
 * @file engine.h
 * @brief Header file for server engine part.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */
#ifndef ENGINE_H
#define ENGINE_H

#include "server.h"
#include "game.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <list>
#include "boost/date_time/posix_time/posix_time.hpp"

/**
 * @struct TgoPlayers
 * @brief struct to store information about go step
 */
typedef struct {
    int gameNumber;
    int playerNumber;
    boost::posix_time::ptime lastMove;
} TgoPlayers;

/**
 * @fn void engine_main();
 * @brief Starting game engine.
 */
void engine_main();

/**
 * @fn int checkGameNumber(int gameNumber);
 * @brief checking game ID.
 * @param gameNumber Number of game. 
 * @return Zero if successfull, 1 if not.
 */
int checkGameNumber(int gameNumber);

/**
 * @fn void generateMessage(Game *game, TCommands changedMapElement);
 * @brief Generate message for clients.
 * @param game object of game.
 * @param changedMapElement structure witch changes. 
 */
void generateMessage(Game *game, TCommands changedMapElement, int playerNumber);

void generateMessageEnd(Game *game, int playerNumber);

int findDead(Game *game);
/**
 * @fn bool isPlayerInList(std::list<TgoPlayers> *goPlayers, int gameNumber, int playerNumber);
 * @brief Checking if client is in List.
 * @param *goPlayers List of players/clients
 * @param gameNumber Actual game ID 
 * @param playerNumber Actual player ID 
 */
bool isPlayerInList(std::list<TgoPlayers> *goPlayers, int gameNumber, int playerNumber);

/**
 * @fn void deletePlayerFromList(std::list<TgoPlayers> *goPlayers, int gameNumber, int playerNumber);
 * @brief Delete client from List.
 * @param *goPlayers List of players/clients
 * @param gameNumber Actual game ID 
 * @param playerNumber Actual player ID 
 */
void deletePlayerFromList(std::list<TgoPlayers> *goPlayers, int gameNumber, int playerNumber);

void processGo(Game **games, std::list<TgoPlayers> *goPlayers);

/**
 * @fn void badBoy(Game **games, int size);
 * @brief Hlidac.
 * @param 
 * @param playerNumber Actual player ID 
 */
void badBoy(Game **games, int size);

#endif