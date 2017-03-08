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
  stringstream me(message);
  string op;
  getline(me, op, ' ');

  if (op == "mkdir" || op == "rm" || op == "read" || 
	  op == "touch" || op == "rmr"|| op == "ls") {
	string path;	
	getline(me, path, ' ');
	gateWay->getMessage(op, path, "", 0);
  } else if(!op.compare("write")) {
	string path;
	getline(me, path, ' ');
	uint64_t size = 0;
	me>>size;
	gateWay->getMessage(op, path, "", size);
  } else if(!op.compare("mv") || !op.compare("mvr") || !op.compare("cp")) {
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	gateWay->getMessage(op, path1, path2, 0);
  } else if(!op.compare("exit")) {
	uint32_t num;
	me>>num;
	gateWay->getMessage(op, "", "", num);
  } else if(!op.compare("check")) {
	string path;
	getline(me, path, ' ');
	uint64_t size = 0;
	me>>size;
	gateWay->getMessage(op, path, "", size);
  } else {
	fprintf(stderr, "INVALID operation %s %d\n", __FILE__, __LINE__);
	return false;
  }
};

#endif
