#ifndef CONSUMER_H
#define CONSUMER_H

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <iostream>
//#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <datatypes.h>
#include <json.hpp>
#include <broker.h>

using namespace std;
using json = nlohmann::json;

class MessageConsumer
{

protected:
  long int id;
  long consumertimestamp;
  json message_json;
  SVData data;
  string message_payload;
  BrokerConsumer &consumer;
public:
  MessageConsumer(BrokerConsumer &consumer, long int id);
  void dataConsumer();
  void from_json(const json &j, SVData &p);
};

#endif
