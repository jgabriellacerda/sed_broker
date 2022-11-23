#!/bin/sh
ldconfig
mv /json.hpp ./json.hpp 2>/dev/null
g++ -std=c++11 -Wall -fexceptions -g -pthread -I header -I libs/inc -c source/main.cpp -o bin/main.o
g++ -std=c++11 -Wall -fexceptions -g -pthread -I header -I libs/inc -c source/consumer.cpp -o bin/consumer.o
g++ -std=c++11 -Wall -fexceptions -g -pthread -I libs/inc -c libs/src/broker.cpp -o bin/broker.o
g++ -std=c++11 -o consumer_module bin/broker.o bin/consumer.o bin/main.o -lpthread -lcppkafka -lSimpleAmqpClient
LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu/ ./consumer_module
