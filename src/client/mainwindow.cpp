/**
 * @file mainwindow.cpp
 * @brief Source File for mainwindow.
 * @author Lukas Vrabec
 * @author Martin Sikora
 * @date 21.05.2014
 *
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    readCommands = true;
    deletedMap = true;
    allocated = false;

    ui->setupUi(this);

    scene = new QGraphicsScene(this); // Create new scene which will be displayed
    ui->graphicsView->setScene(scene); // Set scene to graphicsView
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);


    blockSize = 24;
    scene->setSceneRect(0,0,600,600); // Set bounding rectangle as whole window
    ui->graphicsView->setSceneRect(scene->sceneRect());

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern)); // Set background

    // INIT WINDOW
    title = scene->addText("ICP MAZE");
    QFont font("Arial", 64);
    title->setFont(font);
    title->setPos(100,50);

    stepTimeTitle = scene->addText("Type steptime or map nuber to the box below.");
    QFont font1("Arial", 12);
    stepTimeTitle->setFont(font1);
    stepTimeTitle->setPos(120, 340);


    QPixmap map1Thumb(":/images/map1_thumbnail.jpg");
    QPixmap map2Thumb(":/images/map2_thumbnail.jpg");
    QPixmap map3Thumb(":/images/map3_thumbnail.jpg");

    blockImage.load(":/images/block.jpg");
    pathImage.load(":/images/path.jpg");
    endImage.load(":/images/end.jpg");
    gateImage.load(":/images/gate.jpg");
    keyImage.load(":/images/key.jpg");
    playerUpImage.load(":/images/player_up.jpg");
    playerRightImage.load(":/images/player_right.jpg");
    playerDownImage.load(":/images/player_down.jpg");
    playerLeftImage.load(":/images/player_left.jpg");
    badBoyImage.load(":/images/hlidac.jpg");

    yellowPlayerUpImage.load(":/images/player_yellow_up.jpg");
    greenPlayerUpImage.load(":/images/player_green_up.jpg");
    redPlayerUpImage.load(":/images/player_red_up.jpg");
    bluePlayerUpImage.load(":/images/player_blue_up.jpg");

    yellowPlayerDownImage.load(":/images/player_yellow_down.jpg");
    greenPlayerDownImage.load(":/images/player_green_down.jpg");
    redPlayerDownImage.load(":/images/player_red_down.jpg");
    bluePlayerDownImage.load(":/images/player_blue_down.jpg");

    yellowPlayerLeftImage.load(":/images/player_yellow_left.jpg");
    greenPlayerLeftImage.load(":/images/player_green_left.jpg");
    redPlayerLeftImage.load(":/images/player_red_left.jpg");
    bluePlayerLeftImage.load(":/images/player_blue_left.jpg");

    yellowPlayerRightImage.load(":/images/player_yellow_right.jpg");
    greenPlayerRightImage.load(":/images/player_green_right.jpg");
    redPlayerRightImage.load(":/images/player_red_right.jpg");
    bluePlayerRightImage.load(":/images/player_blue_right.jpg");


    // This will be automatic
    blockImage = blockImage.scaled(blockSize,blockSize);
    pathImage = pathImage.scaled(blockSize,blockSize);
    endImage = endImage.scaled(blockSize,blockSize);
    gateImage = gateImage.scaled(blockSize,blockSize);
    keyImage = keyImage.scaled(blockSize,blockSize);
    playerUpImage = playerUpImage.scaled(blockSize,blockSize);
    playerRightImage = playerRightImage.scaled(blockSize,blockSize);
    playerDownImage = playerDownImage.scaled(blockSize,blockSize);
    playerLeftImage = playerLeftImage.scaled(blockSize,blockSize);
    badBoyImage = badBoyImage.scaled(blockSize,blockSize);

    yellowPlayerUpImage = yellowPlayerUpImage.scaled(blockSize,blockSize);
    greenPlayerUpImage = greenPlayerUpImage.scaled(blockSize,blockSize);
    redPlayerUpImage = redPlayerUpImage.scaled(blockSize,blockSize);
    bluePlayerUpImage = bluePlayerUpImage.scaled(blockSize,blockSize);

    yellowPlayerDownImage = yellowPlayerDownImage.scaled(blockSize,blockSize);
    greenPlayerDownImage = greenPlayerDownImage.scaled(blockSize,blockSize);
    redPlayerDownImage = redPlayerDownImage.scaled(blockSize,blockSize);
    bluePlayerDownImage = bluePlayerDownImage.scaled(blockSize,blockSize);

    yellowPlayerLeftImage = yellowPlayerLeftImage.scaled(blockSize,blockSize);
    greenPlayerLeftImage = greenPlayerLeftImage.scaled(blockSize,blockSize);
    redPlayerLeftImage = redPlayerLeftImage.scaled(blockSize,blockSize);
    bluePlayerLeftImage = bluePlayerLeftImage.scaled(blockSize,blockSize);

    yellowPlayerRightImage = yellowPlayerRightImage.scaled(blockSize,blockSize);
    greenPlayerRightImage = greenPlayerRightImage.scaled(blockSize,blockSize);
    redPlayerRightImage = redPlayerRightImage.scaled(blockSize,blockSize);
    bluePlayerRightImage = bluePlayerRightImage.scaled(blockSize,blockSize);


    map1ThumbItem = scene->addPixmap(map1Thumb);
    map1ThumbItem->setOffset(38,180);

    map2ThumbItem = scene->addPixmap(map2Thumb);
    map2ThumbItem->setOffset(225,180);

    map3ThumbItem = scene->addPixmap(map3Thumb);
    map3ThumbItem->setOffset(412,180);

    mapClickEnabled = true;
    buttonMapChoose = true;

    while (1) {
        if (Message::Instance().msgClient != ""){
            std::string tmp = Message::Instance().msgClient;
            Message::Instance().msgClient = "";

            tmp = tmp.substr(94,tmp.length()-94);
            ui->textEdit->setText(QString::fromStdString(tmp));
            break;
        }
    }

    // When enter pressed push button
    connect(ui->lineEdit, SIGNAL(returnPressed()),ui->pushButton,SIGNAL(clicked()));

    connect(ui->pushButton_2,SIGNAL(clicked()),qApp,SLOT(quit()));

    connect((QObject*) Message::Instance().onlySignal, SIGNAL(messageToGUI(char *)), (QObject*) this, SLOT(changeMap(char *)));
}

MainWindow::~MainWindow()
{
    if (! deletedMap){
        Message::Instance().msgGUI = "logout";
        deleteMap();
    }

    if (allocated){
        for (int i = 0; i < this->mapYSize; i++) {
            free(this->mapArray[i]);
        }

        free(this->mapArray);
    
        for (int i = 0; i < this->mapXSize; i++){
            free(blockItem[i]);
        }

        free(blockItem);
    }

    delete ui;
}

/**
 * @brief MainWindow::mousePressEvent catches mouse clicks in the main window
 * @param mouseEvent contains parameters that describe a mouse event
 */
