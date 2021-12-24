CC := g++

SRC_DIR :=  
INC_DIR := ./ 
OBJ_DIR := obj/
BIN_DIR := bin/

SRC := $(wildcard $(SRC_DIR)*.cpp)
OBJ := $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))
EXE := $(BIN_DIR)executable

CFLAGS := -g -c -std=c++17 -I/$(INC_DIR)
LFLAGS := -lSDL2 -lGL -lGLEW

all: $(OBJ)
	$(CC) $^ -o $(EXE) $(LFLAGS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) $< -o $@ $(CFLAGS)

run: 
	@$(EXE)

debug:
	@gdb $(EXE)

clean:
	@rm $(OBJ_DIR)*.o
