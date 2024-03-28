# Source files in src/ directory
SRC := $(wildcard src/*.cpp)

# Build directory, compiler and compiler tags
BUILD := build
CC = g++
CFLAGS = -Wall -std=c++11 -I include/ -I lib/ -I lib/glm-1.0.1/

# Object files in build/ directory
OBJ = $(SRC:.cpp=.o)
OBJ := $(subst src/, $(BUILD)/, $(OBJ))

# Executable
EXE = $(BUILD)/raytracer

# Output image
OUT = $(BUILD)/output.ppm

# Build
all: $(EXE)

# Link
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile
$(BUILD)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(BUILD)/*

debug:
	@echo CFLAGS = $(CFLAGS)
	@echo SRC = $(SRC)
	@echo OBJ = $(OBJ)

run: $(EXE)
	./$(EXE) > $(OUT) && open $(OUT)

# Phony targets
.PHONY: all clean run

# EOF