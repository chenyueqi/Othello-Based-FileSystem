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
    uint64_t restCapacity;
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
	uint64_t serverCapacity; // in Byte
	uint64_t availCapacity;
	uint64_t usedCapacity;
	vector<Server>* serverArr; //server cluster
	map<string, DirFile> dirFileMap; //dirtory server store in this server

    private:
	//directory operation
	bool existDir(const string dirName);
	bool lsDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool storeDirFile(const string dirName);
	bool mkDir(const string dirName, const bool dirExist, const uint16_t preServerNum, map<string, uint16_t> &resultMap, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool delDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool mvDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool rnDir(const string origName, const string newName, map<string, uint16_t> &resultMap,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);

	//file operation;
	bool touchFile(const string fileName, const bool fileExist, vector<FileBlock> info, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool writeFile(const string fileName, uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool readFile(const string fileName, uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool delFile(const string fileName,uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);

	//check server to find appropriate server for storage
	bool allocDirFileServer(const uint16_t reference, uint16_t &serverResult);
	bool allocFileServer(const uint16_t reference, uint16_t &serverResult);

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

	void testDirFile(); // test Directory File content

	bool getMessage(const string op, stack<string> pathStack, const string origName, const string newName, map<string, uint16_t> &resultMap, const bool exist, const uint16_t preServerNum, const vector<FileBlock> info, const uint64_t size,   uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
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

bool Server::allocFileServer(const uint16_t reference, uint16_t &serverResult)
{
    uint16_t high = reference >> dcBit;
    uint16_t low = reference - (high << dcBit);

    uint16_t serverPerDc = serverArr->size() / (1<<dcBit);

    uint16_t candidate = (rand()%serverPerDc)<<dcBit+low;

    //FIXME if all the servers in a datacenter do not have storage ?
    while(!(serverArr->at(candidate).getAvailableCapacity() > 0))
	candidate = (rand()%serverPerDc)<<dcBit + low;

    serverResult = candidate;

    return true; 
}

void Server::testDirFile()
{
    for(map<string, DirFile>::iterator iter = dirFileMap.begin(); iter != dirFileMap.end(); iter++)
    {
	fprintf(stderr, "%s\n", iter->first.c_str());

	for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++)
	    for(map<string, DirFileEntry>::iterator iter3 = iter2->entryMap.begin(); iter3 !=iter2->entryMap.end(); iter3++){
		fprintf(stderr, "  %s", iter3->first.c_str());
		if(iter3->second.dirOrFile == true)
		    fprintf(stderr, "  directory  %u\n",  iter3->second.serverNum);
		else{ 
		    fprintf(stderr, "  file");
		    for(vector<FileBlock>::iterator iter4 = iter3->second.info.begin(); iter4 != iter3->second.info.end(); iter4++)
			fprintf(stderr, "  0x%x-%u", iter4->serverNum, iter4->restCapacity);
		    fprintf(stderr, "\n");
		}
	    }
    }
}

bool Server::existDir(const string dirName)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);
    if(iter == dirFileMap.end())
	return false;
    else
	return true;
}

bool Server::lsDir(string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    map<string, DirFile>::iterator iter0 = dirFileMap.find(dirName);
    if(iter0 == dirFileMap.end())
	return false;
    else{
	fprintf(stderr, "BEGIN\n");
	for(vector<DirBlock>::iterator iter1 = iter0->second.info.begin(); iter1 != iter0->second.info.end(); iter1++){
	    if(!isSameDc(iter1->serverNum, num))
		dcAcceCnt++;
	    else if(iter1->serverNum != num)
		serverAcceCnt++;

	    for(map<string, DirFileEntry>::iterator iter2 = iter1->entryMap.begin(); iter2 != iter1->entryMap.end(); iter2++){
		fprintf(stderr, "%s\t", iter2->first.c_str());
		if(iter2->second.dirOrFile == true)
		    fprintf(stderr, "directory\t");
		else
		    fprintf(stderr, "file\t");
	    }
	}
	fprintf(stderr, "\nEND\n");
    }
    return true;
}

bool Server::storeDirFile(string dirName)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);
    if(iter == dirFileMap.end()){

	DirBlock newDirBlock;
	newDirBlock.serverNum = num;
	newDirBlock.blockCnt = 1;

	useStorage(fileBlockSize);

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

bool Server::mkDir(const string dirName, const bool dirExist, const uint16_t preServerNum, map<string, uint16_t> &resultMap, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    int i = 0;
    for(i = dirName.size(); i > -1 && dirName[i] != '/'; i--);
    string faName = dirName.substr(0, i);
    map<string, DirFile>::iterator iter0 = dirFileMap.find(faName);
    if(iter0 == dirFileMap.end()){
	fprintf(stderr, "BUG %s %d\n", __FILE__ , __LINE__);
	return false;
    }

    // go through directory file chain to check dirName has been existed
    bool flag = false;
    for(vector<DirBlock>::iterator iter1 = iter0->second.info.begin(); iter1 != iter0->second.info.end(); iter1++){
	if(!isSameDc(iter1->serverNum, num))
	    dcAcceCnt++;
	else if(iter1->serverNum != num)
	    serverAcceCnt++;

	if(iter1->entryMap.find(dirName) == iter1->entryMap.end())
	    continue;
	else{
	    flag = true;
	    break;
	}
    }

    // if direcotory entry does not exist, do adding
    if(!flag){
	vector<DirBlock>::iterator iter2 = iter0->second.info.end() - 1;

	// final directory file block has been filled
	if(iter2->entryMap.size() == iter2->blockCnt * fileBlockSize / dirFileEntrySize){
	    uint16_t num;
	    allocDirFileServer(iter2->serverNum, num);
	    if(num == iter2->serverNum)
		iter2->blockCnt++;
	    else
	    {
		DirBlock newDirBlock;
		newDirBlock.serverNum = num;
		newDirBlock.blockCnt = 1;
		iter0->second.info.push_back(newDirBlock);
		iter2 = iter0->second.info.end() - 1;
	    }
	    serverArr->at(num).useStorage(fileBlockSize);
	}

	DirFileEntry entry;
	entry.dirOrFile = true;
	    
	if(dirExist)
	    entry.serverNum = preServerNum;
	else{
	    allocDirFileServer(iter2->serverNum, entry.serverNum);

	    resultMap.insert(pair<string, uint16_t>(dirName, entry.serverNum));

	    if(!isSameDc(iter2->serverNum, entry.serverNum))
		dcAcceCnt++;
	    else if(iter2->serverNum != entry.serverNum)
		serverAcceCnt++;
	    serverArr->at(entry.serverNum).storeDirFile(dirName);
	}

	iter2->entryMap.insert(pair<string, DirFileEntry>(dirName, entry));
	return true;
    }
    else{
	fprintf(stderr, "directory file entry exist %s %d\n", __FILE__, __LINE__);
	return true;
    }
    return false;
}

bool Server::delDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    map<string, DirFile>::iterator iter0 = dirFileMap.find(dirName);

    if(iter0 == dirFileMap.end()){
	fprintf(stderr, "directory %s does not exist%s %d\n", dirName.c_str(), __FILE__, __LINE__);
	return true;
    }
    else{
	for(vector<DirBlock>::iterator iter1 = iter0->second.info.begin(); iter1 != iter0->second.info.end(); iter1++){
	    if(!isSameDc(iter1->serverNum, num))
		dcAcceCnt++;
	    else if(iter1->serverNum != num)
		serverAcceCnt++;

	    for(map<string, DirFileEntry>::iterator iter2 = iter1->entryMap.begin(); iter2 != iter1->entryMap.begin(); iter2++){
		if(iter2->second.dirOrFile == true){
		    if(!isSameDc(iter1->serverNum, iter2->second.serverNum))
			dcAcceCnt++;
		    else if(iter1->serverNum != iter2->second.serverNum)
			serverAcceCnt++;
		    //recursion
		    serverArr->at(iter2->second.serverNum).delDir(iter2->first, serverAcceCnt, dcAcceCnt);
		}
		else{
		    for(vector<FileBlock>::iterator iter3 = iter2->second.info.begin(); iter3 != iter2->second.info.end(); iter3++){
			if(!isSameDc(iter3->serverNum, iter2->second.serverNum))
			    dcAcceCnt++;
			else if(iter3->serverNum != iter2->second.serverNum)
			    serverAcceCnt++;
			serverArr->at(iter3->serverNum).freeStorage(fileBlockSize - iter3->restCapacity);
		    }
		}
	    }
	    serverArr->at(iter1->serverNum).freeStorage(fileBlockSize * iter1->blockCnt);
	    iter1->entryMap.clear();
	}
	iter0->second.info.clear();
	return true;
    }
    return false;
}

bool Server::mvDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    int i = 0;
    for(i = dirName.size(); i > -1 && dirName[i] != '/'; i--);
    string faName = dirName.substr(0, i);

    map<string, DirFile>::iterator iter0 = dirFileMap.find(faName);

    if(iter0 == dirFileMap.end()){
	fprintf(stderr, "BUG %s %d\n", __FILE__ , __LINE__);
	return false;
    }

    bool flag = false;
    vector<DirBlock>::iterator iter1;
    map<string,DirFileEntry>::iterator iter2;
    for(iter1 = iter0->second.info.begin(); iter1 != iter0->second.info.end(); iter1++){
	if(!isSameDc(iter1->serverNum, num))
	    dcAcceCnt++;
	else if(iter1->serverNum != num)
	    serverAcceCnt++;

	iter2 = iter1->entryMap.find(dirName);
	// if file does not exist in this diretory file entry, check the next one 
	if(iter2 == iter1->entryMap.end())
	    continue;
	else{
	    flag = true;
	    break;
	}
    }

    if(flag == true){
	if(iter2->second.dirOrFile == false){
	    fprintf(stderr, "this is a file %s %d\n", __FILE__, __LINE__);
	    return false;
	}
	else
	    iter1->entryMap.erase(iter2);
    }
    else{
	fprintf(stderr, "directory doesn't exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
}

bool Server::rnDir(const string origPre, const string newPre, map<string, uint16_t> &result,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    map<string, DirFile>::iterator iter = dirFileMap.find(origPre);

    if(iter == dirFileMap.end()){
	fprintf(stderr, "BUG %s %d\n", __FILE__, __LINE__);
	return false;
    }

    for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++){
	if(!isSameDc(iter2->serverNum, num))
	    dcAcceCnt++;
	else if(iter2->serverNum != num)
	    serverAcceCnt++;

	for(map<string, DirFileEntry>::iterator iter3 = iter2->entryMap.end(); iter3 != iter2->entryMap.begin(); iter3++){

	    //if this entry has been modified
	    if(!iter3->first.substr(0, origPre.length()).compare(origPre))
		continue;

	    if(iter3->second.dirOrFile == true){
		string suffix = iter3->first.substr(origPre.length(), iter3->first.length());
		string newName = newPre + origPre;

		if(!isSameDc(iter2->serverNum, iter3->second.serverNum))
		    dcAcceCnt++;
		else if(iter2->serverNum != iter3->second.serverNum)
		    serverAcceCnt++;
		result.insert(pair<string, uint16_t>(newName, iter3->second.serverNum));
		serverArr->at(iter3->second.serverNum).rnDir(iter3->first, newName, result, serverAcceCnt, dcAcceCnt);

		DirFileEntry newEntry;
		newEntry.dirOrFile = iter3->second.dirOrFile;
		newEntry.serverNum = iter3->second.serverNum;

		iter2->entryMap.erase(iter3);
		iter2->entryMap.insert(pair<string, DirFileEntry>(newName, newEntry));
		return true;
	    }
	    else{
		string suffix = iter3->first.substr(origPre.length(), iter3->first.length());
		string newName = newPre + origPre;

		DirFileEntry newEntry;
		newEntry.dirOrFile = iter3->second.dirOrFile;
		newEntry.info = iter3->second.info;

		iter2->entryMap.erase(iter3);
		iter2->entryMap.insert(pair<string, DirFileEntry>(newName, newEntry));
		return true;
	    }
	}
    }
}

bool Server::touchFile(const string fileName, const bool fileExist, vector<FileBlock> info, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    //TODO

}


bool Server::writeFile(const string fileName, uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    int i = 0;
    for(i = fileName.size(); i > -1 && fileName[i] != '/'; i--);
    string faName = fileName.substr(0, i);

    map<string, DirFile>::iterator iter = dirFileMap.find(faName);

    if(iter == dirFileMap.end()){
	fprintf(stderr, "BUG %s %d\n", __FILE__ , __LINE__);
	return false;
    }

    // check if the file exist
    bool flag = false;
    map<string,DirFileEntry>::iterator iter3;
    for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++){
	if(!isSameDc(iter2->serverNum, num))
	    dcAcceCnt++;
	else if(iter2->serverNum != num)
	    serverAcceCnt++;

	iter3 = iter2->entryMap.find(fileName);
	// if file does not exist in this diretory file entry, check the next one 
	if(iter3 == iter2->entryMap.end())
	    continue;
	else{
	    flag = true;
	    break;
	}
    }

    // file exist
    if(flag){
	if(iter3->second.dirOrFile == true){
	    fprintf(stderr, "this is a directory %s %d\n", __FILE__, __LINE__);
	    return false;
	}

	vector<FileBlock>::iterator iter4 = iter3->second.info.end() - 1;

	while(size > 0){
	    if(iter4->restCapacity > size){
		iter4->restCapacity -= size;
		return true;
	    }
	    else{
		size -= iter4->restCapacity;
		iter4->restCapacity = 0;
		FileBlock fileBlock;
		allocFileServer(iter4->serverNum, fileBlock.serverNum);
		fileBlock.restCapacity = fileBlockSize;
		iter3->second.info.push_back(fileBlock);
		iter4 = iter3->second.info.end() - 1;
	    }
	}
	return true;
    }
    else{
	vector<DirBlock>::iterator iter2 = iter->second.info.end() - 1;
	if(iter2->entryMap.size() == iter2->blockCnt * fileBlockSize / dirFileEntrySize)
	{
	    uint16_t num;
	    allocDirFileServer(iter2->serverNum, num);
	    if(num == iter2->serverNum)
		iter2->blockCnt++;
	    else
	    {
		DirBlock newDirBlock;
		newDirBlock.serverNum = num;
		newDirBlock.blockCnt = 1;
		iter->second.info.push_back(newDirBlock);
		iter2 = iter->second.info.end() - 1;
	    }
	    serverArr->at(num).useStorage(fileBlockSize);
	}

	DirFileEntry newDirFileEntry;
	newDirFileEntry.dirOrFile = false;
	FileBlock fileBlock;
	allocFileServer(iter2->serverNum, fileBlock.serverNum);
	fileBlock.restCapacity = fileBlockSize;
	newDirFileEntry.info.push_back(fileBlock);

	vector<FileBlock>::iterator iter4 = newDirFileEntry.info.end() - 1;
	while(size > 0){
	    if(iter4->restCapacity > size){
		iter4->restCapacity -= size;
		break;
	    }
	    else{
		size -= iter4->restCapacity;
		iter4->restCapacity = 0;
		FileBlock fileBlock;
		allocFileServer(iter4->serverNum, fileBlock.serverNum);
		fileBlock.restCapacity = fileBlockSize;
		newDirFileEntry.info.push_back(fileBlock);
		iter4 = newDirFileEntry.info.end() - 1;
	    }
	}

	iter2->entryMap.insert(pair<string, DirFileEntry>(fileName, newDirFileEntry));
	return true;
    }
    return false;
}

