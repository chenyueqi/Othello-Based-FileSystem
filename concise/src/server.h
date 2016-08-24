#ifndef __SERVER_H__
#define __SERVER_H__

#include<iostream>
#include<string>
#include<ctime>
#include<map>
#include<vector>
#include<sstream>
#include<stack>
#include"common.h"

using namespace std;

struct FileBlock
{
    uint16_t serverNum;
    uint16_t blockNum;
};

struct DirFileEntry
{
	bool dirOrFile; // directory: true, file: false
	vector<FileBlock> info;
};

struct DirFile
{
	map<string, DirFileEntry> entryMap;
};

class Server
{
    private:
	uint16_t num;
	uint64_t serverCapacity;
	uint64_t availCapacity;
	uint64_t usedCapacity;
	vector<Server>* serverArr;
	map<string, DirFile> dirFileMap;

    private:
	bool sendMessageToServer(uint16_t num, string intr);

	bool mkDir(const string dirName);
	bool lsDir(const string dirName);
	bool delDir(const string dirName);

	bool storeDirFile(const string dirName);

	bool writeFile(const string fileName);
	bool readFile(const string fileName);
	bool deleteFile(const string fileName);

	bool alloDirFileServer(uint16_t &serverResult);
	bool alloFileServer(uint16_t &serverResult);

	bool usedStorage(uint64_t );
	bool freeStorage(uint64_t );
	
	
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
	bool  getMessage(const string inst, stack<string> pathStack, const uint16_t attach); // message interface
};

bool Server::alloDirFileServer(uint16_t &serverResult)
{
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
    //TODO choose a new server to store directory file  random method
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
    if(iter == dirFileMap.end())
    {
	DirFile newDirFile;
	//TODO use storage
	dirFileMap.insert(pair<string, DirFile>(dirName, newDirFile));
	return true;
    }
    else
    {
	fprintf(stderr, "directory file exist ! %s %d\n", __FILE__, __LINE__);
	return true;
    }
    return false;
}

bool Server::mkDir(string dirName)
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
	//TODO allocate server
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
	    //do something TODO
	}
    }
}

//TODO
bool Server::getMessage(const string inst, stack<string> pathStack, const uint16_t attach)
{
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
	    mkDir(pathStack.top());
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
    }

    else if(!inst.compare("free storage")){
    }

    else
	fprintf(stderr, "INVALID MESSAGE !\n");
}

#endif