void MainWindow::mousePressEvent(QMouseEvent *mouseEvent){
    if (mapClickEnabled){
        // Chech where user clicked
        if ((mouseEvent->y() >= 180) && (mouseEvent->y() <= 330)){
            if ((mouseEvent->x() >= 35) && (mouseEvent->x() <= 185)){
                initGame(1);
            }

            if ((mouseEvent->x() >= 225) && (mouseEvent->x() <= 375)){
                initGame(2);
            }

            if ((mouseEvent->x() >= 412) && (mouseEvent->x() <= 562)){
                initGame(3);
            }
        }
    }
}


void MainWindow::initGame(int mapNumber){
    ui->textEdit->setText("Commands:");

    mapClickEnabled = false;
    buttonMapChoose = false;


    std::stringstream stringStream;
    stringStream << mapNumber;
    
    Message::Instance().msgGUI = stringStream.str();
    ui->graphicsView->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern)); // Set background

    //scene->removeItem(title);
    delete title;
    delete stepTimeTitle;
    delete map1ThumbItem;
    delete map2ThumbItem;
    delete map3ThumbItem;
    
    scene->update();

    while (1) {
        if (Message::Instance().msgClient != ""){
            std::string message = Message::Instance().msgClient;
            Message::Instance().msgClient = "";

            char tmp[2];
            tmp[0] = message[0];
            tmp[1] = message[1];
            this->mapXSize = atoi(tmp);

            tmp[0] = message[3];
            tmp[1] = message[4];
            this->mapYSize = atoi(tmp);

            message = message.substr(6,message.length() - 6);

            this->mapArray = (char **) malloc(sizeof(char **) * this->mapXSize);
            for (int i = 0; i < this->mapYSize; i++) {
                this->mapArray[i] = (char *) malloc(sizeof(char *) * this->mapYSize);
            }

            for (int i = 0; i < this->mapXSize; i++) {
                for (int j = 0; j < this->mapYSize; j++) {
                    this->mapArray[i][j] = message[i * this->mapXSize + j];
                }
            }
            
            break;
        }
        
        usleep(16);
    }
    
    
    blockItem = (QGraphicsPixmapItem ***) malloc(sizeof(QGraphicsPixmapItem ***) * this->mapXSize);
    for (int i=0;i<this->mapYSize;i++)
        blockItem[i] = (QGraphicsPixmapItem **) malloc(sizeof(QGraphicsPixmapItem **) * this->mapYSize);

    paintMap();



    while (1){

        if (Message::Instance().msgCommand2 == "Start") {
            Message::Instance().msgCommand2 = "";
            break;
        }

        if (Message::Instance().msgCommand2 == "Step"){
            Message::Instance().msgCommand2 = "";
            QString lineEditString = ui->lineEdit->displayText();

            if (lineEditString != "") {
                double stepTime = atof(lineEditString.toStdString().c_str());

                if (stepTime >= 0.5 && stepTime <= 5){
                    Message::Instance().msgGUI = lineEditString.toStdString();
                    break;
                } else{
                    Message::Instance().msgGUI = "2";
                    break;
                }
            } else{
                Message::Instance().msgGUI = "2";
                break;
            }
        }

        usleep(16);
    }

    deletedMap = false;
    allocated = true;
}

