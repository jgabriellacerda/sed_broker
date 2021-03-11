#!/bin/sh
ldconfig
mv /json.hpp ./json.hpp 2>/dev/null
g++ -std=c++11 -Wall -fexceptions -g -pthread -I header -c source/main.cpp -o main.o
g++ -std=c++11 -Wall -fexceptions -g -pthread -I header -c source/consumer.cpp -o consumer.o
g++ -std=c++11 -o consumer_module main.o consumer.o -lpthread -lrabbitmq -lSimpleAmqpClient
LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu/ ./consumer_module
