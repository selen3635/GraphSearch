CC=g++
CXXFLAGS=-std=c++11 -g -O3 -Wall -gdwarf-2
LDFLAGS=-g

actorconnections:actorconnections.cpp
pathfinder:pathfinder.cpp

clean:
	rm -f pathfinder actorconnections *.o core* *~

