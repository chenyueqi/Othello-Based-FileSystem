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
	    return dcLabel == (s1/(1 << serverPerDcBit));
	}

	bool isSameDc(uint16_t s1, uint16_t s2){
	    return (s1/ (1 << serverPerDcBit)) == (s2/ (1<<serverPerDcBit));
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
	bool exitMessage(const uint32_t num);
	bool checkMessage(const string path, const uint64_t size);

    public:
	Gateway(vector<Server>* server = NULL): serverArr(server){
	    srand((unsigned)time(NULL));
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

    else if(!op.compare("exit")) 
	return exitMessage(size);

    else if(!op.compare("check"))
	return checkMessage(path1, size);
}

bool Gateway::exitMessage(const uint32_t num)
{
    fprintf(stdout, "TOTAL %u severs have exited abruptedly\n", num);
    for(int i = 0 ; i < num ;){
	uint32_t candidate = rand()% (serverArr->size());
	if(serverArr->at(candidate).getState()){
	    serverArr->at(candidate).setState(false);
	    fprintf(stderr, "%u ", candidate);
	    i++;
	}
    }
}

bool Gateway::checkMessage(const string path, const uint64_t size)
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;
    map<string, objInfo> result;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    fprintf(stdout, "%u %u %u %lu\n", serverNum1, serverNum2, serverNum3, size);
    return true;
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

    uint8_t failcnt = 0;

    if(!(serverArr->at(serverNum1).getState() && serverArr->at(serverNum1).getMessage("touch file", path1, 0, result)))
	failcnt++;
    if(!(serverArr->at(serverNum2).getState() && serverArr->at(serverNum2).getMessage("touch file", path2, 0, result)))
	failcnt++;
    if(!(serverArr->at(serverNum3).getState() && serverArr->at(serverNum3).getMessage("touch file", path3, 0, result)))
	failcnt++;

    if(failcnt == 3)
	fprintf(stderr, "fail ! fail ! fail ! %s %d\n", __FILE__ , __LINE__);

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

    uint8_t failcnt = 0;

    if(!(serverArr->at(serverNum1).getState() && serverArr->at(serverNum1).getMessage("write file", path1, size, result)))
	failcnt++;
    if(!(serverArr->at(serverNum2).getState() && serverArr->at(serverNum2).getMessage("write file", path2, size, result)))
	failcnt++;
    if(!(serverArr->at(serverNum3).getState() && serverArr->at(serverNum3).getMessage("write file", path3, size, result)))
	failcnt++;

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

    if(failcnt == 3){
	fprintf(stderr, "fail ! fail ! fail ! %s %d\n", __FILE__ , __LINE__);
	return false;
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

    uint8_t failcnt = 0;

    if(isSameDc(serverNum1, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(serverArr->at(serverNum1).getState() && serverArr->at(serverNum1).getMessage("read file", path1, 0, result1)){
	if(isSameDc(serverNum1, dcLabel))
	    dataflowStat[0].size += result1.begin()->second.size;
	else
	    dataflowStat[1].size += result1.begin()->second.size;
	return true;
    }
    
    failcnt++;

    if(isSameDc(serverNum2, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(serverArr->at(serverNum2).getState() && serverArr->at(serverNum2).getMessage("read file", path2, 0, result2)){
	if(isSameDc(serverNum2, dcLabel))
	    dataflowStat[0].size += result2.begin()->second.size;
	else
	    dataflowStat[1].size += result2.begin()->second.size;
	return true;
    }

    failcnt++;

    if(isSameDc(serverNum3, dcLabel)){
	dataflowStat[0].cnt++;
	dataflowStat[0].size += messageSize;
    }
    else{
	dataflowStat[1].cnt++;
	dataflowStat[1].size += messageSize;
    }

    if(serverArr->at(serverNum3).getState() && serverArr->at(serverNum3).getMessage("read file", path3, 0, result3)){
	if(isSameDc(serverNum3, dcLabel))
	    dataflowStat[0].size += result3.begin()->second.size;
	else
	    dataflowStat[1].size += result3.begin()->second.size;
	return true;
    }

    failcnt++;

    if(failcnt == 3){
	fprintf(stdout, "fail ! fail ! fail ! %s %d\n", __FILE__ , __LINE__);
	return false;
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
    serverArr->at(serverNum1).getMessage("delete file", path1, 0, result);
    serverArr->at(serverNum2).getMessage("delete file", path2, 0, result);
    serverArr->at(serverNum3).getMessage("delete file", path3, 0, result);

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
    /*
    fprintf(stderr, "%s\n", path.c_str());
    for(map<string, objInfo>::iterator iter = result.begin(); iter != result.end(); iter++){
	fprintf(stderr, "%s ", iter->first.c_str());
	if(iter->second.dirOrFile)
	    fprintf(stderr, "directory\n");
	else
	    fprintf(stderr, "file %lu\n", iter->second.size);
    }
    */
    return true;
}

bool Gateway::mvMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow* dataflowStat)
{
    int i = 0;
    for(i = path1.size(); i > 1 && path1[i] != '/'; i--);
    string temp = path1.substr(0, i);

    if(!temp.compare(path2)){
	fprintf(stderr, "file already exists %s %s %s %d\n", path1.c_str(), path2.c_str(), __FILE__, __LINE__);
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

    if(serverArr->at(serverNum11).getMessage("move file", path11, 0, result1))
	fileSize = result1.begin()->second.size;
    if(serverArr->at(serverNum12).getMessage("move file", path12, 0, result2))
	fileSize = result2.begin()->second.size;
    if(serverArr->at(serverNum13).getMessage("move file", path13, 0, result3))
	fileSize = result3.begin()->second.size;

    for(i = path1.size(); i > 1 && path1[i] != '/'; i--);
    temp = path1.substr(i, path1.size());

    string newpath = path2 + temp;
    string path21 = "firstfirstfirst" + newpath;
    string path22 = "secondsecondsecond" + newpath;
    string path23 = "thirdthirdthird" + newpath;

    uint16_t serverNum21, serverNum22, serverNum23;
    getServerNum(path21, serverNum21);
    getServerNum(path22, serverNum22);
    getServerNum(path23, serverNum23);

    map<string, objInfo> result;
    serverArr->at(serverNum21).getMessage("write file", path21, fileSize, result);
    serverArr->at(serverNum22).getMessage("write file", path22, fileSize, result);
    serverArr->at(serverNum23).getMessage("write file", path23, fileSize, result);

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

bool Gateway::mvrMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow* dataflowStat)
{
    int i = 0;
    for(i = path1.size(); i > 1 && path1[i] != '/'; i--);
    string temp = path1.substr(0, i);

    if(!temp.compare(path2)){
	fprintf(stderr, "directory already exists %s %s %s %d\n", path1.c_str(), path2.c_str(), __FILE__, __LINE__);
	return true;
    }

    for(vector<Server>::iterator iter0 = serverArr->begin(); iter0 != serverArr->end(); iter0++){
	if(iter0->getState()){
	    map<string, objInfo> result;
	    iter0->getMessage("move directory", path1, 0, result);
	    if(isSameDc(iter0->getNum(), dcLabel)){
		dataflowStat[0].cnt++;
		dataflowStat[0].size += messageSize;
	    }
	    else{
		dataflowStat[1].cnt++;
		dataflowStat[1].size += messageSize;
	    }
	    for(map<string, objInfo>::iterator iter1 = result.begin(); iter1 != result.end(); iter1++){
		int i = 0;
		for(i = 0 ; i < iter1->first.size() && iter1->first[i] != '/'; i++);
		string temp1 = iter1->first.substr(0,i);

		string temp2 = iter1->first.substr(path1.size() + i, iter1->first.size());

		string newpath = temp1 + path2 + temp2;
		uint16_t serverNum;
		getServerNum(newpath, serverNum);
		if(iter1->second.dirOrFile){
		    map<string, objInfo> result1;
		    serverArr->at(serverNum).getMessage("make directory", newpath, 0, result1);
		    if(isSameDc(iter0->getNum(), serverNum)){
			dataflowStat[0].cnt++;
			dataflowStat[0].size += messageSize;
		    }
		    else{
			dataflowStat[1].cnt++;
			dataflowStat[1].size += messageSize;
		    }
		}
		else{
		    map<string, objInfo> result1;
		    serverArr->at(serverNum).getMessage("write file", newpath, iter1->second.size, result1);
		    if(isSameDc(iter0->getNum(), serverNum)){
			dataflowStat[0].cnt++;
			dataflowStat[0].size += messageSize;
			dataflowStat[0].size += iter1->second.size;
		    }
		    else{
			dataflowStat[1].cnt++;
			dataflowStat[1].size += messageSize;
			dataflowStat[1].size += iter1->second.size;
		    }
		}
	    }
	}
    }
}

bool Gateway::rmrMessage(const string path, const uint8_t dcLabel, dataflow* dataflowStat)
{
    map<string, objInfo> result;
    for(vector<Server>::iterator iter = serverArr->begin(); iter != serverArr->end(); iter++){
	if(iter->getState()){
	    iter->getMessage("delete directory", path, 0, result);
	    if(isSameDc(iter->getNum(), dcLabel)){
		dataflowStat[0].cnt++;
		dataflowStat[0].size += messageSize;
	    }
	    else{
		dataflowStat[1].cnt++;
		dataflowStat[1].size += messageSize;
	    }
	}
    }
    return true;
}

bool Gateway::cpMessage(const string path1, const string path2, const uint8_t dcLabel, dataflow* dataflowStat)
{
    if(!path1.compare(path2)){
	fprintf(stderr, "file %s does exist\n", path1.c_str());
	return false;
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

    uint64_t fileSize;
    if(serverArr->at(serverNum11).getMessage("copy file", path11, 0, result1))
	fileSize = result1.begin()->second.size;
    if(serverArr->at(serverNum12).getMessage("copy file", path12, 0, result2))
	fileSize = result2.begin()->second.size;
    if(serverArr->at(serverNum13).getMessage("copy file", path13, 0, result3))
	fileSize = result3.begin()->second.size;

    string path21 = "firstfirstfirst" + path2;
    string path22 = "secondsecondsecond" + path2;
    string path23 = "thirdthirdthird" + path2;

    uint16_t serverNum21, serverNum22, serverNum23;
    getServerNum(path21, serverNum21);
    getServerNum(path22, serverNum22);
    getServerNum(path23, serverNum23);

    map<string, objInfo> result;
    serverArr->at(serverNum21).getMessage("write file", path21, fileSize, result);
    serverArr->at(serverNum22).getMessage("write file", path22, fileSize, result);
    serverArr->at(serverNum23).getMessage("write file", path23, fileSize, result);

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

    serverArr->at(serverNum1).getMessage("make directory", path1, 0, result);
    serverArr->at(serverNum2).getMessage("make directory", path2, 0, result);
    serverArr->at(serverNum3).getMessage("make directory", path3, 0, result);

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
