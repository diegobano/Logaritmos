OBJS = pruebas.o hashing.o patricia.o
HEADERS = hashing.hpp patricia.hpp ternario.hpp dictionary.hpp
FILES = hashing.cpp pruebas.cpp patricia.cpp ternario.cpp
CC = g++
DEBUG = -g
CFLAGS = -std=c++11 -Wall -c $(DEBUG)
LFLAGS = -std=c++11 -Wall $(DEBUG)

test : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o test

hashing.o: hashing.cpp hashing.hpp
	$(CC) $(CFLAGS) hashing.cpp

patricia.o: patricia.cpp patricia.hpp
	$(CC) $(CFLAGS) patricia.cpp

ternario.o: ternario.cpp ternario.hpp
	$(CC) $(CFLAGS) ternario.cpp

pruebas.o: pruebas.cpp $(HEADERS)
	$(CC) $(CFLAGS) pruebas.cpp

clean:
	\rm *.o test formateado*

tar:
	tar cfv tarea2.tar $(FILES) $(HEADERS)