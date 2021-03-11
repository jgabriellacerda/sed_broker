#ifndef CONSUMER_H
#define CONSUMER_H

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <datatypes.h>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

class Consumer{

  protected:
    AmqpClient::Channel::ptr_t connection;
    string amqp_host;
    int amqp_port;
    string amqp_username;
    string amqp_password;
    string queue_name;
    string consumer_tag;
    long consumertimestamp;
    json messageBody;
    SVData data;
    string it;
    //string svdata_exchange_name;
    //string routing_key;
  public:
    Consumer(string host, int port, string username, string password, string queue_name );
    void setAmqpParameters(string host, int port, string username, string password, string queue_name);
    void amqpConnect();
    void dataConsumer();
    void from_json(const json& j, SVData& p);
    //void to_json(json& j, const SVData& p);

};

#endif
