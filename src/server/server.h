/**
 * @file server.h
 * @brief Header file for global declarations for server.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 10.05.2014
 *
 */
#ifndef SERVER_H
#define SERVER_H
#include <boost/thread.hpp>
#include <queue>
#include <vector>
#include <string>

/**
 * @struct TCommands
 * @brief struct to store information about old and new step
 */
typedef struct {
    int gameNumber;
    int playerNumber;
    int oldX;
    int oldY;
    char oldSign;
    int newX;
    int newY;
    char newSign;
} TCommands;

/**
 * @Class Logger
 * @brief Class for communication between engine and communication thread
 */
class Logger {
public:
  static Logger& Instance() {
    static Logger theLogger;
    return theLogger;
  }
  std::queue<std::string> commandQ;
  std::vector<std::string> changeMapCommandV;
  std::string msgServer;
  std::string msgClient;
    std::string msgChooseMap;

private:
  Logger(){};
  Logger(Logger const&);
  Logger& operator=(Logger const&);
  ~Logger(){};
};
#endif