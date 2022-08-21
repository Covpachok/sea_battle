CXX = g++
CXXFLAGS = -Wall -g -lncurses
OBJMODULES = ship.o cell.o board.o ai.o sea_battle.o 

%.o: %.cpp %.hpp
		$(CXX) $(CXXFLAGS) -c $< -o $@

sea_battle: main.cpp $(OBJMODULES)
		$(CXX) $(CXXFLAGS) $^ -o $@
