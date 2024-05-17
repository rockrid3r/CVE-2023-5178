CC=gcc
CFLAGS ?= -D FUSE_LOG_LEVEL=0

RM := rm
LIBS := -lfuse3 -lpthread

.PHONY: build
.PHONY: clean

SRC_PATH := ./src/
OBJ_PATH := ./obj/
INC_PATH := -I ./src/

BUILD_PATH := ./build/

SOURCES := $(wildcard $(SRC_PATH)*.c)
OBJ := $(patsubst $(SRC_PATH)%.c,$(OBJ_PATH)%.o,$(SOURCES))

all: clean $(BUILD_PATH)xpl

$(BUILD_PATH)xpl: build $(OBJ_PATH)xpl.o $(BUILD_PATH)fuse
	$(CC) $(OBJ_PATH)xpl.o $(LIBS) -o $@

$(BUILD_PATH)fuse: build $(OBJ_PATH)fuse.o
	$(CC) $(OBJ_PATH)fuse.o $(LIBS) -o $@ -D_FILE_OFFSET_BITS=64

$(BUILD_PATH)poc: build $(OBJ_PATH)poc.o
	$(CC) $(OBJ_PATH)poc.o -o $@

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC_PATH)

build: 
	@mkdir -p build
	@mkdir -p obj

clean:
	@rm -rf build
	@rm -rf obj
