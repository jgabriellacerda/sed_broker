#include <sniffer.h>
#include <iostream>
#include <stdio.h>
#include <cppkafka/cppkafka.h>
#include <broker.h>
#include <json.hpp>

using json = nlohmann::json;
using namespace std;
using namespace cppkafka;

int main(int argc, char **argv)
{
  // // Create the config
  Configuration config = {
      {"metadata.broker.list", "kafka-server1:9092"}};

  // KafkaProducer broker_producer(config);
  RabbitMQProducer broker_producer("rabbitmq", 5672, "dam", "password", "process_bus_data", "sampled_values");

  Sniffer s("SVRecording.pcap", false, broker_producer);

  int counter = 0;
  while (true)
  {
    counter++;
    s.start();
    cout << "\nPackets sent! Test number: " + to_string(counter) + ".\n";
  }
  // std::this_thread::sleep_for(std::chrono::seconds(5));
  return 0;
} // end of main
