# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation

CXX = g++

INC = -Isource/include/SDL2
LNK = -Lsource/lib
HDS = -Ichip8-emu/headers

CXXFLAGS = -Wall -g $(HDS) $(INC)

# ****************************************************
# Targets needed to bring the executable up to date

main: main.o chip8.o
	$(CXX) $(CXXFLAGS) $(LNK) main.o chip8.o -lmingw32 -lSDL2main -lSDL2 -o main

# The main.o target can be written more simply

main.o: main.cpp ./source/include/SDL2/SDL.h
	$(CXX) $(CXXFLAGS) $(LNK) -lSDL2main -lSDL2 -c main.cpp

chip8.o: chip8.cpp
	$(CXX) $(CXXFLAGS) -c chip8.cpp