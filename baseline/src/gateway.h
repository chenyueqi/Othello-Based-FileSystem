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
    string path1 = "0" + path;
    string path2 = "1" + path;
    string path3 = "2" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> result;
    serverArr->at(serverNum1).getMessage("touch file", path, 0, result);
    serverArr->at(serverNum2).getMessage("touch file", path, 0, result);
    serverArr->at(serverNum3).getMessage("touch file", path, 0, result);
}

bool Gateway::writeMessage(const string path, const uint64_t size, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    string path1 = "0" + path;
    string path2 = "1" + path;
    string path3 = "2" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> result;
    serverArr->at(serverNum1).getMessage("write file", path, size, result);
    serverArr->at(serverNum2).getMessage("write file", path, size, result);
    serverArr->at(serverNum3).getMessage("write file", path, size, result);
}

bool Gateway::readMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    string path1 = "0" + path;
    string path2 = "1" + path;
    string path3 = "2" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> result1;
    map<string, objInfo> result2;
    map<string, objInfo> result3;

    serverArr->at(serverNum1).getMessage("read file", path, 0, result1);
    serverArr->at(serverNum2).getMessage("read file", path, 0, result2);
    serverArr->at(serverNum3).getMessage("read file", path, 0, result3);
}

bool Gateway::rmMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    string path1 = "0" + path;
    string path2 = "1" + path;
    string path3 = "2" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> result;
    serverArr->at(serverNum1).getMessage("delete file", path, 0, result);
    serverArr->at(serverNum2).getMessage("delete file", path, 0, result);
    serverArr->at(serverNum3).getMessage("delete file", path, 0, result);
}

bool Gateway::lsMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
    string path1 = "0" + path;
    string path2 = "1" + path;
    string path3 = "2" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> result;
    serverArr->at(serverNum1).getMessage("list directory", path, 0, result);
//    serverArr->at(serverNum2).getMessage("delete file", path2, size, result);
//    serverArr->at(serverNum3).getMessage("delete file", path3, size, result3);
}

bool Gateway::mvMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
}

bool Gateway::rmrMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
}

bool Gateway::cpMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
}

bool Gateway::mvrMessage(const string path1, const string path2, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
}

bool Gateway::mkdirMessage(const string path, uint16_t &serverAcceCnt, uint8_t &dcAcceCnt, uint64_t &otherTime)
{
}

#endif
