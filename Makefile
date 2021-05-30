CC := gcc
CFLAGS := -Wall -Wextra -O2

SRC_DIR := src
OBJ_DIR := obj
EXE_DIR := exe

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXE := pepsi

all: $(EXE)


$(EXE): $(OBJ)
	gcc -o $(EXE) $^ $(shell sdl2-config --cflags --libs)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	gcc -c $(CFLAGS) $< -o $@

clean:
	rm obj/*.o $(EXE)

.PHONY: clean all
