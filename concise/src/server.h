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
	uint64_t serverCapacity;
	uint64_t availCapacity;
	uint64_t usedCapacity;
	vector<Server>* serverArr;
	map<string, DirFile> dirFileMap;

    private:

	//directory operation
	bool mkDir(const string dirName, const bool dirExist, const uint16_t preServerNum, uint16_t &serverResult, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool lsDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt); // does not contain cross server access
	bool delDir(const string dirName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool mvDir(const string dirName, uint16_t &serverResult, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool rnDir(const string origName, const string newName, map<string, uint16_t> &result,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);

	bool storeDirFile(const string dirName);

	//file operation;
	bool touchFile(const string fileName, const bool fileExist, vector<FileBlock> info, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool writeFile(const string fileName, uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool readFile(const string fileName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool deleteFile(const string fileName,uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);

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

	//FIXME
	bool getMessage(const string inst, stack<string> pathStack, const string origName, const string newName, map<string, uint16_t> &resultMap, const bool dirExist, const uint16_t preServerNum, uint16_t &serverResult,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t size);
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
//TODO
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
	    serverArr->at(num).useStorage(fileBlockSize);
	}

	DirFileEntry entry;
	entry.dirOrFile = true;
	    
	if(dirExist)
	    entry.serverNum = preServerNum;
	else{
	    allocDirFileServer(iter3->serverNum, entry.serverNum);
	    serverArr->at(entry.serverNum).storeDirFile(dirName);
	    serverResult = entry.serverNum;
	    if(!isSameDc(iter3->serverNum, entry.serverNum))
		dcAcceCnt++;
	    else if(iter3->serverNum != entry.serverNum)
		serverAcceCnt++;
	}

	iter3->entryMap.insert(pair<string, DirFileEntry>(dirName, entry));
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
    map<string, DirFile>::iterator iter = dirFileMap.find(dirName);

    if(iter == dirFileMap.end())
	return true;
    else{
	for(vector<DirBlock>::iterator iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++){
	    if(!isSameDc(iter2->serverNum, num))
		dcAcceCnt++;
	    else if(iter2->serverNum != num)
		serverAcceCnt++;

	    for(map<string, DirFileEntry>::iterator iter3 = iter2->entryMap.begin(); iter3 != iter2->entryMap.begin(); iter3++){
		if(iter3->second.dirOrFile == true){
		    if(!isSameDc(iter2->serverNum, iter3->second.serverNum))
			dcAcceCnt++;
		    else if(iter2->serverNum != iter3->second.serverNum)
			serverAcceCnt++;

		    serverArr->at(iter3->second.serverNum).delDir(iter3->first, serverAcceCnt, dcAcceCnt);
		}
		else{
		    //TODO
		}
	    }
	}
    }
    return false;
}

bool Server::mvDir(const string dirName, uint16_t &serverResult, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    int i = 0;
    for(i = dirName.size(); i > -1 && dirName[i] != '/'; i--);
    string faName = dirName.substr(0, i);

    map<string, DirFile>::iterator iter = dirFileMap.find(faName);

    if(iter == dirFileMap.end()){
	fprintf(stderr, "BUG %s %d\n", __FILE__ , __LINE__);
	return false;
    }

    bool flag = false;
    map<string,DirFileEntry>::iterator iter3;
    vector<DirBlock>::iterator iter2;
    for(iter2 = iter->second.info.begin(); iter2 != iter->second.info.end(); iter2++){
	if(!isSameDc(iter2->serverNum, num))
	    dcAcceCnt++;
	else if(iter2->serverNum != num)
	    serverAcceCnt++;

	iter3 = iter2->entryMap.find(dirName);
	// if file does not exist in this diretory file entry, check the next one 
	if(iter3 == iter2->entryMap.end())
	    continue;
	else{
	    flag = true;
	    break;
	}
    }
    if(flag == true){
	if(iter3->second.dirOrFile == false){
	    fprintf(stderr, "this is a file %s %d\n", __FILE__, __LINE__);
	    return false;
	}
	serverResult = iter3->second.serverNum;
	iter2->entryMap.erase(iter3);
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

bool Server::readFile(const string fileName, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
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

bool Server::deleteFile(const string fileName,uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
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

bool Server::getMessage(const string inst, stack<string> pathStack, const string origName, const string newName, map<string, uint16_t> &resultMap, const bool dirExist, const uint16_t preServerNum, uint16_t &serverResult,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t size)
{
    serverAcceCnt++;

    //from gateway
    if(!inst.compare("list directory")){
	lsDir(pathStack.top(), serverAcceCnt, dcAcceCnt);
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

    else if(!inst.compare("move directory")){
	while(!pathStack.empty()){
	    mvDir(pathStack.top() , serverResult, serverAcceCnt, dcAcceCnt);
	    pathStack.pop();
	}
    }

    else if(!inst.compare("rename directory")){
	while(!pathStack.empty()){
	    rnDir(origName, newName, resultMap, serverAcceCnt, dcAcceCnt);
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
    else if(!inst.compare("write file")){
	while(!pathStack.empty()){
	    writeFile(pathStack.top(), size, serverAcceCnt, dcAcceCnt);
	    pathStack.pop();
	}
    }

    //from gateway 
    else if(!inst.compare("readfile")){
	while(!pathStack.empty()){
	    writeFile(pathStack.top(), size, serverAcceCnt, dcAcceCnt);
	    pathStack.pop();
	}
    }
}

#endif
