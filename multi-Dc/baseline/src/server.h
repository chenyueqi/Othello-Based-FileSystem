#ifndef __SERVER_H__
#define __SERVER_H__

#include<iostream>
#include<string>
#include<cstdlib>
#include<map>
#include<vector>
#include<sstream>
#include<stack>
#include"common.h"

using namespace std;

struct objInfo
{
    uint64_t size;
    bool dirOrFile;
};


class Server{
    private:
	bool alive;
	uint16_t num;
	uint64_t serverCapacity; // in Byte
	uint64_t availCapacity;
	uint64_t usedCapacity;
	map<string, objInfo> objMap; //dirtory server store in this server

    private:
	//directory operation
	bool lsDir(const string path, map<string, objInfo> &result);
	bool mkDir(const string path);
	bool delDir(const string path);
	bool mvDir(const string path, map<string, objInfo> &result);

	//file operation;
	bool touchFile(const string path);
	bool writeFile(const string path, const uint64_t size);
	bool readFile(const string path, map<string, objInfo> &result);
	bool delFile(const string path);
	bool mvFile(const string path, map<string, objInfo> &result);
	bool cpFile(const string path, map<string, objInfo> &resutl);

	//capacity management in Byte
	bool useStorage(uint64_t capacity){
	    availCapacity -= capacity;
	    usedCapacity += capacity;
	}

	bool freeStorage(uint64_t capacity){
	    availCapacity += capacity;
	    usedCapacity -= capacity;
	}

    public:
	Server(uint16_t n, bool state = true,  uint64_t capacity=defaultCapacity): //constructor
	    alive(state),
	    num(n), 
	    serverCapacity(capacity), 
	    availCapacity(serverCapacity), 
	    usedCapacity(0){}

	bool getState(){return alive;}
	uint16_t getNum(){return num;}
	uint64_t getServerCapacity(){return serverCapacity;}
	uint64_t getAvailableCapacity(){return availCapacity;}
	uint64_t getUsedCapacity(){return usedCapacity;}
	uint64_t getFileNum(){return objMap.size();}

	bool setNum(uint16_t n){num = n;}
	bool setServerCapacity(uint64_t capacity){availCapacity = capacity;}
	bool setState(bool state){alive = state; return true;}

	void testObj();

	bool getMessage(const string op, const string path, const uint64_t size, map<string, objInfo> &result);
};

void Server::testObj()
{
    for(map<string, objInfo>::iterator iter = objMap.begin(); iter != objMap.end(); iter++)
    {
	fprintf(stdout, "%s ", iter->first.c_str());
	if(iter->second.dirOrFile == true)
	    fprintf(stdout, "  directory\n");
	else 
	    fprintf(stdout, "  file %lu\n", iter->second.size);
    }
}

bool Server::lsDir(const string path, map<string, objInfo> &result)
{
    for(map<string, objInfo>::iterator iter = objMap.begin(); iter != objMap.end(); iter++){
	int i = 0;
	for(i = 0; iter->first[i] != '/' ;i++);
	string temp1 = iter->first.substr(i, iter->first.length());
	for(i = temp1.size(); i > 1 && temp1[i] != '/'; i--);
	string temp2 = temp1.substr(0, i);
	if(!path.compare(temp2) && path.compare(temp1)){
	    result.insert(pair<string, objInfo>(temp1, iter->second));
	}
    }
    return true;
}

bool Server::mkDir(const string path)
{
    if(objMap.find(path) != objMap.end()){
	fprintf(stderr, "directory already exist\n");
	return false;
    }

    if(availCapacity < objEntrySize){
	fprintf(stderr, "server capacity isn't enough %s %d\n", __FILE__, __LINE__);
	return false;
    }

    objInfo newinfo;
    newinfo.dirOrFile = true;
    newinfo.size = 0;
    objMap.insert(pair<string, objInfo>(path, newinfo));
    useStorage(objEntrySize);
    return true;
}

bool Server::delDir(const string path)
{
    for(map<string, objInfo>::iterator iter = objMap.begin(); iter != objMap. end(); iter++){
	if(iter->first.find(path) != string::npos){
	    freeStorage(iter->second.size);
	    freeStorage(objEntrySize);
	    objMap.erase(iter);
	}
    }
    return true;
}

bool Server::mvDir(const string path, map<string, objInfo> &result)
{
    for(map<string, objInfo>::iterator iter = objMap.begin(); iter != objMap. end(); iter++){
	if(iter->first.find(path) != string::npos){
	    result.insert(pair<string, objInfo>(iter->first, iter->second));
	    freeStorage(iter->second.size);
	    freeStorage(objEntrySize);
	    objMap.erase(iter);
	}
    }
    return true;
}

