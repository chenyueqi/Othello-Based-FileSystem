#ifndef __CLIENT_H__
#define __CLIENT_H__

#include<iostream>
#include<sstream>
#include<cstdio>
#include<cstdlib>
#include"gateway.h"
using namespace std;

class Client
{
    private:
	Gateway* gateWay;
	bool getDcLabel(uint8_t& dcLabel);
	bool dataflowAna(dataflow* stat);

    public:
	Client(Gateway* p = NULL):gateWay(p){srand((unsigned)time(NULL));}
	bool setGateWay(Gateway* p){gateWay = p;}
	bool sendMessage(string message);
};

bool Client::getDcLabel(uint8_t& dcLabel)
{
    dcLabel = rand()%(1<<dcBit);
    return true;
}

bool Client::dataflowAna(dataflow dataflowStat[2])
{
    fprintf(stdout, "intra: %u-%lu\t inter: %u-%lu\n", dataflowStat[0].cnt, dataflowStat[0].size, dataflowStat[1].cnt, dataflowStat[1].size);
    return true;
}
/*
 * support mkdir, list, recursive remove, recursive move for directory
 * support remove, write, read, touch, move, cp for file
 */
bool Client::sendMessage(string message)
{
    stringstream me(message);
    string op;
    getline(me, op, ' ');

    uint8_t dcLabel = 0;

    dataflow dataflowStat[2];
    dataflowStat[0].cnt = 0;
    dataflowStat[0].size = 0;
    dataflowStat[1].cnt = 0;
    dataflowStat[1].size = 0;


    if(!op.compare("mkdir") || !op.compare("rm") || !op.compare("read") || !op.compare("touch")){
	string path;	
	getline(me, path, ' ');
	getDcLabel(dcLabel);
	gateWay->getMessage(op, path, "", 0, dcLabel, dataflowStat);
	dataflowAna(dataflowStat);
    }

    else if(!op.compare("write")){
	string path;
	getline(me, path, ' ');
	uint64_t size = 0;
	me>>size;
	getDcLabel(dcLabel);
	gateWay->getMessage(op, path, "", size, dcLabel, dataflowStat);
    }

    else if(!op.compare("rmr")){
	string path;
	getline(me, path, ' ');
	getDcLabel(dcLabel);
	gateWay->getMessage(op, path, "", 0, dcLabel, dataflowStat);
    }

    else if(!op.compare("ls")){
	string path;
	getline(me, path, ' ');
	getDcLabel(dcLabel);
	gateWay->getMessage(op, path, "", 0, dcLabel, dataflowStat);
    }

    else if(!op.compare("mv")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	getDcLabel(dcLabel);
	gateWay->getMessage(op, path1, path2, 0, dcLabel, dataflowStat);
    }

    else if(!op.compare("mvr")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	getDcLabel(dcLabel);
	gateWay->getMessage(op, path1, path2, 0, dcLabel, dataflowStat);
    }

    else if(!op.compare("cp")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	getDcLabel(dcLabel);
	gateWay->getMessage(op, path1, path2, 0, dcLabel, dataflowStat);
    }

    else{
	fprintf(stderr, "INVALID operation %s %d\n", __FILE__, __LINE__);
	return false;
    }


    return true;
};

#endif