bool Server::readFile(const string fileName, uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    int i = 0;
    for(i = fileName.size(); i > -1 && fileName[i] != '/'; i--);
    string faName = fileName.substr(0, i);

    map<string, DirFile>::iterator iter = dirFileMap.find(faName);

    if(iter == dirFileMap.end()){
	fprintf(stderr, "BUG %s %d\n", __FILE__ , __LINE__);
	return false;
    }

    // check if the file exist
    bool flag = false;
    map<string,DirFileEntry>::iterator iter3;
    for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++){
	if(!isSameDc(iter2->serverNum, num))
	    dcAcceCnt++;
	else if(iter2->serverNum != num)
	    serverAcceCnt++;

	iter3 = iter2->entryMap.find(fileName);
	// if file does not exist in this diretory file entry, check the next one 
	if(iter3 == iter2->entryMap.end())
	    continue;
	else{
	    flag = true;
	    break;
	}
    }
    if(flag == true){
	if(iter3->second.dirOrFile == true){
	    fprintf(stderr, "this is a directory %s %d\n", __FILE__, __LINE__);
	    return false;
	}

	for(vector<FileBlock>::iterator iter4 =iter3->second.info.begin(); iter4 != iter3->second.info.end(); iter4++){
	    if(!isSameDc(iter4->serverNum, num)){
		dcAcceCnt++;
		break;
	    }
	    else if(iter4->serverNum != num){
		serverAcceCnt++;
		break;
	    }
	}
	return true;
    }
    else{
	fprintf(stderr, "file does not exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
}

bool Server::delFile(const string fileName,uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    int i = 0;
    for(i = fileName.size(); i > -1 && fileName[i] != '/'; i--);
    string faName = fileName.substr(0, i);

    map<string, DirFile>::iterator iter = dirFileMap.find(faName);

    if(iter == dirFileMap.end()){
	fprintf(stderr, "BUG %s %d\n", __FILE__ , __LINE__);
	return false;
    }

    bool flag = false;
    map<string,DirFileEntry>::iterator iter3;
    for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++){
	if(!isSameDc(iter2->serverNum, num))
	    dcAcceCnt++;
	else if(iter2->serverNum != num)
	    serverAcceCnt++;

	iter3 = iter2->entryMap.find(fileName);
	// if file does not exist in this diretory file entry, check the next one 
	if(iter3 == iter2->entryMap.end())
	    continue;
	else{
	    flag = true;
	    break;
	}
    }

    if(flag == true){
	if(iter3->second.dirOrFile == true){
	    fprintf(stderr, "this is a directory %s %d\n", __FILE__, __LINE__);
	    return false;
	}

	for(vector<FileBlock>::iterator iter4 =iter3->second.info.begin(); iter4 != iter3->second.info.end(); iter4++){
	    if(!isSameDc(iter4->serverNum, num)){
		dcAcceCnt++;
		break;
	    }
	    else if(iter4->serverNum != num){
		serverAcceCnt++;
		break;
	    }
	}

	for(vector<FileBlock>::iterator iter4 = iter3->second.info.begin(); iter4 != iter3->second.info.end(); iter4++)
	    serverArr->at(iter4->serverNum).freeStorage(fileBlockSize - iter4->restCapacity);

	return true;
    }
    else{
	fprintf(stderr, "file does not exist %s %d\n", __FILE__, __LINE__);
	return false;
    }
}

