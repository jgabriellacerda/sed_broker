#include <sniffer.h>
#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, char** argv)
{
  Sniffer s("SVRecording.pcap",true,"rabbitmq",5672,"dam","password","process_bus_data","sampled_values");
  s.start();
} // end of main