void MainWindow::endGame(int state, char *stepNumber){
    Message::Instance().msgGUI = "logout";

    ui->lineEdit->setEnabled(false);
    ui->pushButton->setEnabled(false);
    deleteMap();
    deletedMap = true;
    scene->update();



    ui->graphicsView->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern)); // Set background
    if (state == 0){
        std::string tmp1 = "Logged out.\nYou've done ";
        std::string tmp2 = " moves.\n\nPush Exit button for quit game.";

        for (unsigned int i=0; i<strlen(stepNumber); i++){
            if (stepNumber[i] == ' '){
                stepNumber[i] = '\0';
                break;
            }
        }

        char tmp[1024];
        memset(tmp, 0, 1024);
        strcat(tmp,tmp1.c_str());
        strcat(tmp, stepNumber);
        strcat(tmp, tmp2.c_str());

        QString string = tmp;
        title = scene->addText(string);

    }else if (state == 1){
        std::string tmp1 = "You're dead.\nYou've done ";
        std::string tmp2 = " moves.\n\nPush Exit button for quit game.";

        for (unsigned int i=0; i<strlen(stepNumber); i++){
            if (stepNumber[i] == ' '){
                stepNumber[i] = '\0';
                break;
            }
        }

        char tmp[1024];
        memset(tmp, 0, 1024);
        strcat(tmp,tmp1.c_str());
        strcat(tmp, stepNumber);
        strcat(tmp, tmp2.c_str());

        QString string = tmp;
        title = scene->addText(string);

    }else if (state == 2){
        std::string tmp1 = "CONGRATULATION!\nYou won.\nYou've done ";
        std::string tmp2 = " moves.\n\nPush Exit button for quit game.";

        for (unsigned int i=0; i<strlen(stepNumber); i++){
            if (stepNumber[i] == ' '){
                stepNumber[i] = '\0';
                break;
            }
        }

        char tmp[1024];
        memset(tmp, 0, 1024);
        strcat(tmp,tmp1.c_str());
        strcat(tmp, stepNumber);
        strcat(tmp, tmp2.c_str());

        QString string = tmp;
        title = scene->addText(string);
    }

    QFont font("Arial", 24);
    title->setFont(font);
    title->setPos(30,200);
}