bool Server::touchFile(const string path)
{
    if(availCapacity < objEntrySize){
	fprintf(stderr, "server capacity isn't enough %s %d\n", __FILE__, __LINE__);
	return false;
    }

    objInfo newinfo;
    newinfo.dirOrFile = false;
    newinfo.size = 0;
    objMap.insert(pair<string, objInfo>(path, newinfo));
    useStorage(objEntrySize);
    return true;
}

bool Server::writeFile(const string path, const uint64_t size)
{
    map<string, objInfo>::iterator iter = objMap.find(path);
    if(iter == objMap.end()){
	if((size + objEntrySize) > availCapacity){
	    fprintf(stderr, "server capacity isn't enough %s %d\n", __FILE__, __LINE__);
	    return false;
	}

	objInfo newinfo;
	newinfo.dirOrFile = false;
	newinfo.size = size;
	objMap.insert(pair<string, objInfo>(path, newinfo));
	useStorage(objEntrySize);
	useStorage(size);
	return true;
    }
    else{
	//fprintf(stderr, "file %s has been exist\n", path.c_str());
	if(size > availCapacity){
	    fprintf(stderr, "server capacity isn't enough %s %d\n", __FILE__, __LINE__);
	    return false;
	}
	iter->second.size += size;
	useStorage(size);
	return true;
    }
    return false;
}

bool Server::readFile(const string path, map<string, objInfo> &result)
{
    map<string, objInfo>::iterator iter = objMap.find(path);
    if(iter == objMap.end()){
	fprintf(stderr, "file %s doesn't exist %s %d\n", path.c_str(), __FILE__, __LINE__);
	return false;
    }
    else{
	result.insert(pair<string, objInfo>(path, iter->second));
	return true;
    }
    return false;
}

bool Server::delFile(const string path)
{
    map<string, objInfo>::iterator iter = objMap.find(path);
    if(iter == objMap.end()){
	fprintf(stderr, "file %s doesn't exist %s %d\n", path.c_str(), __FILE__, __LINE__);
	return false;
    }
    else{
	freeStorage(iter->second.size);
	freeStorage(objEntrySize);
	objMap.erase(iter);
	return true;
    }
    return false;
}

bool Server::mvFile(const string path, map<string, objInfo> &result)
{
    map<string, objInfo>::iterator iter = objMap.find(path);
    if(iter == objMap.end()){
	fprintf(stderr, "file %s doesn't exist %s %d\n", path.c_str(), __FILE__, __LINE__);
	struct objInfo tmpobj;
	tmpobj.size = 0;
	result.insert(pair<string, objInfo>(path, tmpobj));
	return false;
    }
    else{
	result.insert(pair<string, objInfo>(path, iter->second));
	freeStorage(iter->second.size);
	freeStorage(objEntrySize);
	objMap.erase(iter);
	return true;
    }
    return false;
}

bool Server::cpFile(const string path, map<string, objInfo> &result)
{
    map<string, objInfo>::iterator iter = objMap.find(path);
    if(iter == objMap.end()){
	fprintf(stderr, "file %s doesn't exist %s %d\n", path.c_str(), __FILE__, __LINE__);
	struct objInfo tmpobj;
	tmpobj.size = 0;
	result.insert(pair<string, objInfo>(path, tmpobj));
	return false;
    }
    else{
	result.insert(pair<string, objInfo>(path, iter->second));
	return true;
    }
    return false;
}

/*
 * op: specific operation
 * path: relevant path name
 * size: indicate write operation's volume 
 * result: relevant operation path-size pair
 */

bool Server::getMessage(const string op, const string path, const uint64_t size, map<string, objInfo> &result)
{
    if(!op.compare("list directory"))
	return lsDir(path, result);

    else if(!op.compare("delete directory"))
	return delDir(path);
    
    else if(!op.compare("make directory"))
	return mkDir(path);

    else if(!op.compare("move directory"))
	return mvDir(path, result);

    else if(!op.compare("touch file"))
	return touchFile(path);

    else if(!op.compare("write file"))
	return writeFile(path, size);

    else if(!op.compare("read file"))
	return readFile(path, result);
    
    else if(!op.compare("delete file"))
	return delFile(path);

    else if(!op.compare("move file"))
	return mvFile(path, result);

    else if(!op.compare("copy file"))
	return cpFile(path, result);

    else
	fprintf(stderr, "INVALID OPeration %s %d\n", __FILE__, __LINE__);
    return false;
}

#endif
