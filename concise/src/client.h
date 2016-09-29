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
	n，JavaScript语言程序。

	bool getFaId(string name, uint64_t &id);

    public:
	Client(Gateway* p = NULL):gateWay(p){}
	bool setGateWay(Gateway* p){
	    gateWay = p; 
	    directoryId.insert(pair<string, uint64_t>("/", 0));
	}
	bool sendMessage(string message);
	bool testDirID();
};

bool Client::testDirID()
{
    for(map<string, uint64_t>::iterator iter = directoryId.begin(); iter != directoryId.end(); iter++)
	fprintf(stdout, "%s %u\n", iter->first.c_str(), iter->second);
}

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

// get the id of name's father
bool Client::getFaId(string name, uint64_t &id)
{
    while(true){
	int i = 0;
	for(i = name.size(); i > 1 && name[i] != '/'; i--);
	string faName = name.substr(0,i);
	name = faName;

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
    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;
    uint64_t otherTime = 0;
    
    stringstream me(message);
    string op;
    getline(me, op, ' ');

    map<string, uint64_t> newdir;
    vector<string> olddir;

    if(!op.compare("mkdir") || !op.compare("rm") || !op.compare("read") || !op.compare("touch")){
	string path;
	getline(me, path, ' ');
	uint64_t id;
	getFaId(path, id);
	gateWay->getMessage(op, path, "", id, 0, newdir, olddir, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("write")){
	string path;
	getline(me, path, ' ');
	uint64_t id;
	getFaId(path, id);
	uint64_t size = 0;
	me>>size;
	gateWay->getMessage(op, path, "", id, size, newdir, olddir, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("rmr")){
	string path;
	getline(me, path, ' ');
	uint64_t id1, id2;
	getDirId(path, id1);
	getFaId(path, id2);
	gateWay->getMessage(op, path, "", id1, id2, newdir, olddir, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("ls")){
	string path;
	getline(me, path, ' ');
	uint64_t id;
	getDirId(path, id);
	gateWay->getMessage(op, path, "", id, 0, newdir, olddir, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("mv")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2;
	getFaId(path1, id1);
	getDirId(path2, id2);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir, olddir, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("mvr")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2;
	getDirId(path1, id1);
	getDirId(path2, id2);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir, olddir, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else if(!op.compare("cp")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2;
	getFaId(path1, id1);
	getFaId(path2, id2);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir, olddir, serverAcceCnt, dcAcceCnt, otherTime);
    }

    else{
	fprintf(stderr, "INVALID operation %s %d\n", __FILE__, __LINE__);
	return false;
    }

    for(vector<string>::iterator iter0 = olddir.begin(); iter0 != olddir.end(); iter0++){
	map<string, uint64_t>::iterator iter1 = directoryId.find(*iter0);
	directoryId.erase(iter1);
    }

    for(map<string, uint64_t>::iterator iter = newdir.begin(); iter != newdir.end(); iter++)
	directoryId.insert(pair<string, uint64_t>(iter->first, iter->second));

    //TODO get statistics result
    return true;
};

#endif
