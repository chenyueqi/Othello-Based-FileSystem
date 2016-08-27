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
    uint16_t blockCnt;
};

struct DirFileEntry{
	bool dirOrFile; // directory: true, file: false
	uint16_t serverNum;// used if dirOrFile = true
	vector<FileBlock> info; // used if dirOrFile = fasle
};

struct DirBlock{
    map<string, DirFileEntry> entryMap;
    uint16_t serverNum;
    uint16_t blockCnt;
};

struct DirFile{
    vector<DirBlock> info;
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
	bool sendMessageToServer(const uint16_t serverNum, const string inst, stack<string> pathStack, const bool dirExist, const uint16_t preServerNum, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);

	//directory operation
	bool mkDir(const string dirName, const bool dirExist, const uint16_t preServerNum, uint16_t &serverResult, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool lsDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt); // does not contain cross server access
	bool delDir(const string dirName, uint16_t &serverAcceCn, uint8_t &dcAcceCnt);

	bool storeDirFile(const string dirName);

	//file operation;
	bool writeFile(const string fileName);
	bool readFile(const string fileName);
	bool deleteFile(const string fileName);

	//check server to find appropriate server for storage
	bool allocDirFileServer(const uint16_t reference, uint16_t &serverResult);
	bool allocFileServer(uint16_t &serverResult);

	//capacity management in Byte
	bool useStorage(uint64_t capacity){
	    availCapacity -= capacity;
	    usedCapacity += capacity;
	}

	bool freeStorage(uint64_t capacity){
	    availCapacity += capacity;
	    usedCapacity -= capacity;
	}

	bool isSameDc(uint16_t s1, uint16_t s2){
	    return (s1 - ((s1 >> dcBit) << dcBit)) == (s2 - ((s2 >> dcBit) << dcBit));
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

bool Server::allocDirFileServer(const uint16_t reference, uint16_t &serverResult)
{
    uint16_t high = reference >> dcBit;
    uint16_t low = reference - (high << dcBit);

    fprintf(stderr, "high: %u, low: %u %s %d\n", high, low, __FILE__, __LINE__);

    //if reference server has enough capacity
    if(serverArr->at(reference).getAvailableCapacity() > fileBlockSize){
	serverResult = reference;
	return true;
    }
    else{

	uint16_t high = reference >> dcBit;
	uint16_t low = reference - (high << dcBit);

	uint16_t serverPerDc = serverArr->size() / (1<<dcBit);

	//FIXME if all the servers in a datacenter do not have storage ?
	while(!(serverArr->at(high<<dcBit + low).getAvailableCapacity() > fileBlockSize))
	    high = (high + 1)%serverPerDc;

	serverResult = high << dcBit + low;
    }
}

//TODO alloc according to reference
bool Server::allocFileServer(uint16_t &serverResult)
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

	for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++)
	    for(map<string, DirFileEntry>::iterator iter3 = iter2->entryMap.begin(); iter3 !=iter2->entryMap.end(); iter3++)
		fprintf(stderr, "  %s\n", iter3->first.c_str());
    }
	
}


bool Server::storeDirFile(string dirName)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);

    // if this directory file does not exist
    if(iter == dirFileMap.end()){

	DirBlock newDirBlock;
	newDirBlock.serverNum = num;
	newDirBlock.blockCnt = 1;

	useStorage(newDirBlock.blockCnt * fileBlockSize);

	DirFile newDirFile;
	newDirFile.info.push_back(newDirBlock);
	dirFileMap.insert(pair<string, DirFile>(dirName, newDirFile));

	return true;
    }
    else{
	fprintf(stderr, "directory file exist ! %s %d\n", __FILE__, __LINE__);
	return true;
    }
    return false;
}

