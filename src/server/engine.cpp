/**
 * @file engine.cpp
 * @brief Source code for server engine part.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */
#include "engine.h"

int checkGameNumber(Game **games, int size, int gameNumber){
    for (int i = 0; i < size; i++){
        if (games[i]->getGameNumber() == gameNumber)
            return i;
    }

    return -1;
}

void engine_main()
{
    srand(time(NULL));
    Game **games = (Game **) malloc(sizeof(Game) * 10);
    std::list<TgoPlayers> goPlayers;

    int i = 0;
    std::string command;
    int playerNumber;
    int gameNumber;
    int mapXSize;
    int mapYSize;

    while(1)
    {
        if (Logger::Instance().msgClient.compare(0, 9, "mapCreate") == 0){
            int choosedGameNumber = atoi(&Logger::Instance().msgClient.c_str()[9]);

            if (choosedGameNumber <= 3){
                Logger::Instance().msgClient = "";

                games[i] = new Game();
                games[i]->setGameNumber(i);
                games[i]->loadMap(choosedGameNumber);
                playerNumber = games[i]->initPlayer();
                gameNumber = games[i]->getGameNumber();
                mapXSize = games[gameNumber]->getMapXSize();
                mapYSize = games[gameNumber]->getMapYSize();

                Logger::Instance().msgChooseMap += "\"" + std::to_string(gameNumber+4) + "\" for Map" + std::to_string(games[gameNumber]->getMapNumber()) + "\n";

                std::cout << "New game " << gameNumber << " was created with map " << games[gameNumber]->getMapNumber() << std::endl;
                std::cout << "Player " << playerNumber << " logged into game " << gameNumber << std::endl;

                i++;
            } else
                {
                
                Logger::Instance().msgClient = "";

                gameNumber = checkGameNumber(games, i, choosedGameNumber - 4);

                if (gameNumber == -1){
                    Logger::Instance().msgServer = "Bad";
                    continue;
                }

                playerNumber = games[gameNumber]->initPlayer();

                if (playerNumber == -1){
                    Logger::Instance().msgServer = "Full";
                    continue;
                }

                mapXSize = games[gameNumber]->getMapXSize();
                mapYSize = games[gameNumber]->getMapYSize();

                std::cout << "Player " << playerNumber << " logged into game " << gameNumber << std::endl;

            } // if gameNumber

            // Init message -> player color
            for (int i = 0; i < MAX_PLAYERS; i++){
                if (games[gameNumber]->playerInfo[i].active == true) {
                    TCommands changeMapElement;
                    changeMapElement.oldX = games[gameNumber]->playerInfo[i].posX;
                    changeMapElement.oldY = games[gameNumber]->playerInfo[i].posY;
                    changeMapElement.newX = games[gameNumber]->playerInfo[i].posX;
                    changeMapElement.newY = games[gameNumber]->playerInfo[i].posY;
                    switch (games[gameNumber]->playerInfo[i].direction) {
                        case 1:
                            changeMapElement.oldSign = '^';
                            break;

                        case 2:
                            changeMapElement.oldSign = '>';
                            break;

                        case 3:
                            changeMapElement.oldSign = 'v';
                            break;

                        case 4:
                            changeMapElement.oldSign = '<';
                            break;
                    }
                    
                    changeMapElement.newSign = changeMapElement.oldSign;
                    
                    generateMessage(games[gameNumber], changeMapElement, i);
                }
            }


            std::string mapString;
            for (int l = 0; l < mapXSize; l++)
                for (int j = 0; j < mapYSize; j++) {
                    mapString += games[gameNumber]->getMapSign(l,j);
                }

            Logger::Instance().msgServer = "" + std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " " + std::to_string(mapXSize) + " " + std::to_string(mapYSize) + " " + mapString + "\0";

            continue;
        } //if mapcreate


        if (Logger::Instance().msgClient.compare(0, 9, "playerOut") == 0){
            int choosedGameNumber = atoi(&Logger::Instance().msgClient.c_str()[9]);
            int choosedPlayerNumber = atoi(&Logger::Instance().msgClient.c_str()[11]);
            Logger::Instance().msgClient = "";

            std::cout << "Player " << choosedPlayerNumber << " logged out from game " << choosedGameNumber << std::endl;

            TCommands changedMapElement = games[choosedGameNumber]->deletePlayer(choosedPlayerNumber);

            generateMessage(games[choosedGameNumber],changedMapElement, choosedPlayerNumber);

            continue;
        }

        if (Logger::Instance().msgClient.compare(0, 4, "step") == 0){
            std::string message = Logger::Instance().msgClient;
            Logger::Instance().msgClient = "";

            int gameNumber = atoi(&message.c_str()[5]);

            double stepTime = atof(message.c_str() + 7);
            if (stepTime < 0.5 || stepTime > 5)
                stepTime = 1;


            games[gameNumber]->stepTime = stepTime * 1000;

            games[gameNumber]->initBadBoy();

            std::cout << "Game " << gameNumber << " Step time: " << games[gameNumber]->stepTime << std::endl;

        }

        if (! Logger::Instance().commandQ.empty()){
            command = Logger::Instance().commandQ.front();
            Logger::Instance().commandQ.pop();

            gameNumber = command[0] - '0';
            playerNumber = command[2] - '0';
            command = command.substr(4,command.length() - 4);

            TCommands changedMapElement;

            if (command == "go"){

                if (isPlayerInList(&goPlayers, gameNumber, playerNumber)){
                    Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + "Already going.....");
                    continue;
                }else{
                    changedMapElement = games[gameNumber]->movePlayer(playerNumber);
                    // when player is not blocked
                    if (changedMapElement.oldX >= 0){
                        TgoPlayers newPlayer;
                        newPlayer.gameNumber = gameNumber;
                        newPlayer.playerNumber = playerNumber;
                        newPlayer.lastMove = boost::posix_time::second_clock::local_time();
                        goPlayers.push_back(newPlayer);
                    }
                }

            } else if (command == "left"){
                changedMapElement = games[gameNumber]->turnPlayer(playerNumber, -1);
            } else if (command == "right"){
                changedMapElement = games[gameNumber]->turnPlayer(playerNumber, 1);
            } else if (command == "take"){
                changedMapElement = games[gameNumber]->takeKey(playerNumber);
            } else if (command == "open"){
                changedMapElement = games[gameNumber]->openGate(playerNumber);
            }else if (command == "stop"){
                if (isPlayerInList(&goPlayers, gameNumber, playerNumber)){
                    deletePlayerFromList(&goPlayers, gameNumber, playerNumber);
                    Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " " + "Stoped..............");
                    std::cout << "Player: " << playerNumber << " Game: " << gameNumber << " Command: stop" << std::endl;
                    continue;
                }else{
                    Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " "+ "Not going...........");
                    continue;
                }
            } else if (command == "sync"){
                Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " "+ "Sync.............");
                continue;
            }else if (command == "exit"){
                std::cout << "Player: " << playerNumber << " Game: " << gameNumber << " logged out." << std::endl;
                games[gameNumber]->deletePlayer(playerNumber);
                Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " " + "Logout " + std::to_string(games[gameNumber]->playerInfo[playerNumber].stepNumber) + "\0");
                continue;
            }else {
                Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " " + "Bad..............");
                continue;
            }

            // if there is change
            if (changedMapElement.oldX >= 0){
                std::cout << "Player: " << playerNumber << " Game: " << gameNumber << " Command: " << command << std::endl;
                generateMessage(games[gameNumber],changedMapElement, playerNumber);

            } else if (changedMapElement.oldX == -11){
                std::cout << "Player: " << playerNumber << " Game: " << gameNumber << " Block"<< std::endl;

                if (isPlayerInList(&goPlayers, gameNumber, playerNumber)){
                    deletePlayerFromList(&goPlayers, gameNumber, playerNumber);
                }

                Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " " + "Block............");

            } else if (changedMapElement.oldX == -13){
                std::cout << "Player: " << playerNumber << " Game: " << gameNumber << " is dead." << std::endl;

                changedMapElement.oldX = changedMapElement.newX;
                changedMapElement.oldY = changedMapElement.newY;
                changedMapElement.oldSign = changedMapElement.newSign;

                generateMessage(games[gameNumber], changedMapElement, playerNumber);

                Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " " + "Dead " + std::to_string(games[gameNumber]->playerInfo[playerNumber].stepNumber) + "\0");

                //games[gameNumber]->deletePlayer(playerNumber);

                continue;
            } else if (changedMapElement.oldX == -14){
                std::cout << "Player: " << playerNumber << " Game: " << gameNumber << " finds end!!!" << std::endl;
                generateMessageEnd(games[gameNumber],playerNumber);
                Logger::Instance().changeMapCommandV.push_back(std::to_string(gameNumber) + " " + std::to_string(playerNumber) + " " + "End " + std::to_string(games[gameNumber]->playerInfo[playerNumber].stepNumber) + "\0");
            }

        }

        processGo(games,&goPlayers);

        badBoy(games, i);

        boost::this_thread::sleep(boost::posix_time::milliseconds(16));
    }

    free(games);
}

