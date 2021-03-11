#include <consumer.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <cstring>
#include <chrono>
#include <string>
#include <json.hpp>

#define NAMOSTRAS 1000

//colocar número de pacotes -1
#define BATCHSIZE 49

using namespace std;
using namespace AmqpClient;
using json = nlohmann::json;

Consumer::Consumer(string host, int port, string username, string password, string queue_name)
{

  this->setAmqpParameters(host, port, username, password, queue_name);
  this->amqpConnect();
}

void Consumer::setAmqpParameters(string host, int port, string username, string password, string queue_name)
{
  this->amqp_host = host;
  this->amqp_port = port;
  this->amqp_username = username;
  this->amqp_password = password;
  this->queue_name = queue_name;

}

void Consumer::amqpConnect()
{
  int num_attempts = 20;
  while(num_attempts--)
  {
    try
    {
      this->connection = AmqpClient::Channel::Create(this->amqp_host, this->amqp_port, this->amqp_username, this->amqp_password);
      this->connection->DeclareQueue(this->queue_name, false, false, false, true);
      this->connection->BindQueue(this->queue_name,"process_bus_data","sampled_values");
      this->consumer_tag = this->connection->BasicConsume(this->queue_name,"");


      cout<<"Conexão com RabbitMQ realizada com sucesso."<<endl;
    }
    catch(exception& e)
    {
      cout<<"Erro na conexão com o RabbitMQ:"<< e.what()<<" Tentativas restantes: "<<num_attempts<<endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

void Consumer::dataConsumer(){
  int num_attempts = 20;
    try
    {
        ofstream myfile;
        int h=0;
        string amostras[NAMOSTRAS];
        int temposcamada2[NAMOSTRAS];
        int consumertimestampvector[NAMOSTRAS];
        int times1layer[NAMOSTRAS];
      do{
      Envelope::ptr_t envelope = this->connection->BasicConsumeMessage(consumer_tag);
      //amostrar
      consumertimestampvector[h]= std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      BasicMessage::ptr_t bodyBasicMessage=envelope->Message();

      this->it=bodyBasicMessage->Body();
      amostras[h]=this->it;
      h++;

    } while (h<NAMOSTRAS);

      for(int z=0;z<NAMOSTRAS;z++){

          this->messageBody= json::parse(amostras[z]);

          from_json(this->messageBody,this->data);
          long diff1= (consumertimestampvector[z] - data.timedataSenderlast);

          long diff2= (data.timedataSenderlast-data.arrivetimestampfirst);

          //times1layer[h]= data.time1layer;
          times1layer[z]= diff2;
          temposcamada2[z]=diff1;
    }


    myfile.open("tamanho1000.txt");
    for(int p=0;p<NAMOSTRAS;p++){
    myfile<<temposcamada2[p]<<" "<<times1layer[p]<<endl;
    }



    cout<<endl<<"FIM"<<endl;
    myfile.close();}



      catch(exception& e)
      {
        cout<<"Erro no consumo RabbitMQ:"<< e.what()<<" Tentativas restantes: "<<num_attempts<<endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }


  }


//pegar informações json e transformar para a struct
void Consumer::from_json(const json& j, SVData& p) {
        j[0].at("arrivetimestamp").get_to(p.arrivetimestampfirst);
        j[BATCHSIZE].at("arrivetimestamp").get_to(p.arrivetimestamplast);

        //j[2].at("time1layer").get_to(p.time1layer);

        j[0].at("timedataSender").get_to(p.timedataSenderfirst);
        j[BATCHSIZE].at("timedataSender").get_to(p.timedataSenderlast);

    }
