#ifndef __GATE_H__
#define __GATE_H__

#include<iostream>
#include<sstream>
#include<vector>
#include"central.h"
using namespace std;

class Gateway
{
    private:
	Central* central;
	vector<Server>* serverArr;

	//file related message
	bool touchMessage(const string path, const uint64_t id);
	bool writeMessage(const string path, const uint64_t id);
	bool readMessage(const string path, const uint64_t id);
	bool rmMessage(const string path, const uint64_t id);
	bool mvMessage(const string path1, const string path2, const uint64_t id1, const uint64_t id2);
	
	//directory related message
	bool lsMessage(const string path, const uint64_t id);
	bool rmrMessage(const string path, const uint64_t id1, const uint64_t id2, vector<string> &olddir);
	bool cpMessage(const string path1, const string path2, const uint64_t id1, const uint64_t id2);

	//TODO Othello
	
    public:
	Gateway(Central* c = NULL, vector<Server>* server = NULL): central(c), serverArr(server){}
	bool setting(Central* p1, vector<Server>* p2){central = p1; serverArr = p2;}
	bool getMessage(const string op, const string path1, const string path2, const uint64_t id1, const uint64_t id2, map<string, uint64_t> &newdir, vector<string> &olddir);
	bool sendMessageToServer(const string op, const string path1, const string path2, const uint64_t id1, const uint64_t id2, map<string, uint64_t> &newdir, vector<string> &olddir);

	//TODO update othello
	bool getUpdate();
};

bool Gateway::getMessage(const string op, const string path1, const string path2, const uint64_t id1, const uint64_t id2, map<string, uint64_t> &newdir, vector<string> &olddir)
{
    if(!op.compare("ls") || !op.compare("write") || !op.compare("read") || !op.compare("rm") || !op.compare("rmr") || !op.compare("cp") || !op.compare("touch") || !op.compare("mv"))
	return sendMessageToServer(op, path1, path2, id1, id2, newdir, olddir);
    else if(!op.compare("mkdir") || !op.compare("mvr"))
	return central->getMessage(op, path1, path2, id1, id2, newdir, olddir);
    else
	return false;
}

/*
 * touch, write, read, remove, cp, move for file and recursive remove 
 * has nothing to do with metadata in othello
 */
bool Gateway::sendMessageToServer(const string op, const string path1, const string path2, const uint64_t id1, const uint64_t id2, map<string, uint64_t> &newdir, vector<string> &olddir)
{
    if(!op.compare("touch"))
	touchMessage(path1, id1);

    else if(!op.compare("write")) 
	writeMessage(path1 , id1);

    else if(!op.compare("read")) 
	readMessage(path1, id1);

    else if(!op.compare("rm")) 
	rmMessage(path1, id1);

    else if(!op.compare("ls")) 
	lsMessage(path1, id1);

    else if(!op.compare("rmr")) 
	rmrMessage(path1, id1, id2, olddir);

    else if(!op.compare("cp")) 
	cpMessage(path1, path2, id1, id2);

    else if(!op.compare("mv")) 
	mvMessage(path1, path2, id1, id2);
}

