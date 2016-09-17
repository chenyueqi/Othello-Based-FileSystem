#ifndef __CLIENT_H__
#define __CLIENT_H__

#include<iostream>
#include<sstream>
#include<cstdio>
#include"gateway.h"
using namespace std;

class Client
{
    private:
	Gateway* gateWay;

    public:
	Client(Gateway* p = NULL):gateWay(p){}
	bool setGateWay(Gateway* p){gateWay = p;}
	bool sendMessage(string message);
};

/*
 * support mkdir, list, recursive remove, recursive move for directory
 * support remove, write, read, touch, move, cp for file
 */
bool Client::sendMessage(string message)
{
    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;
    uint64_t otherTime = 0;
    
    stringstream me(message);
    string op;
    getline(me, op, ' ');

    if(!op.compare("mkdir") || !op.compare("rm") || !op.compare("read") || !op.compare("touch")){
	string path;
	getline(me, path, ' ');
	gateWay->getMessage(op, path, "", 0, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("write")){
	string path;
	getline(me, path, ' ');
	uint64_t size = 0;
	me>>size;
	gateWay->getMessage(op, path, "", size, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("rmr")){
	string path;
	getline(me, path, ' ');
	gateWay->getMessage(op, path, "", 0, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("ls")){
	string path;
	getline(me, path, ' ');
	gateWay->getMessage(op, path, "", 0, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("mv")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	gateWay->getMessage(op, path1, path2, 0, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("mvr")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	gateWay->getMessage(op, path1, path2, 0, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("cp")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	gateWay->getMessage(op, path1, path2, 0, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else{
	fprintf(stderr, "INVALID operation %s %d\n", __FILE__, __LINE__);
	return false;
    }

    //TODO get statistics result
    return true;
};

#endif