//if dirExist = true, do not allocate server for new directory to store relative directory file
//else if dirExist = false, allocate server for new directory to store and also assign serverResult
bool Server::mkDir(string dirName, const bool dirExist, const uint16_t preServerNum , uint16_t &serverResult, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    int i = 0;
    for(i = dirName.size(); i > -1 && dirName[i] != '/'; i--);
    string faName = dirName.substr(0, i);

    map<string, DirFile>::iterator iter = dirFileMap.find(faName);

    // if father directory does not exist, this is fasle
    if(iter == dirFileMap.end()){
	fprintf(stderr, "BUG %s %d\n", __FILE__ , __LINE__);
	return false;
    }

    bool flag = false;
    for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++){
	if(!isSameDc(iter2->serverNum, num))
	    dcAcceCnt++;
	else if(iter2->serverNum != num)
	    serverAcceCnt++;

	if(iter2->entryMap.find(dirName) == iter2->entryMap.end())
	    continue;
	else{
	    flag = true;
	    break;
	}
    }

    // if direcotory entry does not exist, do adding
    if(!flag){
	vector<DirBlock>::iterator iter3 = iter->second.info.end() - 1;

	// final directory file block has been filled
	if(iter3->entryMap.size() == iter3->blockCnt * fileBlockSize / dirFileEntrySize)
	{
	    uint16_t num;
	    allocDirFileServer(iter3->serverNum, num);
	    if(num == iter3->serverNum)
		iter3->blockCnt++;
	    else
	    {
		DirBlock newDirBlock;
		newDirBlock.serverNum = num;
		newDirBlock.blockCnt = 1;
		iter->second.info.push_back(newDirBlock);
		iter3 = iter->second.info.end() - 1;
	    }
	}

	DirFileEntry entry;
	entry.dirOrFile = true;
	    
	if(dirExist)
	    entry.serverNum = preServerNum;
	else{
	    allocDirFileServer(iter3->serverNum, entry.serverNum);
	    //TODO send message to server
	}

	iter3->entryMap.insert(pair<string, DirFileEntry>(dirName, entry));
	return true;
    }
    else
    {
	fprintf(stderr, "directory file entry exist %s %d\n", __FILE__, __LINE__);
	return true;
    }
    return false;
}

bool Server::lsDir(string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);

    if(iter == dirFileMap.end())
	return false;
    else
    {
	fprintf(stderr, "BEGIN\n");
	for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++)
	{
	    if(!isSameDc(iter2->serverNum, num))
		dcAcceCnt++;
	    else if(iter2->serverNum != num)
		serverAcceCnt++;

	    for(map<string, DirFileEntry>::iterator iter3 = iter2->entryMap.begin(); iter3 != iter2->entryMap.end(); iter3++)
	    {
		fprintf(stderr, "%s\t", iter3->first.c_str());
		if(iter3->second.dirOrFile == true)
		    fprintf(stderr, "directory\t");
		else
		    fprintf(stderr, "file\t");
	    }
	}
	fprintf(stderr, "\nEND\n");
    }
    return true;
}

bool Server::delDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);

    if(iter == dirFileMap.end())
	return true;
    else
    {
    }
    return false;
}

//TODO
bool Server::getMessage(const string inst, stack<string> pathStack, const bool dirExist, const uint16_t preServerNum, uint16_t &serverResult,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t size)
{
    serverAcceCnt++;

    //from gateway
    if(!inst.compare("list directory")){
	while(!pathStack.empty()){
	    lsDir(pathStack.top(), serverAcceCnt, dcAcceCnt);
	    pathStack.pop();
	}
    }

    //from another server
    else if(!inst.compare("store directory file")){
	while(!pathStack.empty()){
	    storeDirFile(pathStack.top());
	    pathStack.pop();
	}
    }

    //from centralized concise
    else if(!inst.compare("make directory")){
	while(!pathStack.empty()){
	    mkDir(pathStack.top() , dirExist, preServerNum, serverResult, serverAcceCnt, dcAcceCnt);
	    pathStack.pop();
	}
    }

    //from gateway
    else if(!inst.compare("delete directory")){
	while(!pathStack.empty()){
	    delDir(pathStack.top(), serverAcceCnt, dcAcceCnt);
	    pathStack.pop();
	}
    }

    //from gateway
    else if(!inst.compare("writefile")){

    }

    //from gateway 
    else if(!inst.compare("readfile")){
    }

    //from another server
    else if(!inst.compare("use storage")){
	useStorage(size);
    }

    //from another server
    else if(!inst.compare("free storage")){
	freeStorage(size);
    }

    else
	fprintf(stderr, "INVALID MESSAGE !\n");
}

#endif
