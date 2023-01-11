#include <consumer.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <cstring>
#include <chrono>
#include <string>
#include <json.hpp>

using namespace std;
// using namespace AmqpClient;
using json = nlohmann::json;

MessageConsumer::MessageConsumer(BrokerConsumer &consumer, string group_id) : consumer(consumer)
{
  this->group_id = group_id;
}

// consumir dados da fila
void MessageConsumer::dataConsumer()
{
  std::ofstream times_file;
  long delta_time;
  long times_buffer[BUFFERSIZE];
  // json j_message;
  int n = 0;
  int test_number = 0;

  bool try_consume = true;
  int num_attempts = 20;
  bool write_buffer = true;
  bool write_file = false;
  // while (try_consume)
  // {
  try
  {
    do
    {

      this->message_payload = this->consumer.listen();

      // amostrar aqui!
      this->consumertimestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      this->message_json = json::parse(this->message_payload);

      from_json(this->message_json, this->data);

      delta_time = (this->consumertimestamp - data.arrivetimestamp_producer);
      // long diff1 = (this->consumertimestamp - data.arrivetimestampfirst);
      // long diff2= (this->consumertimestamp - data.arrivetimestamplast);

      // cout<< data.arrivetimestamp << "   " << this->consumertimestamp<<endl;}
      if (write_buffer)
      {
        if (n < BUFFERSIZE)
        {
          times_buffer[n] = delta_time;
          n++;
        }
        else
        {
          n = 0;
          write_buffer = false;
          write_file = true;
        }
      }
      else if (write_file)
      {
        cout << "Writing file..." << endl;
        times_file.open("data/" + this->consumer.broker_name + "_" + this->group_id + "_times_" + to_string(consumertimestamp) + ".txt");
        // times_file.open("data/teste");
        for (int p = 0; p < BUFFERSIZE; p++)
        {
          times_file << times_buffer[p] << endl;
          // cout << times_buffer[p] << endl;
        }

        cout << "File saved! Test number: " << ++test_number << endl;
        times_file.close();

        write_buffer = true;
        write_file = false;
      }
      else
      {
        cout << delta_time << endl;
      }

    }
    // cout<< data.sv_id<< endl;}
    while (try_consume);
  }
  catch (exception &e)
  {
    num_attempts--;
    if (num_attempts == 0)
    {
      try_consume = false;
      cout << "Não foi possível consumir:" << e.what() << "." << endl;
    }
    else
    {
      cout << "Erro no consumo do broker:" << e.what() << ". Tentativas restantes: " << num_attempts << endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  times_file.open("data/" + this->consumer.broker_name + "_times.txt");
  for (int p = 0; p < BUFFERSIZE; p++)
  {
    times_file << times_buffer[p] << endl;
    cout << times_buffer[p] << endl;
  }

  cout << "\End of consuming!\n";
  times_file.close();
}

// pegar informações json e transformar para a struct
void MessageConsumer::from_json(const json &j, SVData &p)
{
  j[0].at("arrivetimestamp_producer").get_to(p.arrivetimestamp_producer);
  // j[2].at("arrivetimestamp").get_to(p.arrivetimestamplast);
  // j[0].at("timestamp").get_to(p.timestamp);
  j[0].at("status").get_to(p.status);
  j[0].at("sv_id").get_to(p.sv_id);
}
