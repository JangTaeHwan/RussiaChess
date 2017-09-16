CXX=g++
CFLAGS= -std=c++14 -Wl,-stack_size -Wl,1000000

all:
	$(CXX) $(CFLAGS) $(LIBS) russia_chess.cpp 
