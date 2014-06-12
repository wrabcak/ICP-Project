/**
 * @file gui.cpp
 * @brief Source code for client's gui.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 21.05.2014
 *
 */

#include <QApplication>
#include "gui.h"
#include "mainwindow.h"


int gui_main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    
    return a.exec();
}

void GUISignals::setSignal(char * message){
    emit messageToGUI(message);
}