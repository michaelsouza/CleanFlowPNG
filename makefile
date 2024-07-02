# Compiler
CXX = g++

# Compiler flags
# CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -fopenmp # release
CXXFLAGS = -std=c++11 -Wall -Wextra -g -O0 -fopenmp # debug

# Linker flags for libpng and OpenMP
LDFLAGS = -lpng -fopenmp

# Name of the output executable
TARGET = matrix_update

# Source files
SOURCES = matrix_update.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to link the program
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Clean up the build
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: all clean
