# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -O3 -fopenmp -flto # Focused on execution speed
# CXXFLAGS = -std=c++11 -Wall -Wextra -g -O0 -fopenmp # debug

# Linker flags for libpng and OpenMP
LDFLAGS = -lpng -fopenmp

# Name of the output executable
TARGET = clean_image

# Source files
SOURCES = clean_image.cpp

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