void MainWindow::changeMap(char * msg){
    if (!readCommands)
        return;

    if (msg[4] > '9' || msg[4] < '0'){
        if (msg[4] != 'S' && msg[5] != 'y'){
            char tmp[1024];
            strncpy(tmp, msg+4, 24-3);
            ui->textEdit->append(tmp);
            ui->textEdit->update();
            if (strncmp(tmp,"Logout",6) == 0){
                readCommands = false;
                endGame(0, tmp+7);
                return;
            } else if (strncmp(tmp, "Dead", 4) == 0){
                readCommands = false;
                endGame(1 , tmp+5);
                return;
            } else if (strncmp(tmp, "End", 3) == 0){
                readCommands = false;
                endGame(2, tmp+4);
                return;
            }
        }
    } else {
        int oldX = 0;
        int oldY = 0;
        int newX = 0;
        int newY = 0;
        char oldSign = ' ';
        char newSign = ' ';
        int playerNumber = -1;

        if (msg[4] == '0')
            oldY = msg[5] - '0';
        else
            oldY = ((msg[4] -'0') * 10) + (msg[5] - '0');
        
        if (msg[7] == '0')
            oldX = msg[8] - '0';
        else
            oldX = ((msg[7] -'0') * 10) + (msg[8] - '0');
        
        oldSign = msg[10];
        
        if (msg[12] == '0')
            newY = msg[13] - '0';
        else
            newY = ((msg[12] -'0') * 10) + (msg[13] - '0');
        
        if (msg[15] == '0')
            newX = msg[16] - '0';
        else
            newX = ((msg[15] -'0') * 10) + (msg[16] - '0');
        
        newSign = msg[18];

        playerNumber = msg[20] - '0';
        
        
        delete blockItem[oldX][oldY];

        if (oldSign == ' ')
            blockItem[oldX][oldY] = scene->addPixmap(pathImage);
            
        else if (oldSign == '^'){
            switch (playerNumber) {
                case 0:
                    blockItem[oldX][oldY] = scene->addPixmap(yellowPlayerUpImage);
                    break;
                case 1:
                    blockItem[oldX][oldY] = scene->addPixmap(greenPlayerUpImage);
                    break;
                case 2:
                    blockItem[oldX][oldY] = scene->addPixmap(redPlayerUpImage);
                    break;
                case 3:
                    blockItem[oldX][oldY] = scene->addPixmap(bluePlayerUpImage);
                    break;
            }

        }else if (oldSign == '>'){
            switch (playerNumber) {
                case 0:
                    blockItem[oldX][oldY] = scene->addPixmap(yellowPlayerRightImage);
                    break;
                case 1:
                    blockItem[oldX][oldY] = scene->addPixmap(greenPlayerRightImage);
                    break;
                case 2:
                    blockItem[oldX][oldY] = scene->addPixmap(redPlayerRightImage);
                    break;
                case 3:
                    blockItem[oldX][oldY] = scene->addPixmap(bluePlayerRightImage);
                    break;
            }

        }else if (oldSign == 'v'){
            switch (playerNumber) {
                case 0:
                    blockItem[oldX][oldY] = scene->addPixmap(yellowPlayerDownImage);
                    break;
                case 1:
                    blockItem[oldX][oldY] = scene->addPixmap(greenPlayerDownImage);
                    break;
                case 2:
                    blockItem[oldX][oldY] = scene->addPixmap(redPlayerDownImage);
                    break;
                case 3:
                    blockItem[oldX][oldY] = scene->addPixmap(bluePlayerDownImage);
                    break;
            }

        }else if (oldSign == '<'){
            switch (playerNumber) {
                case 0:
                    blockItem[oldX][oldY] = scene->addPixmap(yellowPlayerLeftImage);
                    break;
                case 1:
                    blockItem[oldX][oldY] = scene->addPixmap(greenPlayerLeftImage);
                    break;
                case 2:
                    blockItem[oldX][oldY] = scene->addPixmap(redPlayerLeftImage);
                    break;
                case 3:
                    blockItem[oldX][oldY] = scene->addPixmap(bluePlayerLeftImage);
                    break;
            }

        }else if (oldSign == '$')
            blockItem[oldX][oldY] = scene->addPixmap(keyImage);

        else if (oldSign == '=')
            blockItem[oldX][oldY] = scene->addPixmap(gateImage);

        else if (oldSign == '1')
            blockItem[oldX][oldY] = scene->addPixmap(badBoyImage);



        blockItem[oldX][oldY]->setOffset(oldX * blockSize, oldY * blockSize);

            
        delete blockItem[newX][newY];
        
        if (newSign == ' ')
            blockItem[newX][newY] = scene->addPixmap(pathImage);
        
        else if (newSign == '^'){
            switch (playerNumber) {
                case 0:
                    blockItem[newX][newY] = scene->addPixmap(yellowPlayerUpImage);
                    break;
                case 1:
                    blockItem[newX][newY] = scene->addPixmap(greenPlayerUpImage);
                    break;
                case 2:
                    blockItem[newX][newY] = scene->addPixmap(redPlayerUpImage);
                    break;
                case 3:
                    blockItem[newX][newY] = scene->addPixmap(bluePlayerUpImage);
                    break;
            }

        }else if (newSign == '>'){
            switch (playerNumber) {
                case 0:
                    blockItem[newX][newY] = scene->addPixmap(yellowPlayerRightImage);
                    break;
                case 1:
                    blockItem[newX][newY] = scene->addPixmap(greenPlayerRightImage);
                    break;
                case 2:
                    blockItem[newX][newY] = scene->addPixmap(redPlayerRightImage);
                    break;
                case 3:
                    blockItem[newX][newY] = scene->addPixmap(bluePlayerRightImage);
                    break;
            }

        }else if (newSign == 'v'){
            switch (playerNumber) {
                case 0:
                    blockItem[newX][newY] = scene->addPixmap(yellowPlayerDownImage);
                    break;
                case 1:
                    blockItem[newX][newY] = scene->addPixmap(greenPlayerDownImage);
                    break;
                case 2:
                    blockItem[newX][newY] = scene->addPixmap(redPlayerDownImage);
                    break;
                case 3:
                    blockItem[newX][newY] = scene->addPixmap(bluePlayerDownImage);
                    break;
            }

        }else if (newSign == '<'){
            switch (playerNumber) {
                case 0:
                    blockItem[newX][newY] = scene->addPixmap(yellowPlayerLeftImage);
                    break;
                case 1:
                    blockItem[newX][newY] = scene->addPixmap(greenPlayerLeftImage);
                    break;
                case 2:
                    blockItem[newX][newY] = scene->addPixmap(redPlayerLeftImage);
                    break;
                case 3:
                    blockItem[newX][newY] = scene->addPixmap(bluePlayerLeftImage);
                    break;
            }

        }else if (newSign == '$')
            blockItem[oldX][oldY] = scene->addPixmap(keyImage);

        else if (newSign == '=')
            blockItem[newX][newY] = scene->addPixmap(gateImage);

        else if (newSign == '1')
            blockItem[newX][newY] = scene->addPixmap(badBoyImage);


        blockItem[newX][newY]->setOffset(newX * blockSize, newY * blockSize);
    }
    
    if (strlen(msg) > 24){
        changeMap(&(msg[24]));
    }
}



