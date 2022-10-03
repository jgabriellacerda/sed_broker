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
  // AmqpClient::Channel::ptr_t connection;
  // string amqp_host;
  // int amqp_port;
  // string amqp_username;
  // string amqp_password;
  // string queue_name;
  // string consumer_tag;
  long consumertimestamp;
  json message_json;
  SVData data;
  string message_payload;

  BrokerConsumer &consumer;

  // string svdata_exchange_name;
  // string routing_key;
public:
  MessageConsumer(BrokerConsumer &consumer);
  // MessageConsumer(string host, int port, string username, string password, string queue_name);
  // void setAmqpParameters(string host, int port, string username, string password, string queue_name);
  // void amqpConnect();
  void dataConsumer();
  void from_json(const json &j, SVData &p);
  // void to_json(json& j, const SVData& p);
};

#endif
