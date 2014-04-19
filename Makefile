
TARGET=tracert

OBJ_DIR=obj
#SRC_DIR=./src
#INC_DIR=./include

#mkdir -p $(OBJ_DIR)
#mkdir -p $(SRC_DIR)
#mkdir -p $(INC_DIR)

CC=g++ 
CFLAG=-g -c -D_MY_TEST_

OBJS=$(OBJ_DIR)/main.o $(OBJ_DIR)/socket_raw.o $(OBJ_DIR)/traceroute.o 
DIR_CREAT=mkdir_obj

$(TARGET): $(DIR_CREAT) $(OBJS)
	g++  $(OBJS) -o $(TARGET)
clean:
	rm -f $(OBJ_DIR)/*

mkdir_obj:
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/main.o:main.cpp
	g++ $(CFLAG) main.cpp -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/socket_raw.o:socket_raw.cpp
	g++ $(CFLAG) socket_raw.cpp -o $(OBJ_DIR)/socket_raw.o

$(OBJ_DIR)/traceroute.o:traceroute.cpp
	g++ $(CFLAG) traceroute.cpp -o $(OBJ_DIR)/traceroute.o





