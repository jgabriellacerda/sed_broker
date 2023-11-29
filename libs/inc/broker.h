#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <iostream>

#include <json.hpp>
#include <cppkafka/cppkafka.h>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

using namespace cppkafka;
using namespace std;
using namespace AmqpClient;
using json = nlohmann::json;

class BrokerProducer
{
public:
  string broker_name;
  virtual void publish(string message) = 0;
};

class BrokerConsumer
{
public:
  string broker_name;
  virtual string listen() = 0;
};

class KafkaProducer : public BrokerProducer
{
public:
  KafkaProducer(Configuration config);
  Producer producer;
  void publish(string message);
};

class KafkaConsumer : public BrokerConsumer
{
public:
  KafkaConsumer(Configuration config, string topic);
  Consumer consumer;
  string listen();
};

class RabbitMQProducer : public BrokerProducer
{
public:
  string amqp_host;
  int amqp_port;
  string amqp_username;
  string amqp_password;
  string svdata_exchange_name;
  string routing_key;
  AmqpClient::Channel::ptr_t connection;
  void amqpConnect();
  void publish(string message);
  RabbitMQProducer(string host, int port, string username, string password, string svdata_exchange_name, string key);
};

class RabbitMQConsumer : public BrokerConsumer
{
public:
  string amqp_host;
  int amqp_port;
  string amqp_username;
  string amqp_password;
  string queue_name;
  string consumer_tag;
  string svdata_exchange_name;
  string routing_key;
  AmqpClient::Channel::ptr_t connection;
  void amqpConnect();
  string listen();
  RabbitMQConsumer(
      string host,
      int port,
      string username,
      string password,
      string queue_name,
      string svdata_exchange_name,
      string routing_key);
};

#endif