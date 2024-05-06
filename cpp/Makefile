### Variables ###

# OpenMP and OpenACC flags
OPENMP_FLAG :=
OPENACC_FLAG :=
ifdef OPENMP
	OPENMP_FLAG = -fopenmp -DOPENMP
endif
ifdef OPENACC
	OPENACC_FLAG = -fopenacc -DOPENACC
endif

# Source and build directory, compiler and compiler tags
SRCDIR := src
BUILD := build
CC = g++
CFLAGS = -Wall -std=c++11 $(OPENMP_FLAG) $(OPENACC_FLAG) \
 -I lib/ \
 -I lib/glm-1.0.1/ \
#  -I lib/tinyobjloader-1.0.6/

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

# compile, run and open the output image
run: $(EXE)
	./$(EXE) > $(OUT) && open $(OUT)

# run in multi-threaded mode
run_mt:
	@$(MAKE) run OPENMP=1

.PHONY: all clean debug run run_mt

# EOF