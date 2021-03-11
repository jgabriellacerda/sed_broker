#ifndef SNIFFER_H
#define SNIFFER_H

#include <string>
#include <vector>
#include <map>
#include <datatypes.h>
#include <parameters.h>
#include <thread>
#include <SimpleAmqpClient/SimpleAmqpClient.h>


using namespace std;

class Sniffer{
protected:
  string base_cmd = "tshark ";
  map<string, vector<string>> params;
  string cmd();
  std::thread t;
  AmqpClient::Channel::ptr_t connection;
  string amqp_host;
  int amqp_port;
  string amqp_username;
  string amqp_password;
  string svdata_exchange_name;
  string routing_key;
  bool syncing = true;
  SVData parser(char* cLine);
  void dispatch(SVData);
  bool infiniteloop=false;
  long timeseg;

public:
  Sniffer(string dst_mac,string host, int port, string username, string password, string svdata_exchange_name,string key);
  Sniffer(string filepath, bool il, string host, int port, string username, string password, string svdata_exchange_name, string key);
  void start();
  void method();
  void initTimestamp(int);
  long updateTimestamp(int);
  void dataSender(SVData);
  void setAmqpParameters(string host, int port, string username, string password, string svdata_exchange_name,string key);
  void amqpConnect();
};


#endif
