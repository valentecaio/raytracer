### Variables ###

# Source and build directory, compiler and compiler tags
SRCDIR := src
BUILD := build
CC = g++
CFLAGS = -Wall -std=c++11 -fopenmp -fopenacc -I include/ -I lib/ -I lib/glm-1.0.1/

# Main executable and output image filepaths
EXE = $(BUILD)/raytracer
OUT = $(BUILD)/output.ppm


### Automatic variables ###

# Source files in src/ directory
SRC := $(wildcard $(SRCDIR)/*.cpp)

# Object files in build/ directory
OBJ = $(SRC:.cpp=.o)
OBJ := $(subst $(SRCDIR)/, $(BUILD)/, $(OBJ))


### Rules ###

# Build
all: $(EXE)

# Link
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile
$(BUILD)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(BUILD)/*

debug:
	@echo CFLAGS = $(CFLAGS)
	@echo EXE = $(EXE)
	@echo OUT = $(OUT)
	@echo SRC = $(SRC)
	@echo OBJ = $(OBJ)

run: $(EXE)
	./$(EXE) > $(OUT) && open $(OUT)

.PHONY: all clean run

# EOF