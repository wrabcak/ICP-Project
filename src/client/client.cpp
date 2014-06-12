/**
 * @file client.cpp
 * @brief Source code for client.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */

#include "client.h"

int main(int argc, char *argv[])
{
    Client *player;
    player = new Client;
    try {
        if (argc != 2 || ((strcmp(argv[1],"-h") == 0) || (strcmp(argv[1],"--help")) == 0))
        {
            help();
            return 1;
        }

#ifndef CONSOLE
        Message::Instance().msgClient = "";
        Message::Instance().msgGUI = "";
        Message::Instance().msgCommand = "";

        Message::Instance().onlySignal = new GUISignals;

        boost::asio::detail::thread gui(boost::bind(gui_main,argc,argv));

#endif
        connect(player,argv[1],43210);
#ifndef CONSOLE
        gui.join();
#endif
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}

int connect(Client *player,std::string ip, unsigned short port)
{
    try {
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::endpoint endPoint(boost::asio::ip::address::from_string(ip), port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(endPoint);
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        char welcomeMsg[max_length] = "Ping";
        socket.send(boost::asio::buffer(welcomeMsg));

		size_t received = socket.receive(boost::asio::buffer(welcomeMsg, max_length));
		welcomeMsg[received] = '\0';
		if (strcmp(welcomeMsg,"Pong") != 0){
			std::cerr << "ERROR: Welcome message."<< std::endl;
			return 1;
 		}

        // get maps
        received = socket.receive(boost::asio::buffer(welcomeMsg, max_length));

#ifndef CONSOLE
        Message::Instance().msgClient = welcomeMsg;

        char input[1024];

        while (1) {
            if (Message::Instance().msgGUI != ""){
                strcpy(input, Message::Instance().msgGUI.c_str());
                Message::Instance().msgGUI = "";
                break;
            }

            boost::this_thread::sleep(boost::posix_time::milliseconds(16));
        }
#else
        welcomeMsg[received] = '\0';
        std::cout << welcomeMsg << std::endl;
        char input[1024];
        std::cout << "Zadaj mapu:";
        std::cin >> input;
#endif
        socket.send(boost::asio::buffer(input));

        // pride ID hraca, hry a mapa
        received = socket.receive(boost::asio::buffer(welcomeMsg, max_length));

        welcomeMsg[received] = '\0';
        if ((strcmp(welcomeMsg,"Full") == 0) || (strcmp(welcomeMsg, "Bad") == 0)){
            std::cerr << "Game is full or you type bad map number." << std::endl;
            socket.close();
            exit(0);
        }

        player->loadGameInfo(welcomeMsg);

#ifndef CONSOLE
        std::string mapString = "";
        std::stringstream stringStream;
        std::stringstream stringStream2;

        if (player->mapXSize < 10){
            mapString += "0";
        }

        stringStream << player->mapXSize;
        mapString += stringStream.str() + " ";

        if (player->mapYSize < 10){
            mapString += "0";
        }

        stringStream2 << player->mapYSize;
        mapString += stringStream2.str() + " ";

        for (int i = 0; i < player->mapXSize; i++){
            for (int j = 0; j < player->mapYSize; j++) {
                mapString += player->map[i][j];
            }
        }

        Message::Instance().msgClient = mapString;
#endif

        // Step or Start
        received = socket.receive(boost::asio::buffer(welcomeMsg, max_length));
        welcomeMsg[received] = '\0';
        if (strcmp(welcomeMsg, "Step") == 0){
            char stepTime[1024];
            memset(stepTime, 0, sizeof(stepTime));
#ifndef CONSOLE
            Message::Instance().msgCommand2 = "Step";
            
            while (1) {
                if (Message::Instance().msgGUI != ""){
                    strcpy(stepTime, Message::Instance().msgGUI.c_str());
                    Message::Instance().msgGUI = "";
                    break;
                }

                boost::this_thread::sleep(boost::posix_time::milliseconds(16));
            }
#else
            std::cout << "Type step time (0.5 - 5): ";
            std::cin >> stepTime;
            std::cout << std::endl;
#endif

            strcpy(welcomeMsg, stepTime);
            socket.send(boost::asio::buffer(welcomeMsg));
#ifndef CONSOLE
        } else{
            Message::Instance().msgCommand2 = "Start";
        }
#else
        }
#endif

#ifndef CONSOLE
        std::string command;
        std::string message;

        boost::posix_time::ptime before = boost::posix_time::second_clock::local_time();
        boost::posix_time::ptime now;
        boost::posix_time::time_duration diff;

        while (1) {
            if (Message::Instance().msgCommand != ""){
                command = Message::Instance().msgCommand;
                Message::Instance().msgCommand = "";

                memset(welcomeMsg, 0, sizeof(welcomeMsg));
                welcomeMsg[0] = player->gameID;
                welcomeMsg[1] = ' ';
                welcomeMsg[2] = player->playerID;
                welcomeMsg[3] = ' ';
                strcat(welcomeMsg, command.c_str());
                socket.send(boost::asio::buffer(welcomeMsg));
            }


            now = boost::posix_time::second_clock::local_time();
            diff = now - before;

            if (diff.total_milliseconds() > 200){
                memset(welcomeMsg, 0, sizeof(welcomeMsg));
                welcomeMsg[0] = player->gameID;
                welcomeMsg[1] = ' ';
                welcomeMsg[2] = player->playerID;
                welcomeMsg[3] = ' ';
                strcat(welcomeMsg, "sync\0");
                socket.send(boost::asio::buffer(welcomeMsg));
                received = socket.receive(boost::asio::buffer(welcomeMsg, max_length));
                welcomeMsg[received] = '\0';
                Message::Instance().onlySignal->setSignal(welcomeMsg);

                if (Message::Instance().msgGUI == "logout") {
                    Message::Instance().msgGUI = "";
                    socket.close();
                    return 0;
                }

                before = boost::posix_time::second_clock::local_time();
            }

            boost::this_thread::sleep(boost::posix_time::milliseconds(16));
        }
#else
        initscr();
        char logMsg[1024];
        memset(logMsg, 0, sizeof(logMsg));
        while (true) {
            player->printMap();
            memset(welcomeMsg, 0, sizeof(welcomeMsg));
            char command[1024];
            welcomeMsg[0] = player->gameID;
            welcomeMsg[1] = ' ';
            welcomeMsg[2] = player->playerID;
            welcomeMsg[3] = ' ';
            printw("\n\nLogs: \n%s",logMsg);
            printw("\n********************");
            printw("\n\nEnter Command: ");

            boost::posix_time::ptime before = boost::posix_time::second_clock::local_time();

            timeout(2000);
            getstr(command);

            boost::posix_time::ptime after = boost::posix_time::second_clock::local_time();
            boost::posix_time::time_duration diff = after - before;

            if (diff.total_milliseconds() < 2000){
                strcat(welcomeMsg,command);

                socket.send(boost::asio::buffer(welcomeMsg));
            } else {
                strcat(welcomeMsg,"sync");

                socket.send(boost::asio::buffer(welcomeMsg));

                socket.receive(boost::asio::buffer(welcomeMsg, max_length));

                strcpy(logMsg,welcomeMsg);

                // If True (1) returned log out.
                int status = 0;
                status = player->changeMap(welcomeMsg);

                if (status == 1){
                    socket.close();
                    break;
                }

                player->printMap();
            }
            
            
            refresh();
        }
#endif
    }
    
    catch (std::exception& e)
    {
        std::cerr << "Exception:" << e.what() << std::endl;
    }
    
    return 0;
}

void Client::loadGameInfo(char* msg)
{
    char tempArray[2];
    
    this->gameID = msg[0];
    this->playerID = msg[2];
    
    tempArray[0]=msg[4];
    tempArray[1]=msg[5];
    
    this->mapXSize = atoi(tempArray);
    
    tempArray[0] = msg[7];
    tempArray[1] = msg[8];
    
    this->mapYSize = atoi(tempArray);
    
   this->map = new char*[mapXSize];
   for (int i = 0; i < mapXSize; ++i)
        this->map[i] = new char[mapYSize];
    
   int msglen = strlen(msg);
   int w = 10;
    while (w < msglen)
    {
    for (int l = 0; l < mapXSize; l++)
        for(int j = 0; j < mapYSize; j++)
        {
            this->map[l][j] = msg[w++];
        }
    }
    
}

void Client::printMap()
{
    clear();
    for (int i = 0; i < this->mapXSize; i++)
    {
        for (int j = 0; j < this->mapYSize; j++)
            printw("%c",this->map[i][j]);
        printw("\n");
    }
    printw("\n");
}

int Client::changeMap(char* msg)
{
    int status = 0;
    if (msg[4] > '9' || msg[4] < '0'){
        if (strncmp(msg+4,"Logout",6) == 0){
            endwin();
            for (unsigned int i=11; i<strlen(msg); i++){
                if (msg[i] == ' '){
                    msg[i] = '\0';
                    break;
                }
            }
            std::cout << "Logged out.\nYou've done " << msg+11 << " steps."<< std::endl;
            return 1;

        } else if (strncmp(msg+4,"End",3) == 0){
            endwin();
            for (unsigned int i=8; i<strlen(msg); i++){
                if (msg[i] == ' '){
                    msg[i] = '\0';
                    break;
                }
            }
            std::cout << "Congratulations\nYou Won!!!!\nYou've done " << msg+8 << " steps."<< std::endl;
            return 1;

        } else if (strncmp(msg+4,"Dead",4) == 0){
            endwin();
            for (unsigned int i=9; i<strlen(msg); i++){
                if (msg[i] == ' '){
                    msg[i] = '\0';
                    break;
                }
            }
            std::cout << "Bad boy killed you.\nYou've done " << msg+9 << " steps."<< std::endl;
            return 1;
        }
    } else {

    int oldX = 0;
    int oldY = 0;
    int newX = 0;
    int newY = 0;    
    char oldSign = ' ';
    char newSign = ' ';
        
    if (msg[4] == '0')
        oldX = msg[5] - '0';
    else
        oldX = ((msg[4] -'0') * 10) + (msg[5] - '0');
    
    if (msg[7] == '0')
        oldY = msg[8] - '0';
    else
        oldY = ((msg[7] -'0') * 10) + (msg[8] - '0');

    oldSign = msg[10];

    if (msg[12] == '0')
        newX = msg[13] - '0';
    else
        newX = ((msg[12] -'0') * 10) + (msg[13] - '0');
    
    if (msg[15] == '0')
        newY = msg[16] - '0';
    else
        newY = ((msg[15] -'0') * 10) + (msg[16] - '0');    
    
    newSign = msg[18];
    
    this->map[oldX][oldY] = oldSign;
    this->map[newX][newY] = newSign;
    }

    if (strlen(msg) > 24){
        status = changeMap(&(msg[24]));
    }

    return status;
}

void help()
{
    std::cout << "Bludiste 2014 client!" << std::endl;
    std::cout << "Usage: ./bludiste2014 [IP]" << std::endl;
}
