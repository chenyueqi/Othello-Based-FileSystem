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

struct FileBlock{
    uint16_t serverNum;
    uint16_t blockNum;
};

struct DirFileEntry{
	bool dirOrFile; // directory: true, file: false
	vector<FileBlock> info;
};

struct DirFile{
	map<string, DirFileEntry> entryMap;
	vector<FileBlock> info;
};

class Server{
    private:
	uint16_t num;
	uint64_t serverCapacity;
	uint64_t availCapacity;
	uint64_t usedCapacity;
	vector<Server>* serverArr;
	map<string, DirFile> dirFileMap;

    private:

	//communication between server
	bool sendMessageToServer(const string inst, stack<string> pathStack, const bool dirExist, const uint16_t preServerNum, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);

	//directory operation
	bool mkDir(const string dirName, const bool dirExist, const uint16_t preServerNum, uint16_t &serverResult);
	bool lsDir(const string dirName); // does not contain cross server access
	bool delDir(const string dirName);

	bool storeDirFile(const string dirName);

	//file operation;
	bool writeFile(const string fileName);
	bool readFile(const string fileName);
	bool deleteFile(const string fileName);

	//check server to find appropriate server for storage
	bool alloDirFileServer(uint16_t &serverResult);
	bool alloFileServer(uint16_t &serverResult);

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
	Server(uint16_t n, uint64_t capacity=defaultCapacity, vector<Server>* serverArr=NULL): //constructor
	    num(n), 
	    serverCapacity(capacity), 
	    availCapacity(serverCapacity), 
	    usedCapacity(0){}

	uint16_t getNum(){return num;}
	uint64_t getServerCapacity(){return serverCapacity;}
	uint64_t getAvailableCapacity(){return availCapacity;}
	uint64_t getUsedCapacity(){return usedCapacity;}

	bool setNum(uint16_t n){num = n;}
	bool setServerCapacity(uint64_t capacity){availCapacity = capacity;}

	void testDirFile(); // test Directory File content

	//FIXME
	bool  getMessage(const string inst, stack<string> pathStack, const bool dirExist, const uint16_t preServerNum, uint16_t &serverResult,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t size); // message interface
};

bool Server::alloDirFileServer(uint16_t &serverResult)
{
    uint16_t high = num >> dcBit;
    uint16_t low = num - (high << dcBit);

    fprintf(stderr, "high: %u, low: %u %s %d\n", high, low, __FILE__, __LINE__);

    if(availCapacity > 0){
	serverResult = num;
	return true;
    }
    else // current server do not have enough storage
    {
	uint16_t high = num >> dcBit;
	uint16_t low = num - (high << dcBit);

	uint16_t serverPerDc = serverArr->size() / (1<<dcBit);

	//FIXME if all the servers in a datacenter do not have storage ?
	while(!(serverArr->at(high<<dcBit + low).getAvailableCapacity() > 0))
	    high = (high + 1)%serverPerDc;

	serverResult = high << dcBit + low;
    }
}

bool Server::alloFileServer(uint16_t &serverResult)
{
    uint16_t high = num >> dcBit;
    uint16_t low = num - (high << dcBit);

    uint16_t serverPerDc = serverArr->size() / (1<<dcBit);

    uint16_t candidate = rand()%serverPerDc;

    //FIXME if all the servers in a datacenter do not have storage ?
    while(!(serverArr->at(candidate).getAvailableCapacity() > 0))
	candidate = rand()%serverPerDc;

    serverResult = candidate;


    return true; 
}

void Server::testDirFile()
{
    for(map<string, DirFile>::iterator iter = dirFileMap.begin(); iter != dirFileMap.end(); iter++)
    {
	fprintf(stderr, "%s\n", iter->first.c_str());
	for(map<string, DirFileEntry>::iterator iter2  = iter->second.entryMap.begin(); iter2 != iter->second.entryMap.end(); iter2++)
	    fprintf(stderr, "  %s\n", iter2->first.c_str());
    }
}

