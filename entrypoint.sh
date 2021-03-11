#!/bin/sh
ldconfig
mv /json.hpp ./json.hpp 2>/dev/null
g++ -std=c++11 -Wall -fexceptions -g -pthread -I inc -c src/main.cpp -o main.o
g++ -std=c++11 -Wall -fexceptions -g -pthread -I inc -c src/sniffer.cpp -o sniffer.o
g++ -std=c++11 -o acquisition_module main.o sniffer.o -lpthread -lrabbitmq -lSimpleAmqpClient
LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu/ ./acquisition_module