void badBoy(Game **games, int size){
    TCommands changedMapElement;
    std::string command;
    
    
    for (int i = 0; i < size; i++){
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        boost::posix_time::time_duration diff = now - games[i]->playerInfo[4].lastMove;
        
        if (diff.total_milliseconds() < games[i]->stepTime || games[i]->playerInfo[4].active == false)
            continue;
        
        changedMapElement = games[i]->movePlayer(4);
        command = "go";
        if (changedMapElement.oldX == -11){
            changedMapElement = games[i]->turnPlayer(4, 1);
            command = "right";
        } else if (changedMapElement.oldX == -12){
            int deadPlayerNumber = findDead(games[i]);
            if (deadPlayerNumber == -1){
                std::cout << "Dead player "<< deadPlayerNumber << " not found." << std::endl;
                return;
            }

            changedMapElement.oldX = games[i]->playerInfo[4].posX;
            changedMapElement.oldY = games[i]->playerInfo[4].posY;
            changedMapElement.newX = games[i]->playerInfo[deadPlayerNumber].posX;
            changedMapElement.newY = games[i]->playerInfo[deadPlayerNumber].posY;
            changedMapElement.oldSign = ' ';
            changedMapElement.newSign = '1';

            std::cout << "Player: " << deadPlayerNumber << " Game: " << i << " is dead." << std::endl;

            Logger::Instance().changeMapCommandV.push_back(std::to_string(i) + " " + std::to_string(deadPlayerNumber) + " " + "Dead " + std::to_string(games[i]->playerInfo[deadPlayerNumber].stepNumber) + "\0");
        }
        
        games[i]->playerInfo[4].lastMove = boost::posix_time::second_clock::local_time();

        std::cout << "Bad Boy " << " Game: " << i << " Command: " << command << std::endl;
        generateMessage(games[i],changedMapElement, 4);

    }

}

