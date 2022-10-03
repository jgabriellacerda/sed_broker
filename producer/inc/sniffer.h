#ifndef SNIFFER_H
#define SNIFFER_H

#include <string>
#include <vector>
#include <map>
#include <datatypes.h>
#include <parameters.h>
#include <thread>
#include <json.hpp>

#include <broker.h>

#include <cppkafka/cppkafka.h>
//#include <SimpleAmqpClient/SimpleAmqpClient.h>

using namespace std;

class Sniffer
{
protected:
  string base_cmd = "tshark ";
  map<string, vector<string>> params;
  std::thread t;

  // AmqpClient::Channel::ptr_t connection;
  BrokerProducer &broker;

  bool syncing = true;
  bool infiniteloop = false;

  long timeseg;
  long long timeparser;
  // long long timerdispatch;
  long long timedataSender;
  long int teste;
  long int teste2;
  long long teste3;

  string cmd();
  SVData parser(char *cLine);
  void dispatch(SVData);

public:
  // Sniffer(string dst_mac,string host, int port, string username, string password, string svdata_exchange_name,string key);
  // Sniffer(string filepath, bool il, string host, int port, string username, string password, string svdata_exchange_name, string key);
  Sniffer(string filepath, bool il, BrokerProducer &connection);
  void start();
  void method();
  void initTimestamp(int);
  long updateTimestamp(int);
  void dataSender(SVData);
  // void setAmqpParameters(string host, int port, string username, string password, string svdata_exchange_name,string key);
  // void amqpConnect();
};

#endif
