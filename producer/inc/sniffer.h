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
// #include <SimpleAmqpClient/SimpleAmqpClient.h>

using namespace std;

class Sniffer
{
protected:
  string base_cmd = "tshark ";
  map<string, vector<string>> params;
  std::thread t;
  BrokerProducer &broker;
  bool syncing = true;
  bool infiniteloop = false;
  long timeseg;
  string cmd();
  SVData parser(char *cLine);
  void dispatch(SVData);

public:
  Sniffer(string filepath, bool il, BrokerProducer &connection);
  void start();
  void method();
  void initTimestamp(int);
  long updateTimestamp(int);
  void dataSender(SVData);
};

#endif