bool Gateway::touchMessage(const string path, const uint64_t id)
{
    uint16_t serverNum = 0;
    //get faName's serverNum from othello using id
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;
    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;

    return serverArr->at(serverNum).getMessage("touch file", pathStack, "", "", useless0, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
}

bool Gateway::writeMessage(const string path, const uint64_t id)
{
    uint16_t serverNum = 0;
    //get faName's serverNum from othello using id
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    //TODO where is size ?
    uint64_t size = 0;

    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;

    return serverArr->at(serverNum).getMessage("write file", pathStack, "", "", useless0, false, 0, useless1, size, serverAcceCnt, dcAcceCnt);
}

bool Gateway::readMessage(const string path, const uint64_t id)
{
    uint16_t serverNum = 0;
    //get faName's serverNum from othello using id
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    //TODO where is size ?
    uint64_t size = 0;

    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;

    return serverArr->at(serverNum).getMessage("read file", pathStack, "", "", useless0, false, 0, useless1, size, serverAcceCnt, dcAcceCnt);
}

bool Gateway::rmMessage(const string path, const uint64_t id)
{
    uint16_t serverNum = 0;
    //get faName's serverNum from othello using id
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;

    return serverArr->at(serverNum).getMessage("delete file", pathStack, "", "", useless0, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
}

bool Gateway::lsMessage(const string path, const uint64_t id)
{
    uint16_t serverNum = 0;
    //get path's serverNum from othello using id 
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;

    return serverArr->at(serverNum).getMessage("list directory", pathStack, "", "", useless0, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
}

bool Gateway::mvMessage(const string path1, const string path2, const uint64_t id1 , const uint64_t id2)
{
    uint16_t serverNum0 = 0;
    uint16_t serverNum1 = 0;
    //TODO
    //path0 =  path1's father
    //serverNum0 = get path0 from othello using id1
    //serverNum1 = get path2 from othello using id2

    stack<string> pathStack;
    pathStack.push(path1);
    map<string, uint16_t> resultMap;
    vector<FileBlock> info;
    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;
    bool r1= serverArr->at(serverNum0).getMessage("move file", pathStack, "", "", resultMap, false, 0, info, 0, serverAcceCnt, dcAcceCnt);

    if(!r1){
	fprintf(stderr, "file %s does not exist %s %d\n", path1.c_str(), __FILE__, __LINE__);
	return false;
    }

    pathStack.pop();

    int i = 0;
    for(i = path1.size(); i > 1 && path1[i] != '/'; i--);
    string temp = path1.substr(0, i);
    string suffix = path1.substr(temp.length(), path1.length());
    string newName = path2 + suffix;

    pathStack.push(newName);

    serverArr->at(serverNum1).getMessage("touch file", pathStack, "", "", resultMap, true, 0, info, 0, serverAcceCnt, dcAcceCnt);

    return true;
    //TODO
    //build replications
}

bool Gateway::rmrMessage(const string path, const uint64_t id1, const uint64_t id2, vector<string> &olddir)
{
    //TODO
    //get faName's serverNum from othello using id1
    //get path's severNum from othello using id2
    uint16_t serverNum1 = 0;
    uint16_t serverNum2 = 0;

    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> delResult;
    vector<FileBlock> useless1;

    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;

    serverArr->at(serverNum1).getMessage("move directory", pathStack, "", "", delResult, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);

    serverArr->at(serverNum2).getMessage("delete directory", pathStack, "", "", delResult, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
    
    for(map<string, uint16_t>::iterator iter = delResult.begin(); iter != delResult.end(); iter++)
	olddir.push_back(iter->first);

    //FIXME deleted directory's id should be recycled
}

bool Gateway::cpMessage(const string path1, const string path2, const uint64_t id1, const uint64_t id2)
{
    //TODO
    //get the serverNum of path1's father from othello using id1
    //get the serverNum of path2's father from othello using id2
    uint16_t serverNum1 = 0;
    uint16_t serverNum2 = 0;
    stack<string> pathStack;
    pathStack.push(path1);
    map<string, uint16_t> useless;
    vector<FileBlock> info;

    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;

    serverArr->at(serverNum1).getMessage("copy file", pathStack, "", "", useless, false, 0, info, 0, serverAcceCnt, dcAcceCnt);

    uint64_t size = 0;
    for(vector<FileBlock>::iterator iter = info.begin(); iter != info.end(); iter++)
	size += (fileBlockSize - iter->restCapacity);

    pathStack.pop();
    pathStack.push(path2);
    serverArr->at(serverNum2).getMessage("write file", pathStack, "", "", useless, false, 0, info, size, serverAcceCnt, dcAcceCnt);
}

#endif
