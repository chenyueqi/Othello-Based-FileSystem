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
using namespace std;


class Central
{
    private:
	uint64_t prekey;
	uint16_t prevalue;
	vector<Server>* serverArr;
	Gateway* gateway;
	Othello<uint64_t> oth;
	
    private:
	bool mkdirProcess(const string dirName);
	bool mvrProcess(const string path1, const string path2);
	bool updateToGateway();
	bool isPathExist(const string path, uint16_t &serverNum);
	bool isPathExist(const string path, uint16_t &serverNum, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt);

    public:
	Central(vector<Server>* s = NULL, Gateway* g = NULL):serverArr(s), gateway(g), prekey(0), prevalue(1), oth(dcBit+serverPerDcBit, &prekey, 1, false, &prevalue, sizeof(uint64_t)){}
	bool setting(vector<Server>* s, Gateway* g){serverArr = s; gateway = g;}
	bool getMessage(const string op, const string path1, const string path2);
	bool testOthello();
	//TODO update othello to all friends
	bool updateGateway();
};

bool Central::testOthello()
{
    uint16_t key = 0;
    fprintf(stderr, "%u %s %d\n", oth.queryInt(key), __FILE__, __LINE__);
}

/*
 * support mkdir, recursive mv for directory
 */
bool Central::getMessage(const string op, const string path1, const string path2)
{
    if(!op.compare("mkdir"))
	return mkdirProcess(path1);
    else if(!op.compare("mvr"))
	return mvrProcess(path1, path2);
    else
	return false;
}

bool Central::isPathExist(const string path, uint16_t &serverNum, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt)
{
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> resultMap;
    vector<FileBlock> useless;
    
    return serverArr->at(serverNum).getMessage("exist directory", pathStack, "", "", resultMap, false, 0, useless, 0, serverAcceCnt, dcAcceCnt);
}

bool Central::mkdirProcess(const string dirName)
{
    stack<string> pathStack;
    string currentName = dirName;
    uint16_t serverNum = 0;

    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;

    while(true)
    {
	if(!isPathExist(currentName, serverNum, serverAcceCnt, dcAcceCnt)){
	    pathStack.push(currentName);
	    int i = 0;
	    for(i = currentName.size(); i > 1 && currentName[i] != '/';i--);
	    currentName = currentName.substr(0, i);
	}
	else
	    break;
    }

    if(pathStack.empty()){
	fprintf(stderr, "directory already exist %s %d\n", __FILE__, __LINE__);
	return false;
    }

    map<string, uint16_t> resultMap;
    vector<FileBlock> useless;
    serverArr->at(serverNum).getMessage("make directory", pathStack, "", "", resultMap, false, 0, useless, 0, serverAcceCnt, dcAcceCnt);

// TODO  for each result, change othelo,  build replication

    return true;
}

bool Central::mvrProcess(const string path1, const string path2)
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
    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;
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
