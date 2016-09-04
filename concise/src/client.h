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

bool Client::sendMessage(string message)
{
    stringstream me(message);
    string op;
    getline(me, op, ' ');

    if(!op.compare("mkdir") || !op.compare("ls") || !op.compare("rm") || !op.compare("rmr") || !op.compare("write")  || !op.compare("read")){
	string path;
	getline(me, path, ' ');
	return gateWay->getMessage(op, path, "");
    }

    else if(!op.compare("mv") || !op.compare("mvr") || !op.compare("cp")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	return gateWay->getMessage(op, path1, path2);
    }

    else{
	fprintf(stderr, "INVALID operation %s %d\n", __FILE__, __LINE__);
	return false;
    }
};

#endif
