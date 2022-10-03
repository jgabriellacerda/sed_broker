#include <sniffer.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <cstring>
#include <chrono>
#include <ctime>
#include <string>
#include <sniffer.h>
#include <json.hpp>

using namespace std::chrono;
using namespace std;
using namespace cppkafka;
// using namespace AmqpClient;
using json = nlohmann::json;

/*Sniffer::Sniffer(string dst_mac,string host, int port, string username, string password, string svdata_exchange_name, string key)
{
  cout<<"Aquisition module has been started ...."<<endl;
  timeseg = -1;
  string mac_filter = "'ether dst "; mac_filter+=dst_mac; mac_filter +="'";
  params["q"].push_back(" ");
  params["l"].push_back(" ");
  params["o"].push_back("sv.decode_data_as_phsmeas:TRUE");
  params["f"].push_back(mac_filter);
  params["b"].push_back("files:2");
  params["b"].push_back("filesize:100000");
  string tmpfile = "/tmp/cm_cap_";
  tmpfile+=dst_mac;
  tmpfile+=".tmp";
  params["w"].push_back(tmpfile);
  params["e"].push_back("sv.meas_value");
  params["e"].push_back("sv.smpCnt");
  params["e"].push_back("sv.svID");
  params["Tfields"].push_back(" ");
  this->setAmqpParameters(host, port, username, password, svdata_exchange_name,key);
  this->amqpConnect();
  FILE * out = popen("ls /sys/class/net/", "r");
  const int max_buffer = 1000;
  char buffer[max_buffer];
  while (fgets(buffer, sizeof buffer, out) != NULL) {
    params["i"].push_back(strtok(buffer,"\n"));
  }
}*/

// Sniffer::Sniffer(string filepath, bool il, string host, int port, string username, string password, string svdata_exchange_name, string key)
// {
//   cout << "Aquisition module has been started ...." << endl;
//   this->timeseg = -1;
//   this->infiniteloop = il;
//   params["r"].push_back(filepath);
//   params["Y"].push_back("sv");
//   params["o"].push_back("sv.decode_data_as_phsmeas:TRUE");
//   params["e"].push_back("sv.meas_value");
//   params["e"].push_back("sv.smpCnt");
//   params["e"].push_back("sv.svID");
//   params["Tfields"].push_back(" ");
//   // this->setAmqpParameters(host, port, username, password, svdata_exchange_name,key);
//   // this->amqpConnect();
// }

Sniffer::Sniffer(string filepath, bool il, BrokerProducer &producer) : broker(producer)
{
  cout << "Aquisition module has been started ...." << endl;
  this->timeseg = -1;
  this->infiniteloop = il;
  params["r"].push_back(filepath);
  params["Y"].push_back("sv");
  params["o"].push_back("sv.decode_data_as_phsmeas:TRUE");
  params["e"].push_back("sv.meas_value");
  params["e"].push_back("sv.smpCnt");
  params["e"].push_back("sv.svID");
  params["Tfields"].push_back(" ");
}

string Sniffer::cmd()
{
  ostringstream stringStream;
  stringStream << this->base_cmd;
  for (auto const &item : this->params)
  {
    for (int i = 0; i < (int)params[item.first].size(); i++)
    {
      stringStream << "-" << item.first << " " << params[item.first][i] << " ";
    }
  }
#if DEBUG > 1
  cout << "Sniffing command: " << stringStream.str() << endl;
#endif // DEBUG
  return stringStream.str();
}

void Sniffer::start()
{
  // this->t = thread(&Sniffer::method,this);

  // cout << "Sniffer::start()" << endl;

  this->method();
}

void Sniffer::method()
{
  // cout << "Sniffer::method()" << endl;

  // Create a new stream connected to a pipe running tshark...
  do
  {

    FILE *fpa = popen(this->cmd().c_str(), "r");

    if (fpa == NULL)
    {
      printf("Error calling popen.\n");
    }
    // Reading data values from pipe and storing in a vector...
    char path[PATH_MAX];
    while (fgets(path, PATH_MAX, fpa) != NULL)
    {
      // auto inicio3 = std::chrono::high_resolution_clock::now();
      this->dispatch(this->parser(path));
      // auto resultado3 = std::chrono::high_resolution_clock::now() - inicio3;
      // long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(resultado3).count();
      // cout<<"Method:"<<microseconds<<endl;
    }

  } while (this->infiniteloop);
}

