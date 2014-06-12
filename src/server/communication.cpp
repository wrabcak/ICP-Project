/**
 * @file communication.cpp
 * @brief Source code for server comunication part.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */
#include "communication.h"

int main()
{
    Logger::Instance().msgChooseMap = "Welcome to ICP maze.\nType:\n\"1\" for Map 1\n\"2\" for Map 2\n\"3\" for Map 3\n\"q\" for quit application\nRunning games:\n";

    // server thread for communication with clients
    boost::thread networking(boost::bind(server_main));
    // thread for game engine
    boost::thread game_engine(boost::bind(engine_main));

    networking.join();
    return 0;
}

/**
 * Main Function
 * Creates map's and player's instance and then starts the game.
 */

int server_main()
{
  try
  {
    boost::asio::io_service io_service;
    tcp_server server(io_service);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}


tcp::socket& tcp_connection::socket(){
    return socket_;
}

void tcp_connection::start_comunication(){
    this->readData = 1;
    this->endRead = 0;
    boost::asio::async_read(socket_, boost::asio::buffer(message_),
        boost::bind(&tcp_connection::handle_welcome, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}


void tcp_connection::handle_welcome(const boost::system::error_code&, size_t){
    if (strcmp(message_,"Ping") != 0)
        return;

    strcpy(message_,"Pong");

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
                             boost::bind(&tcp_connection::chooseMap, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::chooseMap(const boost::system::error_code&, size_t){
    strcpy(message_,Logger::Instance().msgChooseMap.c_str());

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
                             boost::bind(&tcp_connection::chooseMap2, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
    
}

void tcp_connection::chooseMap2(const boost::system::error_code&, size_t){
    boost::asio::async_read(socket_, boost::asio::buffer(message_),
                            boost::bind(&tcp_connection::chooseMap3, shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::chooseMap3(const boost::system::error_code&, size_t size){
    message_[size] = '\0';

    if (strlen(message_) != 1){
        std::cout << "SERVER: Wrong map number!" << std::endl;
        return;
    }


    switch (message_[0]) {
        case '0':
            break;

        case '1':
            preStartGame(1);

            return;

        case '2':
            preStartGame(2);
            return;

        case '3':
            preStartGame(3);
            return;

        case 'e':
            exit(0);
            return;

        default:
            // rozehrané hry, hledat v seznamu
            preStartGame(atoi(&message_[0]));
            break;

    }
}

void tcp_connection::preStartGame(int gameNumber){
    if (gameNumber <= 10){
        Logger::Instance().msgServer = "";
        Logger::Instance().msgClient = "mapCreate" + std::to_string(gameNumber);

        while (1)
        {
            if (Logger::Instance().msgServer != "")
            {
                strcpy(message_, Logger::Instance().msgServer.c_str());
                break;
            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(16));
            
        }

        if ((strcmp(message_,"Bad") == 0) || (strcmp(message_,"Full") == 0)){
            boost::asio::async_write(socket_, boost::asio::buffer(message_),
                    boost::bind(&tcp_connection::errorEnd, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

        } else {
            this->gameNumber = message_[0] - '0';
            this->playerNumber = message_[2] - '0';

            boost::asio::async_write(socket_, boost::asio::buffer(message_),
                boost::bind(&tcp_connection::checkStepTime, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                gameNumber));
        }
    }
}

void tcp_connection::checkStepTime(const boost::system::error_code&, size_t, int mapNumber){
    if (mapNumber <= 3){
        strcpy(message_, "Step");
        boost::asio::async_write(socket_, boost::asio::buffer(message_),
            boost::bind(&tcp_connection::getStepTime, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    } else {
        strcpy(message_, "Start");
        boost::asio::async_write(socket_, boost::asio::buffer(message_),
                                 boost::bind(&tcp_connection::readCommand, shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }
}

void tcp_connection::getStepTime(const boost::system::error_code&, size_t){
    boost::asio::async_read(socket_, boost::asio::buffer(message_),
                             boost::bind(&tcp_connection::getStepTime2, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::getStepTime2(const boost::system::error_code&, size_t){
    Logger::Instance().msgClient = "step " + std::to_string(this->gameNumber) + " " + message_;

    const boost::system::error_code tmp;
    readCommand(tmp, 0);
}

void tcp_connection::errorEnd(const boost::system::error_code&, size_t){
    socket_.close();
}

void tcp_connection::readCommand(const boost::system::error_code&, size_t){
    message_[0] = '\0';
    boost::asio::async_read(socket_, boost::asio::buffer(message_),
                            boost::bind(&tcp_connection::processCommand, shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::processCommand(const boost::system::error_code&, size_t){
    if (strlen(message_) == 0){
        Logger::Instance().msgClient = "playerOut" + std::to_string(this->gameNumber) + " " + std::to_string(this->playerNumber);
        return;
    }
    
    const boost::system::error_code tmp;

    // send changes if command is sync
    if (strcmp(message_+4,"sync") == 0){
        processCommand2(tmp,0,0);
        return;
    }else
        Logger::Instance().commandQ.push(message_);
    
    readCommand(tmp,0);
}

void tcp_connection::processCommand2(const boost::system::error_code&, size_t, int sendedMessagesNumber){
    message_[0] = '\0';

    for (unsigned int i=0;i<Logger::Instance().changeMapCommandV.size();i++)
    {
        if ((Logger::Instance().changeMapCommandV[i][0] -'0') == this->gameNumber && (Logger::Instance().changeMapCommandV[i][2] -'0') == this->playerNumber)
        {
            sendedMessagesNumber++;

            strcat(message_,Logger::Instance().changeMapCommandV[i].c_str());
            strcat(message_, " | ");
            Logger::Instance().changeMapCommandV.erase(Logger::Instance().changeMapCommandV.begin() + i);
            
            --i;
        }
    }

    if (sendedMessagesNumber == 0){
        boost::this_thread::sleep(boost::posix_time::milliseconds(16));
        const boost::system::error_code tmp;
        processCommand2(tmp, 0, sendedMessagesNumber);
    } else {
        boost::asio::async_write(socket_, boost::asio::buffer(message_),
                                 boost::bind(&tcp_connection::readCommand, shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }
}

void tcp_server::start_accept(){
    tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service());

    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&tcp_server::handle_accept, this, new_connection,
        boost::asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error){
    if (!error)
        new_connection->start_comunication();

    start_accept();
}