bool Server::storeDirFile(string dirName)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);

    // if this directory file does not exist
    if(iter == dirFileMap.end()){
	DirFile newDirFile;
	dirFileMap.insert(pair<string, DirFile>(dirName, newDirFile));

	useStorage(fileBlockSize);
	return true;
    }

    // if this directory file has exist in this server
    else{
	fprintf(stderr, "directory file exist ! %s %d\n", __FILE__, __LINE__);
	return true;
    }
    return false;
}

//if dirExist = true, do not allocate server for new directory to store relative directory file
//else if dirExist = false, allocate server for new directory to store and also assign serverResult
bool Server::mkDir(string dirName, const bool dirExist, const uint16_t preServerNum , uint16_t &serverResult)
{
    int i = 0;
    for(i = dirName.size(); i > -1 && dirName[i] != '/'; i--);
    string faName = dirName.substr(0, i);

    map<string, DirFile>::iterator iter = dirFileMap.find(faName);

    if(iter == dirFileMap.end())
    {
	fprintf(stderr, "BUG %s %d\n", __FILE__ , __LINE__);
	return false;
    }

    if(iter->second.entryMap.find(dirName) == iter->second.entryMap.end())
    {
	DirFileEntry entry;
	entry.dirOrFile = true;

	FileBlock block;

	//FIXME allocate server
	//if dirExist = true, how to handle
	
	alloDirFileServer(block.serverNum);
	block.blockNum = 0;
	entry.info.push_back(block);

	//TODO use storage
	iter->second.entryMap.insert(pair<string, DirFileEntry>(dirName, entry));

	storeDirFile(dirName);

	return true;
    }
    else
    {
	fprintf(stderr, "directory file entry exist %s %d\n", __FILE__, __LINE__);
	return true;
    }
    return false;
}


bool Server::lsDir(string dirName)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);

    if(iter == dirFileMap.end())
	return false;
    else
    {
	map<string, DirFileEntry> entryMap = iter->second.entryMap;
	fprintf(stderr, "BEGIN\n");
	for(map<string, DirFileEntry>::iterator iter = entryMap.begin(); iter != entryMap.end(); iter++)
	{
	    fprintf(stderr, "%s\t", iter->first.c_str());
	    if(iter->second.dirOrFile == true)
		fprintf(stderr, "directory\t");
	    else
		fprintf(stderr, "file\t");
	}
	fprintf(stderr, "\nEND\n");
    }
    return true;
}

bool Server::delDir(const string dirName)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);

    if(iter == dirFileMap.end())
	return true;
    else
    {
	for(map<string, DirFileEntry>::iterator iter2 = iter->second.entryMap.begin(); iter2 != iter->second.entryMap.end(); iter2++)
	{
	    // if this entry is directory, do recursion
	    if(iter2->second.dirOrFile == true)
	    {
		//TODO

	    }
	    else
	    {
		//TODO
	    }
	}
    }
    return false;
}

//TODO
bool Server::getMessage(const string inst, stack<string> pathStack, const bool dirExist, const uint16_t preServerNum, uint16_t &serverResult,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t size)
{
    serverAcceCnt++;

    if(!inst.compare("list directory")){
	while(!pathStack.empty()){
	    lsDir(pathStack.top());
	    pathStack.pop();
	}
    }

    else if(!inst.compare("store directory file")){
	while(!pathStack.empty()){
	    storeDirFile(pathStack.top());
	    pathStack.pop();
	}
    }

    else if(!inst.compare("make directory")){
	while(!pathStack.empty()){
	    mkDir(pathStack.top() , dirExist, preServerNum, serverResult);
	    pathStack.pop();
	}
    }

    else if(!inst.compare("delete directory")){
	while(!pathStack.empty()){
	    delDir(pathStack.top());
	    pathStack.pop();
	}
    }

    else if(!inst.compare("writefile")){

    }
    else if(!inst.compare("readfile")){
    }

    else if(!inst.compare("use storage")){
	useStorage(size);
    }

    else if(!inst.compare("free storage")){
	freeStorage(size);
    }

    else
	fprintf(stderr, "INVALID MESSAGE !\n");
}

#endif