void MainWindow::deleteMap(){
    for (int i = 0; i < 25; i++){
        for (int j=0; j < 25; j++){
            scene->removeItem(blockItem[i][j]);
            delete blockItem[i][j];
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent * resizeEvent){
    if (! mapClickEnabled){
        QSize tmp = resizeEvent->size();
        canvasHeight = tmp.height() - 82;
        canvasWidth = tmp.width() - 26;

        if (canvasHeight > canvasWidth)
            blockSize = canvasWidth / 25.0;
        else
            blockSize = canvasHeight / 25.0;


        blockImage = blockImage.scaled(blockSize,blockSize);
        pathImage = pathImage.scaled(blockSize,blockSize);
        endImage = endImage.scaled(blockSize,blockSize);
        gateImage = gateImage.scaled(blockSize,blockSize);
        keyImage = keyImage.scaled(blockSize,blockSize);
        playerUpImage = playerUpImage.scaled(blockSize,blockSize);
        playerRightImage = playerRightImage.scaled(blockSize,blockSize);
        playerDownImage = playerDownImage.scaled(blockSize,blockSize);
        playerLeftImage = playerLeftImage.scaled(blockSize,blockSize);
        badBoyImage = playerLeftImage.scaled(blockSize,blockSize);

        yellowPlayerUpImage = yellowPlayerUpImage.scaled(blockSize,blockSize);
        greenPlayerUpImage = greenPlayerUpImage.scaled(blockSize,blockSize);
        redPlayerUpImage = redPlayerUpImage.scaled(blockSize,blockSize);
        bluePlayerUpImage = bluePlayerUpImage.scaled(blockSize,blockSize);

        yellowPlayerDownImage = yellowPlayerDownImage.scaled(blockSize,blockSize);
        greenPlayerDownImage = greenPlayerDownImage.scaled(blockSize,blockSize);
        redPlayerDownImage = redPlayerDownImage.scaled(blockSize,blockSize);
        bluePlayerDownImage = bluePlayerDownImage.scaled(blockSize,blockSize);

        yellowPlayerLeftImage = yellowPlayerLeftImage.scaled(blockSize,blockSize);
        greenPlayerLeftImage = greenPlayerLeftImage.scaled(blockSize,blockSize);
        redPlayerLeftImage = redPlayerLeftImage.scaled(blockSize,blockSize);
        bluePlayerLeftImage = bluePlayerLeftImage.scaled(blockSize,blockSize);

        yellowPlayerRightImage = yellowPlayerRightImage.scaled(blockSize,blockSize);
        greenPlayerRightImage = greenPlayerRightImage.scaled(blockSize,blockSize);
        redPlayerRightImage = redPlayerRightImage.scaled(blockSize,blockSize);
        bluePlayerRightImage = bluePlayerRightImage.scaled(blockSize,blockSize);

        scene->setSceneRect(0,0,canvasWidth, canvasHeight); // Set bounding rectangle as whole window
        ui->graphicsView->setSceneRect(scene->sceneRect());

        deleteMap();

        paintMap();
    }
}


void MainWindow::paintMap(){
    for (int i = 0; i < 25; i++){
        for (int j = 0; j < 25; j++){
            if (this->mapArray[j][i] == ' '){
                blockItem[i][j] = scene->addPixmap(pathImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] == '#'){
                blockItem[i][j] = scene->addPixmap(blockImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] == '='){
                blockItem[i][j] = scene->addPixmap(gateImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] == '$'){
                blockItem[i][j] = scene->addPixmap(keyImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] == '@'){
                blockItem[i][j] = scene->addPixmap(endImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] == '^'){
                blockItem[i][j] = scene->addPixmap(playerUpImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] == '>'){
                blockItem[i][j] = scene->addPixmap(playerRightImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] == 'v'){
                blockItem[i][j] = scene->addPixmap(playerDownImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] == '<'){
                blockItem[i][j] = scene->addPixmap(playerLeftImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);

            } else if (this->mapArray[j][i] >= '0' && this->mapArray[j][i] <= '9'){
                blockItem[i][j] = scene->addPixmap(badBoyImage);
                blockItem[i][j]->setOffset(i*blockSize,j*blockSize);
            }
        }
    }

}


void MainWindow::on_pushButton_clicked(){
    QString message = ui->lineEdit->displayText();

    if (buttonMapChoose){
        initGame(atoi(message.toStdString().c_str()));
    } else {
        if (message == "go" || message == "left" || message == "right" || message == "stop" || message == "take" || message == "open")
            ui->textEdit->append(message);
        Message::Instance().msgCommand = message.toStdString();
    }
}