/*
 * op: specific operation
 * pathStack: organization path in stack , used by make directory operation , in other operations, only used the top entry
 * origName: 
 * newName: used by rename directory operation, recursively rename 
 * resultMap: used by make directory,  return new results of path-serverNumber pair, also could be used by other operation
 * Exist: indicate if specific directory or file has existed 
 * preServerNum: if exist = true, which indicates that related dictory has existed, preServerNum is the existed directory file's serverNum
 * info: if exist = true, which indicates that related file has existed, info is the fileblock of this file 
 * size: indicate write operation's volume 
 * serverAcceCnt: statistics for across server access
 * dcAcceCnt: statistics for across datacenter access 
 */

bool Server::getMessage(const string op, stack<string> pathStack, const string origName, const string newName, map<string, uint16_t> &resultMap, const bool exist, const uint16_t preServerNum, const vector<FileBlock> info, const uint64_t size,   uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    serverAcceCnt++;

    //from gateway
    if(!op.compare("list directory"))
	return lsDir(pathStack.top(), serverAcceCnt, dcAcceCnt);

    else if(!op.compare("delete directory"))
	return delDir(pathStack.top(), serverAcceCnt, dcAcceCnt);
    
    else if(!op.compare("write file"))
	return writeFile(pathStack.top(), size, serverAcceCnt, dcAcceCnt);

    else if(!op.compare("read file"))
	return readFile(pathStack.top(), size, serverAcceCnt, dcAcceCnt);
    
    else if(!op.compare("delete file"))
	return delFile(pathStack.top(), serverAcceCnt, dcAcceCnt);

    else if(!op.compare("touch file"))
	return touchFile(pathStack.top(), exist, info,  serverAcceCnt, dcAcceCnt);

    else if(!op.compare("exist dirctory"))
	return existDir(pathStack.top());

    //from centralized concise
    else if(!op.compare("make directory")){
	while(!pathStack.empty()){
	    mkDir(pathStack.top(), exist, preServerNum, resultMap, serverAcceCnt, dcAcceCnt);
	    pathStack.pop();
	}
    }

    else if(!op.compare("move directory"))
	mvDir(pathStack.top(), serverAcceCnt, dcAcceCnt);

    else if(!op.compare("rename directory"))
	rnDir(origName, newName, resultMap, serverAcceCnt, dcAcceCnt);

    //from another server
    else if(!op.compare("store directory file"))
	storeDirFile(pathStack.top());
}

#endif
