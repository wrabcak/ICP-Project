/**
 * @file game.cpp
 * @brief Source code for game logic part.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */

#include "game.h"

Game::Game(){
	for (int playerNumber = 0; playerNumber < MAX_PLAYERS; playerNumber++)
		playerInfo[playerNumber].active = false;

	this->map = NULL;
    this->playerInfo[4].active = false;
}

Game::~Game(){
	if (this->map != NULL)
		delete this->map;
}

int Game::loadMap(int mapNumber){
	this->mapNumber = mapNumber;
	this->map = new Map();
	this->map->getMapContent(mapNumber);
	return 0;
}

int Game::getMapXSize(){
	return this->map->getMapXSize();
}

int Game::getMapYSize(){
	return this->map->getMapYSize();
}

char Game::getMapSign(int x, int y){
	return this->map->getMapSign(x, y);
}

int Game::getGameNumber(){
    return this->gameNumber;
}

void Game::setGameNumber(int gameNumber){
    this->gameNumber = gameNumber;
}

int Game::getMapNumber(){
    return this->mapNumber;
}

void Game::printMap(){
    std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << std::endl;

    for (int i = 0; i < getMapXSize(); i++){
        for (int j = 0; j < getMapYSize(); j++)
            std::cout << getMapSign(i,j);
        std::cout << std::endl;
    }

    std::cout << std::endl;

}

int Game::getAvailablePlayerNumber(){
	for (int playerNumber = 0; playerNumber < MAX_PLAYERS; playerNumber++)
		if (playerInfo[playerNumber].active == false)
			return playerNumber;

	return -1; // There's no space for next player
}

int Game::initPlayer(){
	int playerNumber = getAvailablePlayerNumber();
	if (playerNumber == -1)
		return -1;

    playerInfo[playerNumber].active = true;
	playerInfo[playerNumber].direction = 1;
	playerInfo[playerNumber].posX = 23;
	playerInfo[playerNumber].posY = 1;
	playerInfo[playerNumber].keyNumber = 0;
	playerInfo[playerNumber].stepNumber = 0;

    setPlayerPosition(playerNumber, playerInfo[playerNumber].posX, playerInfo[playerNumber].posY);

	return playerNumber;
}

int Game::initBadBoy(){
    int playerNumber = 4;

    playerInfo[playerNumber].active = true;
	playerInfo[playerNumber].direction = 2;
	playerInfo[playerNumber].posX = 17;
	playerInfo[playerNumber].posY = 5;
    playerInfo[playerNumber].lastMove = boost::posix_time::second_clock::local_time();

    setPlayerPosition(playerNumber, playerInfo[playerNumber].posX, playerInfo[playerNumber].posY);

	return playerNumber;
}

TCommands Game::deletePlayer(int playerNumber){
    this->map->mapUpdate(this->playerInfo[playerNumber].posX,this->playerInfo[playerNumber].posY,' ');

    TCommands deleted;
    deleted.oldX = this->playerInfo[playerNumber].posX;
    deleted.oldY = this->playerInfo[playerNumber].posY;
    deleted.newX = this->playerInfo[playerNumber].posX;
    deleted.newY = this->playerInfo[playerNumber].posY;
    deleted.oldSign = ' ';
    deleted.newSign = ' ';

	playerInfo[playerNumber].active = false;

    return deleted;
}

TPlayerInfo Game::getPlayer(int playerNumber){
    return this->playerInfo[playerNumber];
}

void Game::startGame(){

}

void Game::endGame(){

}

/**
 * Method for setting (updating) player's position
 *
 * Invoking method which updates player on map.
 *
 * @param x X position
 * @param y Y position
 */
char Game::setPlayerPosition(int playerNumber, int posX, int posY){
    this->playerInfo[playerNumber].posX = posX;
    this->playerInfo[playerNumber].posY = posY;
    char sign = '?';

    // Set how will be player represented
    switch(this->playerInfo[playerNumber].direction){
        case 1:
            sign = '^';
            break;

        case 2:
            sign = '>';
            break;

        case 3:
            sign = 'v';
            break;

        case 4:
            sign = '<';
            break;
    }

    if (playerNumber > 3){
        sign = (playerNumber - 3) + '0';
    }
    
    this->map->mapUpdate(posX,posY,sign);

    return sign;
}

/**
 * Method which moves player on map.
 *
 * Check direction and if there is an empty space on the next field then move player and clear previous field.
 */
