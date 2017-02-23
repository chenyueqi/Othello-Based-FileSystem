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

struct File{
    uint64_t size;
    uint64_t id;
}

struct DirEntry{
    bool dirOrFile; // directory: true, file: false
    uint16_t serverNum;
}

struct Dir{
    map<string, DirEntry> entryMap;
}

class Server{
    private:
	uint16_t num;
	uint64_t serverCapacity; // in Byte
	uint64_t availCapacity;
	uint64_t usedCapacity;
	vector<Server>* serverArr; //server cluster
	map<string, File> fileMap;
	map<string, Dir> dirMap;

    private:
	//file operation;
	bool touchFile(const string path, const uint64_t id);
	bool writeFile(const string path, const uint64_t size, const uint64_t id);
	bool readFile(const string path, uint64_t &size, uint64_t &id);
	bool rmFile(const string path);
	bool mvFile(const string path1, const string path2, uint64_t &size, uint64_t &id);
	bool cpFile(const string path, uint64_t &id);

	//directory operation
	bool mkDir(const string path);
	bool lsDir(const string path, uint64_t &size);
	bool rmrDir(const string path);
	bool mvrDir(const string path1, const string path2);

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
	Server(uint16_t n, uint64_t capacity=defaultCapacity, vector<Server>* server=NULL): //constructor
	    num(n), 
	    serverCapacity(capacity), 
	    availCapacity(serverCapacity), 
	    usedCapacity(0),
	    serverArr(server){}

	uint16_t getNum(){return num;}
	uint64_t getServerCapacity(){return serverCapacity;}
	uint64_t getAvailableCapacity(){return availCapacity;}
	uint64_t getUsedCapacity(){return usedCapacity;}

	bool setNum(uint16_t n){num = n;}
	bool setServerCapacity(uint64_t capacity){availCapacity = capacity;}

	bool getMessage(const sting op, string path1, string path2, uint64_t &size, uint64_t &id)
};

bool Server::writeFile(const string path, const uint64_t size, const uint64_t id)
{
    //TODO visit othello first
    map<string, File>::iterator iter = fileMap.find(path);
    if(iter == fileMap.end()){
	File newfile;
	newfile.size = size;
	newfile.id = id;
	fileMap.insert(pari<string, File>(string, newfile));
    }
    else
	iter->second.size += size;
    useStorage(size);
    return true;
}

bool Server::touchFile(const string path, const uint64_t id)
{
    //TODO visit othello first
    map<string, File>::iterator iter = fileMap.find(path);
    if(iter == fileMap.end()){
	File newfile;
	newfile.size = 0;
	newfile.id = id;
	fileMap.insert(pari<string, File>(string, newfile));
	return true;
    }
    else{
	fprintf(stderr, "FILE %s already exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
}

bool Server::readFile(const string path, uint64_t &size, uint64_t &id)
{
    //TODO visit othello first
    map<string, File>::iterator iter = fileMap.find(path);
    if(iter == fileMap.end()){
	fprintf(stderr, "FILE %s doesn't exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
    else{
	size = iter->second.size;
	id = iter->second.id;
	return true;
    }
}

bool Server::cpFile(const string path, uint64_t &size)
{
    //TODO visit othello first
    map<string, File>::iterator iter = fileMap.find(path);
    if(iter == fileMap.end()){
	fprintf(stderr, "FILE %s doesn't exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
    else{
	size = iter->second.size;
	return true;
    }
}

bool Server::mvFile(const string path1, const string path2, uint64_t &size, uint64_t &id)
{
    //TODO visit othello first
    map<string, File>::iterator iter = fileMap.find(path1);
    if(iter == fileMap.end()){
	fprintf(stderr, "FILE %s doesn't exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
    else{
	size = iter->second.size;
	id = iter->second.id;
	fileMap.erase(iter);
	File newFile;
	newFile.size = size;
	newFile.id = id;
	fileMap.insert(pair<string, File>(path2, newFile));
	return true;
    }
}

bool Server::rmFile(const string path)
{
    //TODO visit othello first
    map<string, File>::iterator iter = fileMap.find(path);
    if(iter == fileMap.end()){
	fprintf(stderr, "FILE %s doesn't exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
    else{
	freeStorage(iter->second.size);
	fileMap.erase(iter);
	return true;
    }
}

bool Server::mkDir(const string path)
{
    map<string, File>::iterator iter = dirMap.find(path);
    if(iter == dirMap.end()){
	Dir newDir;
	dirMap.insert(pair<string, Dir>(path, newDir));
	return true;
    }
    else{
	fprintf(stderr, "DIRECTORY %s already exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
}

bool Server::lsDir(const string path, uint64_t &size)
{
    map<string, File>::iterator iter = dirMap.find(path);
    if(iter == dirMap.end()){
	fprintf(stderr, "DIRECTORY %s doesn't exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
    else{
	size = iter->second.entryMap.size() * dirEntrySize;
	return false;
    }
}

bool Server::rmrDir(const string path)
{
    map<string, File>::iterator iter0 = dirMap.find(path);
    if(iter0 == dirMap.end()){
	fprintf(stderr, "DIRECTORY %s doesn't exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
    else{
	for(map<string, DirEntry>::iterator iter1 = iter0->second.entryMap.begin(); iter1 != iter0->second.entryMap.end(); iter1++){
	    uint64_t size = 0;
	    uint64_t id = 0;
	    if(iter1->second.dirOrFile == true)
		serverArr->at(iter1->second.serverNum).getMessage("rmr", iter1->first, size, id, 0);
	    else
		serverArr->at(iter1->second.serverNum).getMessage("rm", iter1->first, size, id, 0);
	}
    }
    //TODO
}

bool Server::mvrDir(const string path1, const string path2)
{
}

bool Server::getMessage(const sting op, string path1, string path2, uint64_t &size, uint64_t &id, const uint16_t serverNum)
{
    if(!op.compare("write"))
	return writeFile(path1, size, id);

    else if(!op.compare("touch"))
	return touchFile(path1, id, serverNum);

    else if(!op.compare("read"))
	return readFile(path1);

    else if(!op.compare("rm"))
	return rmFile(path1);

    else if(!op.compare("mv"))
	return mvFile(path1);

    else if(!op.compare("cp"))
	return cpFile(path1);

    else if(!op.compare("mkdir"))
	return mkDir(path1);

    else if(!op.compare("ls"))
	return lsDir(path1);

    else if(!op.compare("rmr"))
	return rmrDir(path1);

    else if(!op.compare("mvr"))
	return mvrDir(path1, path2);

    else{
	fprintf(stderr, "INVALID Operation %s %d\n", __FILE__, __LINE__);
	return false;
    }

    return true
}

#endif
