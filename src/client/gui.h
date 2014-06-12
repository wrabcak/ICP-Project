/**
 * @file gui.h
 * @brief Header File for client's gui.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 21.05.2014
 *
 */

#ifndef GUI_H
#define GUI_H

#include <QObject>

#include <string>

/**
 * @class GUISignals
 * @brief Class which contains signal nad slot for communication with client.
 */
class GUISignals : public QObject
{
    Q_OBJECT
    
    public slots:
        /**
        * @fn void setSignal(char *)
        * @brief Emit signal with message to send.
        * @param message Message to send.
        * @return void
        */
        void setSignal(char *message);
    
    signals:
        /**
        * @fn void messageToGUI(char *message)
        * @brief Signal which indicates sended message.
        * @param message Message to send.
        * @return void
        */
        void messageToGUI(char *message);
};

/**
 * @class Message
 * @brief Singleton with cross-module communation messages (client - GUI).
 */
class Message {
public:
    static Message& Instance() {
        static Message theMessage;   // Instantiated when this function is called
        return theMessage;
    }

    std::string msgGUI; //! Message from GUI.
    std::string msgCommand; //! Message from GUI with player's command.
    std::string msgCommand2; //! Step or Start
    std::string msgClient; //! Message from Client.
    GUISignals *onlySignal; //! Class instance with signal and slot.
    
private:
    Message(){};
    Message(Message const&);
    Message& operator=(Message const&);
    ~Message(){};
};

/**
 * @fn int gui_main(int argc, char *argv[])
 * @brief Create application window.
 * @param argc Number of arguments.
 * @param argv Array containing arguments.
 * @return Zero if successfull, 1 if not.
 */
int gui_main(int argc, char *argv[]);

#endif