TCommands Game::movePlayer(int playerNumber){
    char sign = '\0';
    TCommands returnValue;
    returnValue.oldX = -11; // Default

    switch(this->playerInfo[playerNumber].direction){
        case 1:
            sign = this->map->getMapSign(this->playerInfo[playerNumber].posX - 1, this->playerInfo[playerNumber].posY);

            if (sign == ' '){
                // clear previous position
                this->map->mapUpdate(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY, ' ');

                returnValue.oldX = this->playerInfo[playerNumber].posX;
                returnValue.oldY = this->playerInfo[playerNumber].posY;
                returnValue.oldSign = ' ';

                // move player
                setPlayerPosition(playerNumber, this->playerInfo[playerNumber].posX - 1, this->playerInfo[playerNumber].posY);

                returnValue.newX = this->playerInfo[playerNumber].posX;
                returnValue.newY = this->playerInfo[playerNumber].posY;
                returnValue.newSign = '^';


                if (playerNumber > 3){
                    returnValue.newSign = (playerNumber - 3) + '0';
                }

                this->playerInfo[playerNumber].stepNumber++;

                return returnValue;
            }
            break;

        case 2:
            sign = this->map->getMapSign(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY + 1);

            if (sign == ' '){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY, ' ');
                returnValue.oldX = this->playerInfo[playerNumber].posX;
                returnValue.oldY = this->playerInfo[playerNumber].posY;
                returnValue.oldSign = ' ';

                setPlayerPosition(playerNumber, this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY + 1);

                returnValue.newX = this->playerInfo[playerNumber].posX;
                returnValue.newY = this->playerInfo[playerNumber].posY;
                returnValue.newSign = '>';

                if (playerNumber > 3){
                    returnValue.newSign = (playerNumber - 3) + '0';
                }

                this->playerInfo[playerNumber].stepNumber++;

                return returnValue;
            }
            break;

        case 3:
            sign = this->map->getMapSign(this->playerInfo[playerNumber].posX + 1, this->playerInfo[playerNumber].posY);

            if (sign == ' '){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY, ' ');

                returnValue.oldX = this->playerInfo[playerNumber].posX;
                returnValue.oldY = this->playerInfo[playerNumber].posY;
                returnValue.oldSign = ' ';

                setPlayerPosition(playerNumber, this->playerInfo[playerNumber].posX + 1, this->playerInfo[playerNumber].posY);

                returnValue.newX = this->playerInfo[playerNumber].posX;
                returnValue.newY = this->playerInfo[playerNumber].posY;
                returnValue.newSign = 'v';

                if (playerNumber > 3){
                    returnValue.newSign = (playerNumber - 3) + '0';
                }

                this->playerInfo[playerNumber].stepNumber++;

                return returnValue;
            }
            break;

        case 4:
            sign = this->map->getMapSign(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY - 1);

            if (sign == ' '){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY, ' ');

                returnValue.oldX = this->playerInfo[playerNumber].posX;
                returnValue.oldY = this->playerInfo[playerNumber].posY;
                returnValue.oldSign = ' ';

                setPlayerPosition(playerNumber, this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY - 1);

                returnValue.newX = this->playerInfo[playerNumber].posX;
                returnValue.newY = this->playerInfo[playerNumber].posY;
                returnValue.newSign = '<';

                if (playerNumber > 3){
                    returnValue.newSign = (playerNumber - 3) + '0';
                }

                this->playerInfo[playerNumber].stepNumber++;

                return returnValue;
            }
            break;
    }

    if (playerNumber > 3){
        if (sign == '^' || sign == '>' || sign == 'v' || sign == '<'){
            returnValue.oldX = - 12;
            
            return returnValue;
        }
    } else {
        if (sign > '0' && sign < '9'){
            returnValue.oldX = -13;
            returnValue.newX = this->playerInfo[playerNumber].posX;
            returnValue.newY = this->playerInfo[playerNumber].posY;
            returnValue.newSign = ' ';
            this->playerInfo[playerNumber].stepNumber++;

            return returnValue;
        }
    }

    if (sign == '@'){
        returnValue.oldX = -14;
        this->playerInfo[playerNumber].stepNumber++;

        return returnValue;
    }
    
    return returnValue;
}

/**
 * Method which turns player.
 *
 * @param side Variable determinates turning side ( -1 - left, 1 - right )
 */
TCommands Game::turnPlayer(int playerNumber, int side){
    TCommands returnValue;

    this->playerInfo[playerNumber].direction = (this->playerInfo[playerNumber].direction + side) % 4;
    if (this->playerInfo[playerNumber].direction == 0)
        this->playerInfo[playerNumber].direction = 4;

    char sign = setPlayerPosition(playerNumber, this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY);
    returnValue.oldX = this->playerInfo[playerNumber].posX;
    returnValue.oldY = this->playerInfo[playerNumber].posY;
    returnValue.oldSign = sign;
    returnValue.newX = this->playerInfo[playerNumber].posX;
    returnValue.newY = this->playerInfo[playerNumber].posY;
    returnValue.newSign = sign;

    this->playerInfo[playerNumber].stepNumber++;

    return returnValue;
}

