# ============================================================
# Makefile – Pacman (Computer Graphics Lab Final)
# ============================================================
# Requires: freeglut3-dev, libgl-dev, libglu1-mesa-dev
#
# Build:   make
# Run:     ./pacman
# Clean:   make clean
# ============================================================

CXX      = g++
TARGET   = pacman
SRC      = pacman.cpp

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11 -O2

# Libraries
LIBS     = -lGL -lGLU -lglut

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
