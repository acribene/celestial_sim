# Compiler and flags
CXX = C:/raylib/w64devkit/bin/g++.exe  # compiler from raylib's w64devkit
CXXFLAGS = -Wall -Wpedantic -std=c++17 -I C:/raylib/w64devkit/x86_64-w64-mingw32/include -fdiagnostics-color=always -D_USE_MATH_DEFINES # ADD -g for debugging
LDFLAGS = -L C:/raylib/w64devkit/x86_64-w64-mingw32/lib -lraylib -lopengl32 -lgdi32 -lwinmm

# Target
TARGET = main

# Sources and objects
SRC = $(wildcard src/source/*.cpp) $(wildcard src/utils/*.cpp)
OBJS = $(SRC:.cpp=.o)

all: $(TARGET).exe

# Link step
$(TARGET).exe: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Compile step
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET).exe
	./$(TARGET).exe

clean:
	rm -f $(OBJS) $(TARGET).exe
