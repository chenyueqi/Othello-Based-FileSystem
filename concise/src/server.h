#ifndef __SERVER_H__
#define __SERVER_H__

#include<iostream>
#include<cstdint>
#include<string>
#include<ctime>
#include<map>
#include<vector>
#include<sstream>

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
	bool queryDirectoryFileEntry(string fileName, uint16_t& serverNum, bool& directoryOrFile);
	bool deleteDirectoryFileEntry(string fileName, uint16_t& serverNum, bool& directoryOrFile);
	bool addDirectoryFileEntry(string fileName, uint16_t serverNum, bool directoryOrFile);
	bool listDirectoryFileEntry();

};

bool DirectoryFile::queryDirectoryFileEntry(string fileName, uint16_t& serverNum, bool& directoryOrFile)
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

bool DirectoryFile::deleteDirectoryFileEntry(string fileName, uint16_t& serverNum, bool& directoryOrFile)
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

bool DirectoryFile::addDirectoryFileEntry(string fileName, uint16_t serverNum, bool directoryOrFile)
{
    map<string, DirectoryFileEntry>::iterator iter = dir.find(fileName);
    if(iter != dir.end())
	return true;
    else
    {
	DirectoryFileEntry entry(serverNum, directoryOrFile);
	dir.insert(pair<string, DirectoryFileEntry>(fileName, entry));
	return true;
    }
    return false;
}

bool DirectoryFile::listDirectoryFileEntry()
{
    fprintf(stderr, "BEGIN\n");
    for(map<string, DirectoryFileEntry>::iterator iter = dir.begin(); iter != dir.end(); iter++)
    {
	fprintf(stderr, "%s\t", iter->first.c_str());
	if(iter->second.getEntryAttribute() == true)
	    fprintf(stderr, "directory\t");
	else
	    fprintf(stderr, "file\t");
	fprintf(stderr, "%u\n", iter->second.getServerNum());
    }
    fprintf(stderr, "END\n");
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

	bool addDirectoryFileEntry(string fileName, uint16_t &serverNum, bool &directoryOrFile);//XXX should be private , to test for public

	bool listDirectory(string directoryName);

	//XXX
	uint16_t  getMessage(const string inst, const string path, const uint16_t attach); // message interface
};


bool Server::addDirectoryFileEntry(string fileName, uint16_t &serverNum, bool &directoryOrFile)
{
    int i = 0;
    for(i = fileName.size(); i > -1 && fileName[i] != '/'; i--);

    string directoryName = fileName.substr(0, i);

    map<string, DirectoryFile>::iterator iter = directoryFile.find(directoryName);
    if(iter == directoryFile.end())
    {
	DirectoryFile* newDirectoryFile = new DirectoryFile();
	directoryFile.insert(pair<string, DirectoryFile>(directoryName, *newDirectoryFile));
	iter = directoryFile.find(directoryName);
    }

    iter->second.addDirectoryFileEntry(fileName, serverNum, directoryOrFile);
}

bool Server::listDirectory(string directoryName)
{
    map<string, DirectoryFile>::iterator iter = directoryFile.find(directoryName);
    if(iter == directoryFile.end())
	return false;
    else
	iter->second.listDirectoryFileEntry();
    return true;

}

//TODO
uint16_t getMessage(const string inst, const String path, const uint16_t attach)
{
    if(!inst.compare("store"))
	return 0;
}

#endif
