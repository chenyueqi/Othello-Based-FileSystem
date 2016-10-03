#ifndef __GATE_H__
#define __GATE_H__

#include<iostream>
#include<sstream>
#include<vector>
#include"server.h"
#include"conHash.h"
#include"common.h"
using namespace std;

class Gateway
{
    private:
	vector<Server>* serverArr;
	typedef Conhash<vnode, crc32> ConhashType;
	ConhashType hashRing;

	bool getServerNum(const string path, uint16_t &serverNum);
	bool isSameDc(uint16_t s1, uint8_t dcLabel){
	    return dcLabel == ( (s1-(s1>>(dcBit + 1)<<(dcBit + 1)))>>1);
	}

	bool isSameDc(uint16_t s1, uint16_t s2){
	    return ( (s1-(s1>>(dcBit + 1)<<(dcBit + 1)))>>1) == ( (s2-(s2>>(dcBit + 1)<<(dcBit + 1)))>>1);
	}

	//file related message
	bool touchMessage(const string path, const uint8_t dcLabel, dataflow dataflowStat[2]);
	bool writeMessage(const string path, const uint64_t size, const uint8_t dcLabel, dataflow dataflowStat[2]);
	bool readMessage(const string path, const uint8_t dcLabel, dataflow dataflowStat[2]);
	bool rmMessage(const string path, const uint8_t dcLabel, dataflow dataflowStat[2]);
	bool mvMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow dataflowStat[2]);
	
	//directory related message
	bool lsMessage(const string path, const uint8_t dcLabel, dataflow dataflowStat[2]);
	bool rmrMessage(const string path, const uint8_t dcLabel, dataflow dataflowStat[2]);
	bool cpMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow dataflowStat[2]);
	bool mkdirMessage(const string path, const uint8_t dcLabel, dataflow dataflowStat[2]);
	bool mvrMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow dataflowStat[2]);

    public:
	Gateway(vector<Server>* server = NULL): serverArr(server){
	    uint16_t cnt = 0;
	    for(int i = 0 ; i < dcNum; i++)
		for(int j = 0 ; j < (1 << serverPerDcBit); j++){
		    if(j < datacenter[i]){
			for(int k = 0 ; k < 10 ;k++)
			    hashRing.insert(vnode(cnt,k));
		    }
		    cnt++;
		}
	}
	bool setting(vector<Server>* p2){serverArr = p2;}
	bool getMessage(const string op, const string path1, const string path2, const uint64_t size, const uint8_t dcLabel, dataflow stat[2]);
	bool sendMessageToServer(const string op, const string path1, const string path2, const uint64_t size, const uint8_t dcLabel, dataflow stat[2]);

	bool testConsHash();
};

bool Gateway::getServerNum(const string path, uint16_t &serverNum)
{
    boost::crc_32_type ret;
    ret.process_bytes(path.c_str(), path.size());
    ConhashType::iterator iter;
    iter = hashRing.find(ret.checksum());
    serverNum = iter->second.id;
    return true;
}

bool Gateway::testConsHash()
{
    for(ConhashType::iterator iter = hashRing.begin(); iter != hashRing.end(); iter++)
	fprintf(stdout, "hash value:%u server id:%u %s %d\n",(unsigned) iter->first, (unsigned)iter->second.id,  __FILE__ , __LINE__);
}

bool Gateway::getMessage(const string op, const string path1, const string path2, const uint64_t size, const uint8_t dcLabel, dataflow dataflowStat[2])
{
    return sendMessageToServer(op, path1, path2, size, dcLabel, dataflowStat);
}

/*
 * touch, write, read, remove, cp, move for file and recursive remove 
 * has nothing to do with metadata in othello
 */
bool Gateway::sendMessageToServer(const string op, const string path1, const string path2, const uint64_t size, const uint8_t dcLabel, dataflow dataflowStat[2])
{
    if(!op.compare("touch"))
	return touchMessage(path1, dcLabel, dataflowStat);

    //id2 means size in write operation
    else if(!op.compare("write")) 
	writeMessage(path1 , size, dcLabel, dataflowStat);

    else if(!op.compare("read")) 
	readMessage(path1, dcLabel, dataflowStat);

    else if(!op.compare("rm")) 
	rmMessage(path1, dcLabel, dataflowStat);

    else if(!op.compare("ls")) 
	lsMessage(path1, dcLabel, dataflowStat);

    else if(!op.compare("rmr")) 
	rmrMessage(path1, dcLabel, dataflowStat);

    else if(!op.compare("cp")) 
	cpMessage(path1, path2, dcLabel, dataflowStat);

    else if(!op.compare("mv")) 
	mvMessage(path1, path2, dcLabel, dataflowStat);

    else if(!op.compare("mvr")) 
	mvrMessage(path1, path2, dcLabel, dataflowStat);

    else if(!op.compare("mkdir")) 
	return mkdirMessage(path1, dcLabel, dataflowStat);
}

