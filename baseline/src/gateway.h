#ifndef __GATE_H__
#define __GATE_H__

#include<iostream>
#include<sstream>
#include<vector>
#include"server.h"
#include"consistentHash.h"
using namespace std;

class Gateway
{
    private:
	vector<Server>* serverArr;
	typedef consistent_hash_map<vnode_t, crc32_hasher> consistent_hash_t;
	consistent_hash_t consistent_hash_;

	bool getServerNum(const string path, uint16_t &id);

	//file related message
	bool touchMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool writeMessage(const string path, const uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool readMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool rmMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool mvMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	
	//directory related message
	bool lsMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool rmrMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool cpMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool mkdirMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool mvrMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);

    public:
	Gateway(vector<Server>* server = NULL): serverArr(server){
	    for(int i = 0 ; i < totalServer; i++)
		consistent_hash_.insert(vnode_t(i,1));
	}
	bool setting(vector<Server>* p2){serverArr = p2;}
	bool getMessage(const string op, const string path1, const string path2, const uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);
	bool sendMessageToServer(const string op, const string path1, const string path2, const uint64_t size,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime);

	bool testConsHash();

};

bool Gateway::getServerNum(const string path, uint16_t &id)
{
    boost::crc_32_type ret;
    ret.process_bytes(path.c_str(), totalServer);
    consistent_hash_t::iterator iter;
    iter = consistent_hash_.find(ret.checksum());
    id = iter->second.node_id;
    return true;
}

bool Gateway::testConsHash()
{
    for(consistent_hash_t::iterator iter = consistent_hash_.begin(); iter != consistent_hash_.end(); iter++)
	fprintf(stdout, "hash value:%u server id:%u %s %d\n", iter->first, iter->second.node_id,  __FILE__ , __LINE__);
}

bool Gateway::getMessage(const string op, const string path1, const string path2, const uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
	return sendMessageToServer(op, path1, path2, size, serverAcceCnt, dcAcceCnt, otherTime);
}

/*
 * touch, write, read, remove, cp, move for file and recursive remove 
 * has nothing to do with metadata in othello
 */
bool Gateway::sendMessageToServer(const string op, const string path1, const string path2, const uint64_t size,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    if(!op.compare("touch"))
	touchMessage(path1, serverAcceCnt, dcAcceCnt, otherTime);

    //id2 means size in write operation
    else if(!op.compare("write")) 
	writeMessage(path1 , size,  serverAcceCnt, dcAcceCnt, otherTime);

    else if(!op.compare("read")) 
	readMessage(path1, serverAcceCnt, dcAcceCnt, otherTime);

    else if(!op.compare("rm")) 
	rmMessage(path1, serverAcceCnt, dcAcceCnt, otherTime);

    else if(!op.compare("ls")) 
	lsMessage(path1, serverAcceCnt, dcAcceCnt, otherTime);

    else if(!op.compare("rmr")) 
	rmrMessage(path1, serverAcceCnt, dcAcceCnt, otherTime);

    else if(!op.compare("cp")) 
	cpMessage(path1, path2, serverAcceCnt, dcAcceCnt, otherTime);

    else if(!op.compare("mv")) 
	mvMessage(path1, path2, serverAcceCnt, dcAcceCnt, otherTime);

    else if(!op.compare("mvr")) 
	mvrMessage(path1, path2, serverAcceCnt, dcAcceCnt, otherTime);

    else if(!op.compare("mkdir")) 
	mkdirMessage(path1, serverAcceCnt, dcAcceCnt, otherTime);
}

bool Gateway::touchMessage(const string path,  uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    uint16_t serverNum = 0;
    //get faName's serverNum from othello using id
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    return serverArr->at(serverNum).getMessage("touch file", pathStack, "", "", useless0, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
}

bool Gateway::writeMessage(const string path, const uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    uint16_t serverNum = 0;
    //get faName's serverNum from othello using id
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    return serverArr->at(serverNum).getMessage("write file", pathStack, "", "", useless0, false, 0, useless1, size, serverAcceCnt, dcAcceCnt);
}

bool Gateway::readMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    uint16_t serverNum = 0;
    //get faName's serverNum from othello using id
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    return serverArr->at(serverNum).getMessage("read file", pathStack, "", "", useless0, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
}

bool Gateway::rmMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    uint16_t serverNum = 0;
    //get faName's serverNum from othello using id
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    return serverArr->at(serverNum).getMessage("delete file", pathStack, "", "", useless0, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
}

bool Gateway::lsMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    uint16_t serverNum = 0;
    //get path's serverNum from othello using id 
    //TODO
    stack<string> pathStack;
    pathStack.push(path);
    map<string, uint16_t> useless0;
    vector<FileBlock> useless1;

    return serverArr->at(serverNum).getMessage("list directory", pathStack, "", "", useless0, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
}

bool Gateway::mvMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
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

bool Gateway::rmrMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
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

    serverArr->at(serverNum1).getMessage("move directory", pathStack, "", "", delResult, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);

    serverArr->at(serverNum2).getMessage("delete directory", pathStack, "", "", delResult, false, 0, useless1, 0, serverAcceCnt, dcAcceCnt);
    
    for(map<string, uint16_t>::iterator iter = delResult.begin(); iter != delResult.end(); iter++);

    //FIXME deleted directory's id should be recycled
}

bool Gateway::cpMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
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

    serverArr->at(serverNum1).getMessage("copy file", pathStack, "", "", useless, false, 0, info, 0, serverAcceCnt, dcAcceCnt);

    uint64_t size = 0;
    for(vector<FileBlock>::iterator iter = info.begin(); iter != info.end(); iter++)
	size += (fileBlockSize - iter->restCapacity);

    pathStack.pop();
    pathStack.push(path2);
    serverArr->at(serverNum2).getMessage("write file", pathStack, "", "", useless, false, 0, info, size, serverAcceCnt, dcAcceCnt);
}

bool Gateway::mvrMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
}

bool Gateway::mkdirMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    uint16_t serverNum = 0;
    getServerNum(path, serverNum);
    fprintf(stdout, "%u %s %d\n", serverNum, __FILE__ , __LINE__);
    return true;
}

#endif
