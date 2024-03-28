# Source files
SRC = main.cpp image.cpp

# Header files
HDR = image.hpp point.hpp

# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++11

# Object files
OBJ = $(SRC:.cpp=.o)

# Executable
EXE = build/main

# Output image
OUT = build/output.ppm

# Build
all: $(EXE)

# Link
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile
%.o: %.cpp $(HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean
clean:
	rm -f *.o build/*

# Run
run: $(EXE)
	./$(EXE) > $(OUT) && open $(OUT)

# Phony targets
.PHONY: all clean run

# EOF