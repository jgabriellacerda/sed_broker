#ifndef DATATYPES_H
#define DATATYPES_H
#include <string>
#include <map>
#include <chrono>
#include <ctime>
#include <json.hpp>
using namespace std;
using namespace std::chrono;



struct SVData
{
	float Phsmeas[8][8];
	int SmpCnt[8];
	string sv_id;
	long timestamp;
	bool status;
	long arrivetimestampfirst;
	long arrivetimestamplast;
	long time1layerfirst;
	long time1layerlast;
	long timedataSenderlast;
	long timedataSenderfirst;
};
#endif
