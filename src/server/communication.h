/**
 * @file communication.h
 * @brief Header file for comunication thread on server.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */
#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "engine.h"
#include "server.h"

using boost::asio::ip::tcp;

// max length msg for sending an receiving.
const int max_length = 1024;
// port where server listening
const int SERVER_PORT = 43210;

/**
 * @struct TClientInfo
 * @brief struct to store information about players move steps and if is player blocked
 */
struct TClientInfo
{
	char playerID;
	std::string move;
	int block;
};

/**
 * @fn int server_main()
 * @brief Starting comunication with clients.
 * @return Zero if successfull, 1 if not.
 */
int server_main();

/**
 * @class tcp_connection
 * @brief class for servicing connection over clients
 */
class tcp_connection : public boost::enable_shared_from_this<tcp_connection>{
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;
		tcp::socket& socket();
        // start communicate with clients 
		void start_comunication();
        // create connection
		static pointer create(boost::asio::io_service& io_service){
			return pointer(new tcp_connection(io_service));
		}

	private:
		tcp_connection(boost::asio::io_service& io_service) : socket_(io_service){
		}
        // communication handlers 
		void preStartGame(int gameNumber);
        void checkStepTime(const boost::system::error_code&, size_t, int mapNumber);
        void getStepTime(const boost::system::error_code&, size_t);
        void getStepTime2(const boost::system::error_code&, size_t);
		void errorEnd(const boost::system::error_code&, size_t);
        void readCommand(const boost::system::error_code&, size_t);
        void processCommand(const boost::system::error_code&, size_t);
        void processCommand2(const boost::system::error_code&, size_t, int sendedMessagesNumber);
		void chooseMap3(const boost::system::error_code&, size_t size);
		void chooseMap2(const boost::system::error_code&, size_t);
		void chooseMap(const boost::system::error_code&, size_t);
		void handle_welcome(const boost::system::error_code&, size_t);
        
		tcp::socket socket_;
		char message_[max_length]; //for receiving and sending msgs to clients
        int gameNumber; // actual game ID
        int playerNumber; // actual player ID
        bool readData; // bool value if reading data
        bool endRead; // end  reading data ;
};

/**
 * @class tcp_server
 * @brief class for make server and accpting clients.
 */
class tcp_server{
	public:
		tcp_server(boost::asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), SERVER_PORT)){
			start_accept();
		}

	private:
		void start_accept();
		void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error);
		tcp::acceptor acceptor_;
};

#endif