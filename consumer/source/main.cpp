#include <consumer.h>
#include <iostream>
#include <stdio.h>
using namespace std;


int main(int argc, char** argv)
{


  Consumer c("rabbitmq",5672,"dam","password","queue1");

  c.dataConsumer();


} // end of main
