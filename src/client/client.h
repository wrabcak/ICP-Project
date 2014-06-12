/**
 * @file client.h
 * @brief Header File for client.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 */

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#include <curses.h>

#ifndef CONSOLE
    #include "gui.h"
    #include <boost/thread/thread.hpp>
    #include <sstream>
#endif

using boost::asio::ip::tcp;


const int max_length = 1024; // max size of msg.

/**
 * @class Client
 * @brief Class for one player, including IDs about player and game, 
 *        map sizes and whole map. 
 */
class Client
{
    public:
        unsigned char gameID; // ID of actual game
        unsigned char playerID; // player ID
        char **map; // pointer to map metrix
        int mapXSize; // map X size
        int mapYSize; // map Y size
        
        void loadGameInfo(char *msg); // Method load IDs, map and map size from msg.
        void printMap(); // Method for print Map.
        int changeMap(char *msg); // Method for changing game state

};

/**
 * @fn int connect(Client *player,std::string ip, unsigned short port)
 * @brief Connect and communicate with server.
 * @param player Pointer to object player 
 * @param ip Server IP.
 * @param port Server port.
 * @return Zero if successfull, 1 if not.
 */
int connect(Client *player,std::string ip, unsigned short port);

/**
 * @fn void help();
 * @brief Print help msg.
 */
void help();