/**
 * Method which takes key.
 */
TCommands Game::takeKey(int playerNumber){
    TCommands returnValue;
    returnValue.oldX = -1; // Default
    bool taked = false;

    switch(this->playerInfo[playerNumber].direction){
        case 1:
            if (this->map->getMapSign(this->playerInfo[playerNumber].posX - 1, this->playerInfo[playerNumber].posY) == '$'){
                // clear next position
                this->map->mapUpdate(this->playerInfo[playerNumber].posX - 1, this->playerInfo[playerNumber].posY, ' ');

                returnValue.oldX = this->playerInfo[playerNumber].posX - 1;
                returnValue.oldY = this->playerInfo[playerNumber].posY;

                //take key
                this->playerInfo[playerNumber].keyNumber++;

                taked = true;
            }
            break;

        case 2:
            if (this->map->getMapSign(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY + 1) == '$'){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY + 1, ' ');
                this->playerInfo[playerNumber].keyNumber++;

                returnValue.oldX = this->playerInfo[playerNumber].posX;
                returnValue.oldY = this->playerInfo[playerNumber].posY + 1;


                taked = true;
            }
            break;

        case 3:
            if (this->map->getMapSign(this->playerInfo[playerNumber].posX + 1, this->playerInfo[playerNumber].posY) == '$'){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX + 1, this->playerInfo[playerNumber].posY, ' ');
                this->playerInfo[playerNumber].keyNumber++;

                returnValue.oldX = this->playerInfo[playerNumber].posX + 1;
                returnValue.oldY = this->playerInfo[playerNumber].posY;

                taked = true;
            }
            break;

        case 4:
            if (this->map->getMapSign(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY - 1) == '$'){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY - 1, ' ');
                this->playerInfo[playerNumber].keyNumber++;

                returnValue.oldX = this->playerInfo[playerNumber].posX;
                returnValue.oldY = this->playerInfo[playerNumber].posY - 1;

                taked = true;
            }
            break;
    }

    if (taked){
        returnValue.oldSign = ' ';
        returnValue.newX = returnValue.oldX;
        returnValue.newY = returnValue.oldY;
        returnValue.newSign = ' ';
        this->playerInfo[playerNumber].stepNumber++;
    }

    return returnValue;
}

/**
 * Method which opens gate.
 */
TCommands Game::openGate(int playerNumber){
    TCommands returnValue;
    returnValue.oldX = -1; // Default
    
    bool opened = false;

    if (this->playerInfo[playerNumber].keyNumber == 0)
        return returnValue;

    switch(this->playerInfo[playerNumber].direction){
        case 1:
            if (this->map->getMapSign(this->playerInfo[playerNumber].posX - 1, this->playerInfo[playerNumber].posY) == '='){
                // clear next position
                this->map->mapUpdate(this->playerInfo[playerNumber].posX - 1, this->playerInfo[playerNumber].posY, ' ');

                returnValue.oldX = this->playerInfo[playerNumber].posX - 1;
                returnValue.oldY = this->playerInfo[playerNumber].posY;


                //drop key
                this->playerInfo[playerNumber].keyNumber--;

                opened = true;
            }
            break;

        case 2:
            if (this->map->getMapSign(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY + 1) == '='){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY + 1, ' ');
                this->playerInfo[playerNumber].keyNumber--;

                returnValue.oldX = this->playerInfo[playerNumber].posX;
                returnValue.oldY = this->playerInfo[playerNumber].posY + 1;

                opened = true;
            }
            break;

        case 3:
            if (this->map->getMapSign(this->playerInfo[playerNumber].posX + 1, this->playerInfo[playerNumber].posY) == '='){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX + 1, this->playerInfo[playerNumber].posY, ' ');
                this->playerInfo[playerNumber].keyNumber--;

                returnValue.oldX = this->playerInfo[playerNumber].posX + 1;
                returnValue.oldY = this->playerInfo[playerNumber].posY;

                opened = true;
            }
            break;

        case 4:
            if (this->map->getMapSign(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY - 1) == '='){
                this->map->mapUpdate(this->playerInfo[playerNumber].posX, this->playerInfo[playerNumber].posY - 1, ' ');
                this->playerInfo[playerNumber].keyNumber--;

                returnValue.oldX = this->playerInfo[playerNumber].posX;
                returnValue.oldY = this->playerInfo[playerNumber].posY - 1;

                opened = true;
            }
            break;
    }

    if (opened){
        returnValue.oldSign = ' ';
        returnValue.newX = returnValue.oldX;
        returnValue.newY = returnValue.oldY;
        returnValue.newSign = ' ';
        this->playerInfo[playerNumber].stepNumber++;
    }

    return returnValue;
}
