#ifndef __CENTRAL_H__
#define __CENTRAL_H__

#include<iostream>
#include<sstream>
#include<vector>
#include<stack>
#include"common.h"
#include<map>
#include"server.h"
#include"./Othello/othello.h"
//#include"./OldOthello/othelloV7.h"
using namespace std;

class Central
{
    private:
	uint64_t prekey;
	uint16_t prevalue;
	vector<Server>* serverArr;
	Gateway* gateway;
	//XXX othello version
	Othello<uint64_t> oth;
	// support up to 512 * 64 = 32,768 different directory at the same time
	uint64_t idPool[512]; 
	uint64_t getId();
	
    private:
	bool mkdirProcess(const string dirName, const uint64_t id, map<string, uint64_t> &newdir, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool mvrProcess(const string path1, const string path2, const  uint64_t id1, const uint64_t id2,  map<string, uint64_t> &newdir, vector<string> &olddir, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool isPathExist(const string path, const uint16_t serverNum, map<string, uint16_t> &candidate, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);
	bool updateToGateway();

    public:
	Central(vector<Server>* s = NULL, Gateway* g = NULL):serverArr(s), gateway(g), prekey(0), prevalue(0), oth(dcBit+serverPerDcBit, &prekey, 1, false, &prevalue, sizeof(uint64_t)){
	    idPool[0] = 0x8000000000000000;
	    for(int i = 1 ; i < 512; i++)
		idPool[i] = 0;
	}

	/* XXX othello version
	Central(vector<Server>* s = NULL, Gateway* g = NULL):serverArr(s), gateway(g), prekey(0), prevalue(0), oth(&prekey, &prevalue, 1){
	    idPool[0] = 0x8000000000000000;
	    for(int i = 1 ; i < 512; i++)
		idPool[i] = 0;
	}*/

	bool setting(vector<Server>* s, Gateway* g){serverArr = s; gateway = g;}
	bool getMessage(const string op, const string path1, const string path2, const uint64_t id1, const uint64_t id2, map<string, uint64_t> &newdir, vector<string> &olddir, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool testOthello();
	//TODO update othello and idBitMap to all friends and 
	bool updateGateway();
};

uint64_t Central::getId()
{
    uint64_t id = 0;
    int i = 0;
    for( ; i < 512 && idPool[i] == 0xffffffffffffffff; i++)
	id += 64;
    uint64_t temp = idPool[i];

    int j = 0;
    for(; j < 64; j++){
	if(((temp<<j))>>63 == 0)
	    break;
    }

    id += j;

    idPool[i] |= (0x8000000000000000>>j);
    return id;
}

bool Central::testOthello()
{
    uint16_t key = 0;
    fprintf(stderr, "%u %s %d\n", oth.queryInt(key), __FILE__, __LINE__);
    //XXX othello version
//    fprintf(stderr, "%u %s %d\n", oth.query(key), __FILE__, __LINE__);
}

/*
 * support mkdir, recursive mv for directory
 */
bool Central::getMessage(const string op, const string path1, const string path2, const uint64_t id1, const uint64_t id2, map<string, uint64_t> &newdir, vector<string> &olddir, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    if(!op.compare("mkdir"))
	return mkdirProcess(path1, id1, newdir, serverAcceCnt, dcAcceCnt, otherTime);
    else if(!op.compare("mvr"))
	return mvrProcess(path1, path2, id1, id2, newdir, olddir, serverAcceCnt, dcAcceCnt, otherTime);
    else
	return false;
}

bool Central::isPathExist(const string path, const uint16_t serverNum, map<string, uint16_t> &candidate, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    stack<string> pathStack;
    pathStack.push(path);
    vector<FileBlock> useless;
    
    return serverArr->at(serverNum).getMessage("exist directory", pathStack, "", "", candidate, false, 0, useless, 0, serverAcceCnt, dcAcceCnt);
}

bool Central::mkdirProcess(const string dirName, const uint64_t id,  map<string, uint64_t> &newdir, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    //TODO get serverNum of dirName from othello using id
    //XXX othello version
    uint16_t serverNum = 0;

    map<string, uint16_t> candidate;

    isPathExist(dirName, serverNum, candidate, serverAcceCnt, dcAcceCnt);

    stack<string> pathStack;
    for(map<string, uint16_t>::iterator iter = candidate.begin(); iter != candidate.end(); iter++)
	pathStack.push(iter->first);

    map<string, uint16_t> resultMap;
    vector<FileBlock> useless;
    serverArr->at(serverNum).getMessage("make directory", pathStack, "", "", resultMap, false, 0, useless, 0, serverAcceCnt, dcAcceCnt);

    uint64_t newkey[20];
    uint16_t newvalue[20];
    int i = 0;
    
    //TODO
    for(map<string, uint16_t>::iterator iter = resultMap.begin(); iter != resultMap.end(); iter++){
	uint64_t id = getId();
	newdir.insert(pair<string , uint64_t>(iter->first, id));
	newkey[i] = id;
	newvalue[i] = iter->second;
	i++;
    }

    //XXX othello version
//    oth.batchupdateOrInsert(newkey, newvalue, i, false);


// TODO  for each result, change othelo,  build replication

    return true;
}

bool Central::mvrProcess(const string path1, const string path2, const uint64_t id1, const uint64_t id2, map<string, uint64_t> &newdir, vector<string> &olddir, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    uint16_t serverNum0 = 0;
    uint16_t serverNum1 = 0;
    uint16_t serverNum2 = 0;
    //TODO 
    //path0 = path1's father
    //serverNum0 = get path0 from othello
    //serverNum1 = get path1 from othello;
    //serverNum2 = get path2 from othello;
    
    stack<string> pathStack;
    pathStack.push(path1);
    map<string, uint16_t> resultMap;
    vector<FileBlock> useless;

    bool r1 = serverArr->at(serverNum0).getMessage("move directory", pathStack, "", "", resultMap, false, 0, useless, 0, serverAcceCnt, dcAcceCnt);

    if(!r1){
	fprintf(stderr, "directory %s dose not exist %s %d\n", path1.c_str(), __FILE__, __LINE__);
	return false;
    }

    pathStack.pop();

    serverArr->at(serverNum1).getMessage("rename directory", pathStack, path1, path2, resultMap, false, 0, useless, 0, serverAcceCnt, dcAcceCnt);

    int i = 0;
    for(i = path1.size(); i > 1 && path1[i] != '/'; i--);
    string temp = path1.substr(0,i);
    string suffix = path1.substr(temp.length(), path1.length());
    string newName = path2 + suffix;
    pathStack.push(newName);
    serverArr->at(serverNum2).getMessage("make directory", pathStack , "", "", resultMap, true, serverNum1, useless, 0, serverAcceCnt, dcAcceCnt);
   
    return true;
    //TODO
    //for each result in resultMap, add to othello and  build replication 
}

#endif
