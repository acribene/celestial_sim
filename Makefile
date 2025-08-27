# Compiler and flags
CXX = C:/raylib/w64devkit/bin/g++.exe  # using compiler included with raylib
CXXFLAGS = -Wall -Wpedantic -Wshadow -g -std=c++17 -I C:/raylib/w64devkit/x86_64-w64-mingw32/include -fdiagnostics-color=always
LDFLAGS = -L C:/raylib/w64devkit/x86_64-w64-mingw32/lib -lraylib -lopengl32 -lgdi32 -lwinmm

# Target
TARGET = main
SRC = source.cpp

all: $(TARGET).exe

$(TARGET).exe: $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

run: $(TARGET).exe
	./$(TARGET).exe

clean:
	rm $(TARGET).exe