SVData Sniffer::parser(char *cLine)
{

  // cout << "Sniffer::parser()" << endl;

#if (DEBUG > 3)
  printf("%s\n", cLine);
#endif
  // amostragem de tempo

  SVData data;

  data.arrivetimestamp_producer = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  // this->teste = (long)std::chrono::high_resolution_clock::now();

  string testestr(cLine);
  char seps[] = " ,\t\n";
  int iCol = 0, iLine = 0;
  char *token = strtok(cLine, seps);

  try
  {
    while (token)
    {
      if (iLine == 9)
      {
        data.sv_id = token;
        return data;
      }
      // Store voltage and current values (Phsmeas)
      if (iLine < NSAMPLES)
      {
        if (iCol < 4)
        { // Currents
          data.Phsmeas[iLine][iCol] = atof(token) * 1.0E-3;
        }
        else
        { // Voltages
          data.Phsmeas[iLine][iCol] = atof(token) * 1.0E-2;
        }

#if (DEBUG > 2)
        printf("Line: %d\tColumn: %d\tValue: %f\n", iLine, iCol, data.Phsmeas[iLine][iCol]);
#endif
      }
      else
      { // Store sample count values
        data.SmpCnt[iCol] = atoi(token);

        if (iCol == 0)
        {
          data.timestamp = updateTimestamp(data.SmpCnt[iCol]);
        }

#if (DEBUG > 2)
        printf("Index: %d\tValue is: %d\n", iCol, data.SmpCnt[iCol]);
#endif
      }
      token = strtok(0, seps);
      iCol++;
      if (iCol > 7)
      { // Reset column index and increment line number
        iCol = 0;
        iLine++;
      }
    } // end while
    // auto resultado1 = std::chrono::high_resolution_clock::now() - inicio1;
    // this->timeparser = std::chrono::duration_cast<std::chrono::microseconds>(resultado1).count();
    // cout<<"Parser:"<<this->timeparser<<"   "<<endl;
  }
  catch (exception &e)
  {
    cout << e.what() << endl;
  }

  return data;
}

void Sniffer::dispatch(SVData parsedData)
{

  // cout << "Sniffer::dispatch()" << endl;

  // auto inicio = std::chrono::high_resolution_clock::now();
  if (this->syncing)
  {
    if ((parsedData.timestamp / 1000000) % 60 == 1 && parsedData.SmpCnt[0] == 0) // Os pacotes começarão a ser publicados no segundo zero e SmpCnt 0
    {
      this->syncing = false;
      cout << "Acquisition module synchronized." << endl;
    }
    else
    {
      return;
    }
  }
  // auto resultado = std::chrono::high_resolution_clock::now() - inicio;
  // this->timerdispatch = std::chrono::duration_cast<std::chrono::microseconds>(resultado).count();
  else
  {
    dataSender(parsedData);
  }
}

void Sniffer::dataSender(SVData data) // Rotina de publicação dos dados para o RabbitMQ ou Kafka
{

  // cout << "Sniffer::dataSender()" << endl;

  try
  {
    // auto inicio2 = std::chrono::high_resolution_clock::now();
    static json message;
    json j;
    static int i = 0;
    map<int, vector<float>> samples;
    for (int s = 0; s < NSAMPLES; s++)
    {
      for (int ch = 0; ch < NCH; ch++)
      {
        samples[data.SmpCnt[s]].push_back(data.Phsmeas[s][ch]);
      }
    }
    // this->teste2 = std::chrono::high_resolution_clock::now() - this->teste;
    // this->teste3 = std::chrono::duration_cast<std::chrono::microseconds>(this->teste2).count();
    // cout<<"DataSender:"<<this->timedataSender<<"   "<<endl;
    // cout<<"Parser:"<<this->timeparser<<"   "<<endl;;
    // data.timefirstlayer= this->timeparser +  this->timedataSender;

    // j["timefirstlayer"]=data.timefirstlayer;
    j["timestamp"] = data.timestamp;
    j["status"] = data.status;
    j["samples"] = samples;
    j["sv_id"] = data.sv_id;
    j["arrivetimestamp_producer"] = data.arrivetimestamp_producer;

    if (i < BATCHSIZE)
    {
      message[i] = j;

      // cout << message.dump() << endl;
      i++;
    }
    else
    {
      i = 0;
      // BasicMessage::ptr_t message_str = BasicMessage::Create(message.dump());

      // cout << message.dump() << endl;
      this->broker.publish(message.dump());
      // return;
      // this->connection->producer.flush();
    }
  }
  catch (exception &e)
  {
    cout << "Erro na conexão com o broker: " << e.what() << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void Sniffer::initTimestamp(int SmpCnt) // Rotina para inicialização da estampa de tempo
{
  auto time = std::chrono::system_clock::now();
  auto tmsmp = std::chrono::time_point_cast<std::chrono::microseconds>(time);

  long microsecNTP = (tmsmp.time_since_epoch().count()) % 1000000;
  long microsecMU = SmpCnt * TS * 1000000;

  this->timeseg = (microsecNTP > microsecMU) ? ((tmsmp.time_since_epoch().count()) / 1000000) : (((tmsmp.time_since_epoch().count()) / 1000000) - 1);
}

long Sniffer::updateTimestamp(int SmpCnt) // Rotina para atualização da estampa de tempo
{

  if (this->timeseg < 0)
  {
    initTimestamp(SmpCnt);
    if (SmpCnt == 0)
    {
      return this->timeseg * 1000000;
    }
  }
  long microsecMU = SmpCnt * TS * 1000000;

  if (SmpCnt == 0)
  {
    this->timeseg++;
  }

  return this->timeseg * 1000000 + microsecMU;
}
