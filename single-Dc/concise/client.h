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
	map<string, uint64_t> dirFileId;
	bool getId(string path, uint64_t &id);
	bool getFaId(string path, uint64_t &id);

    public:
	Client(Gateway* p = NULL):gateWay(p){}

	bool setGateWay(Gateway* p){
	    gateWay = p; 
	    dirFileId.insert(pair<string, uint64_t>("/", 0));
	}

	bool testDirID(){
	    for(map<string, uint64_t>::iterator iter = dirFileId.begin(); iter != dirFileId.end(); iter++)
		fprintf(stdout, "%s %u\n", iter->first.c_str(), iter->second);
	}

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

    map<string, uint64_t> newdir;
    vector<string> olddir;

    if(!op.compare("touch")){
	string path;
	getline(me, path, ' ');
	uint64_t id;
	getFaId(path, id);
	gateWay->getMessage(op, path, "", id, 0, newdir, olddir);
    }

    else if(!op.compare("write")){
	string path;
	getline(me, path, ' ');
	uint64_t id1;
	getFaId(path, id1);
	uint64_t id2;
	getId(path, id2);
	uint64_t size = 0;
	me>>size;
	gateWay->getMessage(op, path, "", id1, id2, size, newdir, olddir); //size as id2
    }

    else if(!op.compare("read")){
	string path;
	getline(me, path, ' ');
	uint64_t id1;
	getFaId(path, id1);
	uint64_t id2;
	getId(path, id2);
	gateWay->getMessage(op, path, "", id1, id2, newdir, olddir);
    }

    else if(!op.compare("mv")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2, id3;
	getFaId(path1, id1);
	getId(path1, id2);
	getId(path2, id3);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir, olddir);
    }

    if(!op.compare("mkdir") || !op.compare("rm") || !op.compare("read") || !op.compare("touch")){
	string path;
	getline(me, path, ' ');
	uint64_t id;
	getFaId(path, id);
	gateWay->getMessage(op, path, "", id, 0, newdir, olddir);
    }


    else if(!op.compare("rmr")){
	string path;
	getline(me, path, ' ');
	uint64_t id1, id2;
	getDirId(path, id1);
	getFaId(path, id2);
	gateWay->getMessage(op, path, "", id1, id2, newdir, olddir);
    }

    else if(!op.compare("ls")){
	string path;
	getline(me, path, ' ');
	uint64_t id;
	getDirId(path, id);
	gateWay->getMessage(op, path, "", id, 0, newdir, olddir);
    }


    else if(!op.compare("mvr")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2;
	getDirId(path1, id1);
	getDirId(path2, id2);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir, olddir);
    }

    else if(!op.compare("cp")){
	string path1, path2;
	getline(me, path1, ' ');
	getline(me, path2, ' ');
	uint64_t id1, id2;
	getFaId(path1, id1);
	getFaId(path2, id2);
	gateWay->getMessage(op, path1, path2, id1, id2, newdir, olddir);
    }

    else{
	fprintf(stderr, "INVALID operation %s %d\n", __FILE__, __LINE__);
	return false;
    }

    for(vector<string>::iterator iter0 = olddir.begin(); iter0 != olddir.end(); iter0++){
	map<string, uint64_t>::iterator iter1 = dirFileId.find(*iter0);
	dirFileId.erase(iter1);
    }

    for(map<string, uint64_t>::iterator iter = newdir.begin(); iter != newdir.end(); iter++)
	dirFileId.insert(pair<string, uint64_t>(iter->first, iter->second));

    return true;
};


bool Client::getId(string path, uint64_t &id)
{
    map<string, uint64_t>::iterator iter = dirFileId.find(path);

    if(iter == dirFileId.end()){
	fprintf(stderr, "directory %s does not exist %s %d\n", path.c_str(), __FILE__, __LINE__);
	return false;
    }
    id = iter->second;
    return true;
}

bool Client::getFaId(string path, uint64_t &id)
{
    int i = 0;
    for(i = path.size(); i > 1 && path[i] != '/'; i--);
    string faName = path.substr(0,i);
    map<string, uint64_t>::iterator iter = dirFileId.find(faName);
    
    if(iter == dirFileId.end()){
	fprintf(stderr, "directory %s does not exist %s %d\n", faName.c_str(), __FILE__, __LINE__);
	return false;
    }
    id = iter->second;
    return true;
}

#endif
