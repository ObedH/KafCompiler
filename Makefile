# Compiler and flags
CC := C:/mingw64/bin/gcc.exe
CFLAGS := -std=c17 -Wall -Wpedantic -Iinclude -Iexternal/SDL2/include
LDFLAGS := -Lexternal/SDL2/lib -lmingw32 -lSDL2main -lSDL2

# Source and output
SRC := $(wildcard src/*.c)
OUT := build/Debug/Perchrit.exe

# Default build rule
all: $(OUT)

$(OUT): $(SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(OUT)

# Clean rule
clean:
	rm -f $(OUT)

.PHONY: all clean