bool Gateway::touchMessage(const string path, const uint8_t dcLabel, dataflow* dataflowStat)
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    map<string, objInfo> result;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    serverArr->at(serverNum1).getMessage("touch file", path, 0, result);
    serverArr->at(serverNum2).getMessage("touch file", path, 0, result);
    serverArr->at(serverNum3).getMessage("touch file", path, 0, result);

    if(isSameDc(serverNum1, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum2, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum3, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    return true;
}

bool Gateway::writeMessage(const string path, const uint64_t size, const uint8_t dcLabel, dataflow* dataflowStat)
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    map<string, objInfo> result;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    serverArr->at(serverNum1).getMessage("write file", path, size, result);
    serverArr->at(serverNum2).getMessage("write file", path, size, result);
    serverArr->at(serverNum3).getMessage("write file", path, size, result);

    if(isSameDc(serverNum1, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
	dataflowStat[0].size += size;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
	dataflowStat[1].size += size;
    }

    if(isSameDc(serverNum2, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
	dataflowStat[0].size += size;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
	dataflowStat[1].size += size;
    }

    if(isSameDc(serverNum3, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
	dataflowStat[0].size += size;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
	dataflowStat[1].size += size;
    }
    return true;
}

bool Gateway::readMessage(const string path, const uint8_t dcLabel, dataflow* dataflowStat)
{
    uint16_t serverNum1, serverNum2, serverNum3;
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> result1;
    map<string, objInfo> result2;
    map<string, objInfo> result3;

    if(isSameDc(serverNum1, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(serverArr->at(serverNum1).getMessage("read file", path, 0, result1)){
	if(isSameDc(serverNum1, dcLabel))
	    dataflowStat[0].size += result1.begin()->second.size;
	else
	    dataflowStat[1].size += result1.begin()->second.size;
	return true;
    }

    if(isSameDc(serverNum2, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(serverArr->at(serverNum2).getMessage("read file", path, 0, result2)){
	if(isSameDc(serverNum2, dcLabel))
	    dataflowStat[0].size += result2.begin()->second.size;
	else
	    dataflowStat[1].size += result2.begin()->second.size;
	return true;
    }

    if(isSameDc(serverNum3, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(serverArr->at(serverNum3).getMessage("read file", path, 0, result3)){
	if(isSameDc(serverNum3, dcLabel))
	    dataflowStat[0].size += result3.begin()->second.size;
	else
	    dataflowStat[1].size += result3.begin()->second.size;
	return true;
    }
    return true;
}

bool Gateway::rmMessage(const string path, const uint8_t dcLabel, dataflow* dataflowStat)
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> result;
    serverArr->at(serverNum1).getMessage("delete file", path, 0, result);
    serverArr->at(serverNum2).getMessage("delete file", path, 0, result);
    serverArr->at(serverNum3).getMessage("delete file", path, 0, result);

    if(isSameDc(serverNum1, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum2, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum3, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    return true;
}

bool Gateway::lsMessage(const string path, const uint8_t dcLabel, dataflow* dataflowStat)
{
    map<string, objInfo> result;
    uint16_t cnt = result.size();
    for(vector<Server>::iterator iter = serverArr->begin(); iter != serverArr->end(); iter++){
	if(iter->getState()){
	    iter->getMessage("list directory", path, 0, result);
	    if(isSameDc(iter->getNum(), dcLabel)){
		dataflowStat[0].cnt++;
		dataflowStat[0].size += messageSize;
		dataflowStat[0].size += ((result.size() - cnt) * objEntrySize);
		cnt = result.size();
	    }
	    else{
		dataflowStat[1].cnt++;
		dataflowStat[1].size += messageSize;
		dataflowStat[1].size += ((result.size() - cnt) * objEntrySize);
		cnt = result.size();
	    }
	}
    }
    fprintf(stderr, "%s\n", path.c_str());
    for(map<string, objInfo>::iterator iter = result.begin(); iter != result.end(); iter++){
	fprintf(stderr, "%s ", iter->first.c_str());
	if(iter->second.dirOrFile)
	    fprintf(stderr, "directory\n");
	else
	    fprintf(stderr, "file %lu\n", iter->second.size);
    }

    return true;
}

bool Gateway::mvMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow* dataflowStat)
{
    if(path1.find(path2, 0) != string::npos){
	fprintf(stderr, "file already exists %s %d\n", __FILE__, __LINE__);
	return true;
    }

    string path11 = "firstfirstfirst" + path1;
    string path12 = "secondsecondsecond" + path1;
    string path13 = "thirdthirdthird" + path1;

    uint16_t serverNum11, serverNum12, serverNum13;
    getServerNum(path11, serverNum11);
    getServerNum(path12, serverNum12);
    getServerNum(path13, serverNum13);
    
    map<string, objInfo> result1;
    map<string, objInfo> result2;
    map<string, objInfo> result3;

    uint64_t fileSize = 0;

    if(serverArr->at(serverNum11).getMessage("move file", path1, 0, result1))
	fileSize = result1.begin()->second.size;
    if(serverArr->at(serverNum12).getMessage("move file", path1, 0, result2))
	fileSize = result2.begin()->second.size;
    if(serverArr->at(serverNum13).getMessage("move file", path1, 0, result3))
	fileSize = result3.begin()->second.size;

    int i = 0;
    for(i = path1.size(); i > 1 && path1[i] != '/'; i--);
    string temp = path1.substr(0, i);

    string newpath = path2 + temp;
    string path21 = "firstfirstfirst" + newpath;
    string path22 = "secondsecondsecond" + newpath;
    string path23 = "thirdthirdthird" + newpath;

    uint16_t serverNum21, serverNum22, serverNum23;
    getServerNum(path21, serverNum21);
    getServerNum(path22, serverNum22);
    getServerNum(path23, serverNum23);

    map<string, objInfo> result;
    serverArr->at(serverNum21).getMessage("write file", newpath, fileSize, result);
    serverArr->at(serverNum22).getMessage("write file", newpath, fileSize, result);
    serverArr->at(serverNum23).getMessage("write file", newpath, fileSize, result);

    //former server analysis
    if(isSameDc(serverNum11, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum12, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum13, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    //later servver analysis
    if(isSameDc(serverNum21, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum22, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum23, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    //communication between servers
    if(isSameDc(serverNum11, serverNum21)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
	dataflowStat[0].size += fileSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
	dataflowStat[1].size += fileSize;
    }

    if(isSameDc(serverNum12, serverNum22)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
	dataflowStat[0].size += fileSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
	dataflowStat[1].size += fileSize;
    }

    if(isSameDc(serverNum13, serverNum23)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
	dataflowStat[0].size += fileSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
	dataflowStat[1].size += fileSize;
    }
}

//TODO
bool Gateway::rmrMessage(const string path, const uint8_t dcLabel, dataflow* dataflowStat)
{
}

//TODO
bool Gateway::cpMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow* dataflowStat)
{
    string path11 = "firstfirstfirst" + path1;
    string path12 = "secondsecondsecond" + path1;
    string path13 = "thirdthirdthird" + path1;

    uint16_t serverNum11, serverNum12, serverNum13;
    getServerNum(path11, serverNum11);
    getServerNum(path12, serverNum12);
    getServerNum(path13, serverNum13);
    
    map<string, objInfo> result1;
    map<string, objInfo> result2;
    map<string, objInfo> result3;

    uint64_t fileSize;
    if(serverArr->at(serverNum11).getMessage("copy file", path1, 0, result1))
	fileSize = result1.begin()->second.size;
    if(serverArr->at(serverNum12).getMessage("copy file", path1, 0, result2))
	fileSize = result2.begin()->second.size;
    if(serverArr->at(serverNum13).getMessage("copy file", path1, 0, result3))
	fileSize = result3.begin()->second.size;

    string path21 = "firstfirstfirst" + path2;
    string path22 = "secondsecondsecond" + path2;
    string path23 = "thirdthirdthird" + path2;

    uint16_t serverNum21, serverNum22, serverNum23;
    getServerNum(path21, serverNum21);
    getServerNum(path22, serverNum22);
    getServerNum(path23, serverNum23);

    map<string, objInfo> result;
    serverArr->at(serverNum21).getMessage("write file", path2, fileSize, result);
    serverArr->at(serverNum22).getMessage("write file", path2, fileSize, result);
    serverArr->at(serverNum23).getMessage("write file", path2, fileSize, result);
}

//TODO
bool Gateway::mvrMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow* dataflowStat)
{
}

bool Gateway::mkdirMessage(const string path, const uint8_t dcLabel, dataflow dataflowStat[2])
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    map<string, objInfo> result;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    serverArr->at(serverNum1).getMessage("make directory", path, 0, result);
    serverArr->at(serverNum2).getMessage("make directory", path, 0, result);
    serverArr->at(serverNum3).getMessage("make directory", path, 0, result);

    if(isSameDc(serverNum1, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum2, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(isSameDc(serverNum3, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    return true;
}

#endif
