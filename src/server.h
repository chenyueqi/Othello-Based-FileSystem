#ifndef __SERVER_H__
#define __SERVER_H__

#include<iostream>
#include<cstdint>
#include<string>
#include<ctime>
#include<map>
#include<vector>
using namespace std;


const uint64_t defaultCapacity = 100;

class DirectoryFileEntry
{
    private:
	uint16_t serverNum;
	bool directoryOrFile; // directory: true, file: false
    public:
	DirectoryFileEntry(uint16_t num=0, bool attribute=true):serverNum(num), directoryOrFile(attribute){}
	uint16_t getServerNum(){return serverNum;}
	bool getEntryAttribute(){return directoryOrFile;}
};

class DirectoryFile
{
    private:
	map<string, DirectoryFileEntry> dir;
    public:
	DirectoryFile(){}
	bool queryFileEntry(string fileName, uint16_t& serverNum, bool& directoryOrFile);
	bool deleteFileEntry(string fileName, uint16_t& serverNum, bool& directoryOrFile);
	bool addFileEntry(string fileName, uint16_t serverNum, bool directoryOrFile);
	bool listFileEntry();

};

bool DirectoryFile::queryFileEntry(string fileName, uint16_t& serverNum, bool& directoryOrFile)
{
    map<string, DirectoryFileEntry>::iterator iter = dir.find(fileName);
    if(iter == dir.end())
	return false;
    else
    {
	serverNum = iter->second.getServerNum();
	directoryOrFile = iter->second.getEntryAttribute();
	return true;
    }
}

bool DirectoryFile::deleteFileEntry(string fileName, uint16_t& serverNum, bool& directoryOrFile)
{
    map<string, DirectoryFileEntry>::iterator iter = dir.find(fileName);
    if(iter == dir.end())
	return true;
    else
    {
	serverNum = iter->second.getServerNum();
	directoryOrFile = iter->second.getEntryAttribute();
	dir.erase(iter);
	return true;
    }
    return false;
}

bool DirectoryFile::addFileEntry(string fileName, uint16_t serverNum, bool directoryOrFile)
{
    map<string, DirectoryFileEntry>::iterator iter = dir.find(fileName);
    if(iter == dir.end())
	return true;
    else
    {
	DirectoryFileEntry entry(serverNum, directoryOrFile);
	dir.insert(pair<string, DirectoryFileEntry>(fileName, entry));
	return true;
    }
    return false;
}

bool DirectoryFile::listFileEntry()
{
    for(map<string, DirectoryFileEntry>::iterator iter = dir.begin(); iter != dir.end(); iter++)
    {
	fprintf(stderr, "%s\t", iter->first.c_str());
	if(iter->second.getEntryAttribute() == true)
	    fprintf(stderr, "directory\t");
	else
	    fprintf(stderr, "file\n");
	fprintf(stderr, "%u\t", iter->second.getServerNum());

    }
}

class Server
{
    private:
	uint16_t no;
	uint64_t serverCapacity;
	uint64_t availCapacity;
	uint64_t usedCapacity;
	vector<Server>* serverArr;
	map<string, DirectoryFile> directoryFile;

    private:
	bool sendMessageToServer(uint16_t num, string intr);
	uint16_t getServerNumOfFile(string fileName);
	
    public:
	Server(uint16_t num, uint64_t capacity=defaultCapacity, vector<Server>* serverArr=NULL): //constructor
	    no(num), 
	    serverCapacity(capacity), 
	    availCapacity(serverCapacity), 
	    usedCapacity(0){}

	uint16_t getNo(){return no;}
	uint64_t getServerCapacity(){return serverCapacity;}
	uint64_t getAvailableCapacity(){return availCapacity;}
	uint64_t getUsedCapacity(){return usedCapacity;}

	//XXX
	uint16_t  getMessage(const string inst, const string path, const uint16_t attach); // message interface
};

#endif
