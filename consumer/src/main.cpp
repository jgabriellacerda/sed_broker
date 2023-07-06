#include <iostream>
#include <cppkafka/cppkafka.h>
#include "cppkafka/configuration.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <functional>
#include <cstdlib>

#include <json.hpp>
#include <broker.h>
#include <consumer.h>

using json = nlohmann::json;
using namespace std;
using cppkafka::Configuration;
using cppkafka::Consumer;
using cppkafka::Message;
using cppkafka::TopicPartitionList;

int main(int argc, char **argv)
{
  string group_id = "foo";
  const char *env_group = std::getenv("CONSUMER_GROUP");
  if (env_group)
  {
    std::cout << "Your Consumer Group is: " << env_group << '\n';
    group_id = env_group;
  }

  // Consumer c("rabbitmq",5672,"dam","password","gabriela");

  // c.dataConsumer();
  printf("Running consumer...\n");

  // Create a configuration and set the group.id and broker list fields
  Configuration config = {
      {"metadata.broker.list", "kafka-server1:9092"},
      {"group.id", group_id},
      {"queue.buffering.max.ms", 0},
      {"fetch.wait.max.ms", 0}};

  string topic = "my_topic";

  KafkaConsumer broker_consumer(config, topic);
  // RabbitMQConsumer broker_consumer("rabbitmq", 5672, "dam", "password", "gabriela");

  MessageConsumer msg_consumer(broker_consumer, group_id);

  msg_consumer.dataConsumer();

} // end of main