int findDead(Game *game){
    int lookingForX = -1;
    int lookingForY = -1;

    switch(game->playerInfo[4].direction){
        case 1:
            lookingForX = game->playerInfo[4].posX - 1;
            lookingForY = game->playerInfo[4].posY;
            break;

        case 2:
            lookingForX = game->playerInfo[4].posX;
            lookingForY = game->playerInfo[4].posY + 1;
            break;

        case 3:
            lookingForX = game->playerInfo[4].posX + 1;
            lookingForY = game->playerInfo[4].posY;
            break;
        case 4:
            lookingForX = game->playerInfo[4].posX;
            lookingForY = game->playerInfo[4].posY - 1;
        break;
    }



    for (int i = 0; i < MAX_PLAYERS; i++){
        if (game->playerInfo[i].active == true && lookingForX == game->playerInfo[i].posX && lookingForY == game->playerInfo[i].posY){
            TCommands changeMapElement2;
            changeMapElement2.oldX = lookingForX;
            changeMapElement2.oldY = lookingForY;
            changeMapElement2.newX = lookingForX;
            changeMapElement2.newY = lookingForY;
            changeMapElement2.oldSign = ' ';
            changeMapElement2.newSign = ' ';

            generateMessage(game, changeMapElement2, i);

            return i;
        }
    }

    return -1;
}

bool isPlayerInList(std::list<TgoPlayers> *goPlayers, int gameNumber, int playerNumber){
    for (std::list<TgoPlayers>::iterator it = goPlayers->begin() ; it != goPlayers->end(); ++it)
        if (it->gameNumber == gameNumber && it->playerNumber == playerNumber)
            return true;

    return false;
}

void deletePlayerFromList(std::list<TgoPlayers> *goPlayers, int gameNumber, int playerNumber){
    for (std::list<TgoPlayers>::iterator it = goPlayers->begin() ; it != goPlayers->end(); ++it)
        if (it->gameNumber == gameNumber && it->playerNumber == playerNumber){
            goPlayers->erase(it);
            return;
        }
}


