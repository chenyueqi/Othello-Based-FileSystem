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
	string testOp;
	bool getDcLabel(uint8_t& dcLabel);
	bool dataflowAna(dataflow* stat);

	uint64_t intradata;
	uint64_t interdata;

	uint32_t intracnt;
	uint32_t intercnt;

	uint32_t totalopcnt;


    public:
	Client(Gateway* p = NULL):gateWay(p){
	    srand((unsigned)time(NULL));
	    intradata = 0;
	    intradata = 0;

	    intercnt = 0;
	    intercnt = 0;

	    totalopcnt = 0;
	}
	bool setGateWay(Gateway* p){gateWay = p;}
	bool sendMessage(string message);
	bool setTestOp(string op){testOp = op;}
	bool getStat();
};

bool Client::getDcLabel(uint8_t& dcLabel)
{
    dcLabel = rand()%dcNum;
    return true;
}

bool Client::getStat()
{
    fprintf(stdout, "total related operation time:\t%u\n", totalopcnt);
    fprintf(stdout, "intradatacenter access time:\t%u\n", intracnt);
    fprintf(stdout, "intradatacenter data flow:\t%lu\n", intradata);
    fprintf(stdout, "interdatacenter access time:\t%u\n", intercnt);
    fprintf(stdout, "interdatacenter data flow:\t%lu\n", interdata);
    fprintf(stdout, "intra: %f-%f\t, inter: %f-%f\n", (double)intracnt/(double)totalopcnt, (double)intradata/(double)totalopcnt, (double)intercnt/(double)totalopcnt, (double)interdata/(double)totalopcnt);
    return true;
}

bool Client::dataflowAna(dataflow dataflowStat[2])
{
    totalopcnt++;
    intracnt += dataflowStat[0].cnt;
    intradata += dataflowStat[0].size;

    intercnt += dataflowStat[1].cnt;
    interdata += dataflowStat[1].size;
//    fprintf(stdout, "intra: %u-%lu\t inter: %u-%lu\n", dataflowStat[0].cnt, dataflowStat[0].size, dataflowStat[1].cnt, dataflowStat[1].size);
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

    if(!op.compare(testOp))
	dataflowAna(dataflowStat);
    return true;
};

#endif
