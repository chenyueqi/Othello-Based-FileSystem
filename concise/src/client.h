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
	map<string, uint64_t> directoryId;
	bool getDirId(string name, uint64_t &id);
	bool getFaId(string name, uint64_t &id);

    public:
	Client(Gateway* p = NULL):gateWay(p){}
	bool setGateWay(Gateway* p){
	    gateWay = p; 
	    directoryId.insert(pair<string, uint64_t>("/", 0));
	}
	bool sendMessage(string message);
};

bool Client::getDirId(string name, uint64_t &id)
{
    map<string, uint64_t>::iterator iter = directoryId.find(name);

    if(iter == directoryId.end()){
	fprintf(stderr, "directory %s does not exist %s %d\n", name.c_str(), __FILE__, __LINE__);
	return false;
    }
    id = iter->second;
    return true;
}

bool Client::getFaId(string name, uint64_t &id)
{
    while(true){
	int i = 0;
	for(i = name.size(); i > 1 && name[i] != '/'; i--);
	string name = name.substr(0,i);

	map<string, uint64_t>::iterator iter = directoryId.find(name);
	if(iter != directoryId.end()){
	    id = iter->second;
	    return true;
	}
    }
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

    map<string, uint64_t> newdir;

    if(!op.compare("mkdir") || !op.compare("rm") || !op.compare("write")  || !op.compare("read") || !op.compare("touch")){
	string path;
	getline(me, path, ' ');
	uint64_t id;
	getFaId(path, id);
	gateWay->getMessage(op, path, "", id, 0, newdir);
    }

    else if(!op.compare("ls") || !op.compare("rmr")){
	string path;
	getline(me, path, ' ');
	uint64_t id;
	getDirId(path, id);
	gateWay->getMessage(op, path, "", id, 0, newdir);
    }

    else if(!op.compare("mv")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2;
	getFaId(path1, id1);
	getDirId(path2, id2);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir);
    }

    else if(!op.compare("mvr")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2;
	getDirId(path1, id1);
	getDirId(path2, id2);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir);
    }

    else if(!op.compare("cp")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2;
	getFaId(path1, id1);
	getFaId(path2, id2);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir);
    }

    else{
	fprintf(stderr, "INVALID operation %s %d\n", __FILE__, __LINE__);
	return false;
    }

    for(map<string, uint64_t>::iterator iter = newdir.begin(); iter != newdir.end(); iter++)
	directoryId.insert(pair<string, uint64_t>(iter->first, iter->second));
    return true;
};

#endif