void processGo(Game **games, std::list<TgoPlayers> *goPlayers){
    TCommands changedMapElement;

    for (std::list<TgoPlayers>::iterator it = goPlayers->begin() ; it != goPlayers->end(); ++it){
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        boost::posix_time::time_duration diff = now - it->lastMove;
        
        if (diff.total_milliseconds() >= games[it->gameNumber]->stepTime){
            changedMapElement = games[it->gameNumber]->movePlayer(it->playerNumber);
            if (changedMapElement.oldX == -11){
                std::cout << "Player: " << it->playerNumber << " Game: " << it->gameNumber << " Block"<< std::endl;
                Logger::Instance().changeMapCommandV.push_back(std::to_string(it->gameNumber) + " " + std::to_string(it->playerNumber) + " " + "Block............");

                deletePlayerFromList(goPlayers, it->gameNumber, it->playerNumber);

                processGo(games, goPlayers);
                return;
            } else if (changedMapElement.oldX == -13){
                generateMessageEnd(games[it->gameNumber],it->playerNumber);

                std::cout << "Player: " << it->playerNumber << " Game: " << it->gameNumber << " is dead." << std::endl;

                Logger::Instance().changeMapCommandV.push_back(std::to_string(it->gameNumber) + " " + std::to_string(it->playerNumber) + " " + "Dead " + std::to_string(games[it->gameNumber]->playerInfo[it->playerNumber].stepNumber) + "\0");

                deletePlayerFromList(goPlayers, it->gameNumber, it->playerNumber);
                processGo(games, goPlayers);
                return;

            } else if (changedMapElement.oldX == -14){
                std::cout << "Player: " << it->playerNumber << " Game: " << it->gameNumber << " finds end!!!" << std::endl;
                generateMessageEnd(games[it->gameNumber],it->playerNumber);
                Logger::Instance().changeMapCommandV.push_back(std::to_string(it->gameNumber) + " " + std::to_string(it->playerNumber) + " " + "End " + std::to_string(games[it->gameNumber]->playerInfo[it->playerNumber].stepNumber) + "\0");

                deletePlayerFromList(goPlayers, it->gameNumber, it->playerNumber);


                processGo(games, goPlayers);
                return;

            } else {
                generateMessage(games[it->gameNumber], changedMapElement, it->playerNumber);
                it->lastMove = boost::posix_time::second_clock::local_time();
                std::cout << "Player: " << it->playerNumber << " Game: " << it->gameNumber << " Command: go (automatic after " << diff.total_milliseconds() / 1000 << "s)" << std::endl;
            }
        }
    }

}


void generateMessageEnd(Game *game, int playerNumber){
    int gameNumber = game->getGameNumber();

    for (int i = 0; i < MAX_PLAYERS; i++){
        if (game->playerInfo[i].active == true){

            std::string messageToSend = std::to_string(gameNumber) + " " + std::to_string(i) + " ";

            if (game->playerInfo[playerNumber].posX < 10)
                messageToSend += '0' + std::to_string(game->playerInfo[playerNumber].posX) + " ";
            else
                messageToSend += std::to_string(game->playerInfo[playerNumber].posX) + " ";


            if (game->playerInfo[playerNumber].posY < 10)
                messageToSend += '0' + std::to_string(game->playerInfo[playerNumber].posY) + "   ";
            else
                messageToSend += std::to_string(game->playerInfo[playerNumber].posY) + "   ";



            if (game->playerInfo[playerNumber].posX < 10)
                messageToSend += '0' + std::to_string(game->playerInfo[playerNumber].posX) + " ";
            else
                messageToSend += std::to_string(game->playerInfo[playerNumber].posX) + " ";


            if (game->playerInfo[playerNumber].posY < 10)
                messageToSend += '0' + std::to_string(game->playerInfo[playerNumber].posY) + "  ";
            else
                messageToSend += std::to_string(game->playerInfo[playerNumber].posY) + "  ";

            messageToSend += " " + std::to_string(playerNumber);

            Logger::Instance().changeMapCommandV.push_back(messageToSend);
        }
    }
}


void generateMessage(Game *game, TCommands changedMapElement, int playerNumber){
    for (int i = 0; i < MAX_PLAYERS; i++){
        if (game->getPlayer(i).active == true){
            changedMapElement.gameNumber = game->getGameNumber();
            changedMapElement.playerNumber = i;

            std::string messageToSend = std::to_string(changedMapElement.gameNumber) + " " + std::to_string(changedMapElement.playerNumber) + " ";

            if (changedMapElement.oldX < 10)
                messageToSend += '0' + std::to_string(changedMapElement.oldX) + " ";
            else
                messageToSend += std::to_string(changedMapElement.oldX) + " ";


            if (changedMapElement.oldY < 10)
                messageToSend += '0' + std::to_string(changedMapElement.oldY) + " " + changedMapElement.oldSign + " ";
            else
                messageToSend += std::to_string(changedMapElement.oldY) + " " + changedMapElement.oldSign + " ";



            if (changedMapElement.newX < 10)
                messageToSend += '0' + std::to_string(changedMapElement.newX) + " ";
            else
                messageToSend += std::to_string(changedMapElement.newX) + " ";


            if (changedMapElement.newY < 10)
                messageToSend += '0' + std::to_string(changedMapElement.newY) + " " + changedMapElement.newSign;
            else
                messageToSend += std::to_string(changedMapElement.newY) + " " + changedMapElement.newSign;

            messageToSend += " " + std::to_string(playerNumber);

            Logger::Instance().changeMapCommandV.push_back(messageToSend);
        }
    }
}
