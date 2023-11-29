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
  do
  {
    FILE *fpa = popen(this->cmd().c_str(), "r");
    if (fpa == NULL)
    {
      printf("Error calling popen.\n");
    }
    char path[PATH_MAX];
    while (fgets(path, PATH_MAX, fpa) != NULL)
    {
      this->dispatch(this->parser(path));
    }

  } while (this->infiniteloop);
}

SVData Sniffer::parser(char *cLine)
{
#if (DEBUG > 3)
  printf("%s\n", cLine);
#endif

  SVData data;

  data.arrivetimestamp_producer = std::chrono::duration_cast<std::chrono::microseconds>(
                                      std::chrono::system_clock::now().time_since_epoch())
                                      .count();
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
  }
  catch (exception &e)
  {
    cout << e.what() << endl;
  }

  return data;
}

void Sniffer::dispatch(SVData parsedData)
{
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
  else
  {
    dataSender(parsedData);
  }
}

void Sniffer::dataSender(SVData data) // Rotina de publicação dos dados para o RabbitMQ ou Kafka
{
  try
  {
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

    j["timestamp"] = data.timestamp;
    j["status"] = data.status;
    j["samples"] = samples;
    j["sv_id"] = data.sv_id;
    j["arrivetimestamp_producer"] = data.arrivetimestamp_producer;

    if (i < BATCHSIZE)
    {
      message[i] = j;
      i++;
    }
    else
    {
      i = 0;
      this->broker.publish(message.dump());
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
