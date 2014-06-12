/**
 * @file mainwindow.h
 * @brief Header File for mainwindow.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 21.05.2014
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QApplication>

#include <QMainWindow>
#include <QtCore>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsTextItem>


#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>

#include "gui.h"

namespace Ui {
    class MainWindow;
}

/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * @fn void initGame(int mapNumber)
     * @brief Print map and wait for commands.
     * @param mapNumber Map's number.
     */
    void initGame(int mapNumber);

    /**
     * @fn void paintMap()
     * @brief Paint whole map.
     */
    void paintMap();

    /**
     * @fn void deleteMap()
     * @brief Delete whole map.
     */
    void deleteMap();

    
private slots:
    /**
     * @fn void on_pushButton_clicked()
     * @brief Push button handler.
     */
    void on_pushButton_clicked();

    /**
     * @fn void changeMap(char *message)
     * @brief Change map block.
     * @param message Message with changes.
     */
    void changeMap(char *message);


private:
    /**
     * @fn void mousePressEvent(QMouseEvent *mouseEvent)
     * @brief Mouse press event handler.
     * @param mouseEvent Event.
     */
    void mousePressEvent(QMouseEvent *mouseEvent);

    /**
     * @fn void resizeEvent(QResizeEvent * event)
     * @brief Window resize handler.
     * @param event Event.
     */
    void resizeEvent(QResizeEvent * event);

    void endGame(int status, char *stepNumber);

    Ui::MainWindow *ui;

    int canvasWidth; // Canvas's width
    int canvasHeight; // Canvas's height
    double blockSize; // Block's size

    QGraphicsScene *scene; // Scene which will be displayed in the main window
    QGraphicsTextItem *title; // Text displayed in the main window
    QGraphicsTextItem *stepTimeTitle; // Text displayed in the main window
    QGraphicsPixmapItem *map1ThumbItem; // Object with the first map's thumbnail
    QGraphicsPixmapItem *map2ThumbItem; // Object with the second map's thumbnail
    QGraphicsPixmapItem *map3ThumbItem; // Object with the third map's thumbnail
    QPixmap blockImage; // Block's image
    QPixmap pathImage; // Path's image
    QPixmap endImage; // End's image
    QPixmap gateImage; // Gate's image
    QPixmap keyImage; // Key's image
    QPixmap playerUpImage; // Player's image - direction UP
    QPixmap playerLeftImage; // Player's image - direction LEFT
    QPixmap playerDownImage; // Player's image - direction DOWN
    QPixmap playerRightImage; // Player's image - direction RIGHT
    QPixmap badBoyImage; // BadBoy's image
    QGraphicsPixmapItem ***blockItem; // Array with whole map (images)

    QPixmap yellowPlayerUpImage; // Player's image - direction UP
    QPixmap greenPlayerUpImage; // Player's image - direction UP
    QPixmap redPlayerUpImage; // Player's image - direction UP
    QPixmap bluePlayerUpImage; // Player's image - direction UP

    QPixmap yellowPlayerDownImage; // Player's image - direction DOWN
    QPixmap greenPlayerDownImage; // Player's image - direction DOWN
    QPixmap redPlayerDownImage; // Player's image - direction DOWN
    QPixmap bluePlayerDownImage; // Player's image - direction DOWN

    QPixmap yellowPlayerLeftImage; // Player's image - direction LEFT
    QPixmap greenPlayerLeftImage; // Player's image - direction LEFT
    QPixmap redPlayerLeftImage; // Player's image - direction LEFT
    QPixmap bluePlayerLeftImage; // Player's image - direction LEFT

    QPixmap yellowPlayerRightImage; // Player's image - direction RIGHT
    QPixmap greenPlayerRightImage; // Player's image - direction RIGHT
    QPixmap redPlayerRightImage; // Player's image - direction RIGHT
    QPixmap bluePlayerRightImage; // Player's image - direction RIGHT



    int mapXSize; // Map's height
    int mapYSize; // Map's width
    char **mapArray; // Map's content
    bool mapClickEnabled; // Enable button clicks to canvas for choosing map
    bool buttonMapChoose; // Enable button clicks to choose map
    bool deletedMap;
    bool allocated;
    bool readCommands;

};

#endif // MAINWINDOW_H
