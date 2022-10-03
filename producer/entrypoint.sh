#!/bin/sh
ldconfig
mv /json.hpp ./json.hpp 2>/dev/null
g++ -std=c++11 -Wall -fexceptions -g -pthread -I inc -I libs/inc -c src/main.cpp -o bin/main.o
g++ -std=c++11 -Wall -fexceptions -g -pthread -I inc -I libs/inc -c src/sniffer.cpp -o bin/sniffer.o
g++ -std=c++11 -Wall -fexceptions -g -pthread -I libs/inc -c libs/src/broker.cpp -o bin/broker.o
g++ -std=c++11 -o acquisition_module bin/broker.o bin/sniffer.o bin/main.o -lpthread -lcppkafka -lSimpleAmqpClient
LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu/ ./acquisition_module
