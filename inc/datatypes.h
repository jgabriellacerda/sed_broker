#ifndef DATATYPES_H
#define DATATYPES_H
#include <parameters.h>
#include <string>
#include <map>
#include <chrono>
#include <ctime>
using namespace std;
using namespace std::chrono;

struct SVData
{
	float Phsmeas[NSAMPLES][NCH];
	int SmpCnt[NSAMPLES];
	string sv_id;
	long timestamp;
	bool status;
	long arrivetimestamp;
	long time1layer;
	long timedataSender ;
	long timeParser;

};

#endif
