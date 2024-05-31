#---------------------------------------------------------------------
# Arquivo	: Makefile
# Conteúdo	: compilar o programa
# Autor		: Juan Braga
# Crédito   : Adaptado de  Alysson Silva (https://www.youtube.com/watch?v=v9mWnGrAKUQ)
#---------------------------------------------------------------------
# Opções	: make all - compila tudo
#		: make clean - remove objetos e executável
#---------------------------------------------------------------------

CC = g++
CXXFLAGS = -Wall

INCLUDE_FOLDER = ./include/
BIN_FOLDER = ./bin/
OBJ_FOLDER = ./obj/
SRC_FOLDER = ./src/

TARGET = a.out
SRC = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))

$(OBJ_FOLDER)%.o: $(SRC_FOLDER)%.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@ -I$(INCLUDE_FOLDER)

all: $(OBJ)
	$(CC) $(CXXFLAGS) -o $(BIN_FOLDER)$(TARGET) $(OBJ)

clean:
	rm $(OBJ_FOLDER)* $(BIN_FOLDER)a.out