PROJECT = Bludiste2014

CC = g++
RM = rm -rf
ZIP = zip

STD_CPP11 = -std=c++0x

CFLAGS = -O2 $(STD_CPP11) -s -Wall -Wextra -pedantic -c

LFAGS =  -lboost_system  -lboost_thread-mt -static-libstdc++

SRC_SERVER = src/server
SRC_CLIENT = src/client

ZIP_FILE = xvrabe07.zip

PACKED_FILES = Doxyfile Makefile src/client src/server src/maps  Readme.txt doc/

DOC_DIR = doc

all:  bludiste2014  bludiste2014-server bludiste2014-cli

bludiste2014-server: $(SRC_SERVER)/engine.o $(SRC_SERVER)/map.o $(SRC_SERVER)/game.o $(SRC_SERVER)/communication.o
	$(CC) -O2 -s -o $@  $(SRC_SERVER)/engine.o  $(SRC_SERVER)/map.o $(SRC_SERVER)/game.o  $(SRC_SERVER)/communication.o $(LFAGS)

bludiste2014-cli: $(SRC_CLIENT)/client.o
	$(CC) -O2 -s -o $@ $(SRC_CLIENT)/client.o $(LFAGS) -lncurses

$(SRC_CLIENT)/client.o: $(SRC_CLIENT)/client.cpp
	$(CC) $(CFLAGS) -DCONSOLE -o $@ $<

$(SRC_SERVER)/engine.o: $(SRC_SERVER)/engine.cpp
	$(CC) $(CFLAGS) -o $@ $<

$(SRC_SERVER)/map.o: $(SRC_SERVER)/map.cpp
	$(CC) $(CFLAGS) -o $@ $<

$(SRC_SERVER)/game.o: $(SRC_SERVER)/game.cpp
	$(CC) $(CFLAGS) -o $@ $<

$(SRC_SERVER)/communication.o: $(SRC_SERVER)/communication.cpp
	$(CC) $(CFLAGS) -o $@ $<

bludiste2014:
	export PATH=/usr/local/share/Qt-5.2.1/5.2.1/gcc_64/bin/:/opt/qt/5.2.1/gcc_64/bin/:$PATH
	qmake ./src/client/bludiste2014.pro -o Makefile-gui
	mv Makefile-gui ./src/client/
	make -f ./src/client/Makefile-gui

run:
	./bludiste2014-server > /dev/null &
	./bludiste2014

pack:
	$(ZIP) -r $(ZIP_FILE) $(PACKED_FILES)

doc:
	doxygen Doxyfile

clean:
	rm -rf Makefile-gui doc/ src/obj/  Doxyfile.bak $(SRC_CLIENT)/moc_*  $(SRC_SERVER)/*.o  $(SRC_SERVER)/*.a  $(SRC_SERVER)/*.so $(SRC_CLIENT)/*.o  $(SRC_CLIENT)/*.a  $(SRC_CLIENT)/*.so bludiste2014 bludiste2014-server bludiste2014-cli
	


