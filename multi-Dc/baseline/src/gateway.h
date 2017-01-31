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
//	typedef Conhash<vnode, cityHash> ConhashType;
	ConhashType hashRing;

	bool getServerNum(const string path, uint16_t &serverNum);

	//file related message
	bool touchMessage(const string path);
	bool writeMessage(const string path, const uint64_t size);
	bool readMessage(const string path);
	bool rmMessage(const string path);
	bool mvMessage(const string path1, const string path2);
	
	//directory related message
	bool lsMessage(const string path);
	bool rmrMessage(const string path);
	bool cpMessage(const string path1, const string path2);
	bool mkdirMessage(const string path);
	bool mvrMessage(const string path1, const string path2);

	bool exitMessage(const uint32_t num);
	bool checkMessage(const string path, const uint64_t size);
	string serverInterpret(const uint16_t num)
	{
	    string result;
	    stringstream temp;
	    if(num < 256)
	    {
		uint16_t innernum = num;
		temp<<innernum;
		result = "0 "+temp.str();
	    }
	    else if(num < (256+256))
	    {
		uint16_t innernum = num;
		temp<<(innernum-256);
		result = "1 "+temp.str();
	    }
	    else if(num < (256+256+128))
	    {
		uint16_t innernum = num;
		temp<<(innernum-256-256);
		result = "2 "+temp.str();
	    }
	    else if(num < (256+256+128+512))
	    {
		uint16_t innernum = num;
		temp<<(innernum-256-256-128);
		result = "3 "+temp.str();
	    }
	    else if(num < (256+256+128+512+64))
	    {
		uint16_t innernum = num;
		temp<<(innernum-256-256-128-512);
		result = "4 "+temp.str();
	    }
	    return result;
	}

    public:
	Gateway(vector<Server>* server = NULL): serverArr(server)
    	{
	    for(uint16_t cnt = 0; cnt < server_num; cnt++)
		for(int i = 0; i < 10; i++)
		    hashRing.insert(vnode(cnt, i));
	}

	bool setting(vector<Server>* p2){serverArr = p2;}
	bool getMessage(const string op, const string path1, const string path2, const uint64_t size)
	{	
		return sendMessageToServer(op, path1, path2, size);
	}

	bool sendMessageToServer(const string op, const string path1, const string path2, const uint64_t size)
	{
		if(!op.compare("touch"))
			return touchMessage(path1);

    		else if(!op.compare("write")) 
			return writeMessage(path1 , size);//id2 means size in write operation

		else if(!op.compare("read")) 
			return readMessage(path1);

    		else if(!op.compare("rm")) 
			return rmMessage(path1);

    		else if(!op.compare("ls")) 
			return lsMessage(path1);

    		else if(!op.compare("rmr")) 
			return rmrMessage(path1);

    		else if(!op.compare("cp")) 
			return cpMessage(path1, path2);

    		else if(!op.compare("mv")) 
			return mvMessage(path1, path2);

    		else if(!op.compare("mvr")) 
			return mvrMessage(path1, path2);

    		else if(!op.compare("mkdir")) 
			return mkdirMessage(path1);

    		else if(!op.compare("exit")) 
			return exitMessage(size);

    		else if(!op.compare("check"))
			return checkMessage(path1, size);
	}

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

bool Gateway::exitMessage(const uint32_t num)
{
    fprintf(stdout, "# TOTAL %u severs have exited abruptedly #\n# ", num);
    for(int i = 0 ; i < num ;){
	uint32_t candidate = rand()% (serverArr->size());
	if(serverArr->at(candidate).getState()){
	    serverArr->at(candidate).setState(false);
	    fprintf(stdout, "%u ", candidate);
	    i++;
	}
    }
    fprintf(stdout, " #\n");
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

    fprintf(stdout, "# %u %lu #\n", serverNum1, size);
    fprintf(stdout, "# %u %lu #\n", serverNum2, size);
    fprintf(stdout, "# %u %lu #\n", serverNum3, size);
    return true;
}

bool Gateway::touchMessage(const string path)
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> useless_result;

    uint8_t cnt = 0;

    if(serverArr->at(serverNum1).getState())
    {
	if(!serverArr->at(serverNum1).getMessage("touch file", path1, 0, useless_result))
	    cnt++;
	fprintf(stdout, "# %u %u #\n", serverNum1, MSGSIZE);
    }
    else
	cnt++;

    if(serverArr->at(serverNum2).getState())
    {
	if(!serverArr->at(serverNum2).getMessage("touch file", path2, 0, useless_result))
	    cnt++;
	fprintf(stdout, "# %u %u #\n", serverNum2, MSGSIZE);
    }
    else
	cnt++;

    if(serverArr->at(serverNum3).getState())
    {
	if(!serverArr->at(serverNum3).getMessage("touch file", path3, 0, useless_result))
	    cnt++;
	fprintf(stdout, "# %u %u #\n", serverNum3, MSGSIZE);
    }
    else
	cnt++;

    if(cnt == 3)
    {
	fprintf(stdout, "ERROR!\n");
	return false;
    }
    else
	return true;
}

bool Gateway::writeMessage(const string path, const uint64_t size)
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> useless_result;

    uint8_t cnt = 0;

    if(serverArr->at(serverNum1).getState())
    {
	if(serverArr->at(serverNum1).getMessage("write file", path1, size, useless_result))
	    fprintf(stdout, "# %s %u %lu #\n", serverInterpret(serverNum1).c_str(), MSGSIZE, size);
	else
	{
	    fprintf(stdout, "# %u %u 0 #\n", serverNum1, MSGSIZE);
	    cnt++;
	}
    }
    else
	cnt++;

    if(serverArr->at(serverNum2).getState())
    {
	if(serverArr->at(serverNum2).getMessage("write file", path2, size, useless_result))
	    fprintf(stdout, "# %u %u %lu #\n", serverNum2, MSGSIZE, size);
	else
	{
	    fprintf(stdout, "# %u %u 0 #\n", serverNum2, MSGSIZE);
	    cnt++;
	}
    }
    else
	cnt++;

    if(serverArr->at(serverNum3).getState())
    {
	if(serverArr->at(serverNum3).getMessage("write file", path3, size, useless_result))
	    fprintf(stdout, "# %u %u %lu #\n", serverNum3, MSGSIZE, size);
	else
	{
	    fprintf(stdout, "# %u %u 0 #\n", serverNum3, MSGSIZE);
	    cnt++;
	}
    }
    else
	cnt++;

    if(cnt == 3)
    {
	fprintf(stdout, "ERROR!\n");
	return false;
    }
    else
	return true;
}

bool Gateway::readMessage(const string path)
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    uint8_t cnt = 0;

    if(serverArr->at(serverNum1).getState())
    {
	map<string, objInfo> result;
	if(serverArr->at(serverNum1).getMessage("read file", path1, 0, result))
	{
	    fprintf(stdout, "# %u %u %lu #\n", serverNum1, MSGSIZE, result.begin()->second.size);
	    return true;
	}
	else
	{
	    fprintf(stdout, "# %u %u 0 #\n", serverNum1, MSGSIZE);
	    cnt++;
	}
    }
    else
    {
	fprintf(stdout, "# %u #\n", serverNum1);
	cnt++;
    }

    if(serverArr->at(serverNum2).getState())
    {
	map<string, objInfo> result;
	if(serverArr->at(serverNum2).getMessage("read file", path2, 0, result))
	{
	    fprintf(stdout, "# %u %u %lu #\n", serverNum2, MSGSIZE, result.begin()->second.size);
	    return true;
	}
	else
	{
	    fprintf(stdout, "# %u %u 0 #\n", serverNum2, MSGSIZE);
	    cnt++;
	}
    }
    else
    {
	fprintf(stdout, "# %u #\n", serverNum2);
	cnt++;
    }

    if(serverArr->at(serverNum3).getState())
    {
	map<string, objInfo> result;
	if(serverArr->at(serverNum3).getMessage("read file", path3, 0, result))
	{
	    fprintf(stdout, "# %u %u %lu #\n", serverNum3, MSGSIZE, result.begin()->second.size);
	    return true;
	}
	else
	{
	    fprintf(stdout, "# %u %u 0 #\n", serverNum3, MSGSIZE);
	    cnt++;
	}
    }
    else
    {
	fprintf(stdout, "# %u #\n", serverNum3);
	cnt++;
    }

    if(cnt == 3)
    {
	fprintf(stdout, "ERROR!\n");
	fprintf(stderr, "ERROR!\n");
	return false;
    }
    else
	return true;
}

bool Gateway::rmMessage(const string path)
{
    string path1 = "firstfirstfirst" + path;
    string path2 = "secondsecondsecond" + path;
    string path3 = "thirdthirdthird" + path;

    uint16_t serverNum1, serverNum2, serverNum3;

    getServerNum(path1, serverNum1);
    getServerNum(path2, serverNum2);
    getServerNum(path3, serverNum3);

    map<string, objInfo> useless_result;
    uint8_t cnt = 0;

    if(serverArr->at(serverNum1).getState())
    {
	if(!serverArr->at(serverNum1).getMessage("delete file", path1, 0, useless_result))
	    cnt++;
	fprintf(stdout, "# %u %u #\n", serverNum1, MSGSIZE);
    }
    else
	cnt++;


    if(serverArr->at(serverNum2).getState())
    {
	if(!serverArr->at(serverNum2).getMessage("delete file", path2, 0, useless_result))
	    cnt++;
	fprintf(stdout, "# %u %u #\n", serverNum2, MSGSIZE);
    }
    else
	cnt++;

    if(serverArr->at(serverNum3).getState())
    {
	if(!serverArr->at(serverNum3).getMessage("delete file", path3, 0, useless_result))
	    cnt++;
	fprintf(stdout, "# %u %u #\n", serverNum3, MSGSIZE);
    }
    else
	cnt++;

    if(cnt == 3)
    {
	fprintf(stdout, "ERROR!\n");
	return false;
    }
    else
	return true;
}

bool Gateway::lsMessage(const string path)
{
    for(vector<Server>::iterator iter = serverArr->begin(); iter != serverArr->end(); iter++)
    {
	map<string, objInfo> useless_result;
	if(iter->getState())
	{
	    iter->getMessage("list directory", path, 0, useless_result);
	    fprintf(stdout, "# %u %u %lu #\n", iter->getNum(), MSGSIZE, useless_result.size() * ENTRYSIZE);
	}
    }
    return true;
}

bool Gateway::mvMessage(const string path1, const string path2)
{
    int i = 0;
    for(i = path1.size(); i > 1 && path1[i] != '/'; i--);
    string temp = path1.substr(0, i);

    if(!temp.compare(path2)){
	fprintf(stderr, "file %s already exists in server %s,  %s %d\n", path1.c_str(), path2.c_str(), __FILE__, __LINE__);
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

    fprintf(stdout, "# %s %u %u %u %lu #\n", serverInterpret(serverNum11).c_str(), serverNum21, MSGSIZE, MSGSIZE, result1.begin()->second.size);
    fprintf(stdout, "# %u %u %u %u %lu #\n", serverNum12, serverNum22, MSGSIZE, MSGSIZE, result2.begin()->second.size);
    fprintf(stdout, "# %u %u %u %u %lu #\n", serverNum13, serverNum23, MSGSIZE, MSGSIZE, result3.begin()->second.size);
}

bool Gateway::mvrMessage(const string path1, const string path2)
{
    int i = 0;
    for(i = path1.size(); i > 1 && path1[i] != '/'; i--);
    string temp = path1.substr(0, i);

    if(!temp.compare(path2)){
	fprintf(stderr, "directory %s already exists in server %s,  %s %d\n", path1.c_str(), path2.c_str(), __FILE__, __LINE__);
	return true;
    }

    for(vector<Server>::iterator iter0 = serverArr->begin(); iter0 != serverArr->end(); iter0++)
    {
	if(iter0->getState())
	{
	    map<string, objInfo> result;
	    iter0->getMessage("move directory", path1, 0, result);
	    for(map<string, objInfo>::iterator iter1 = result.begin(); iter1 != result.end(); iter1++)
	    {
		int i = 0;
		for(i = 0 ; i < iter1->first.size() && iter1->first[i] != '/'; i++);
		string temp1 = iter1->first.substr(0,i);

		string temp2 = iter1->first.substr(temp.size() + i, iter1->first.size());

		string newpath = temp1 + path2 + temp2;
		uint16_t serverNum;
		getServerNum(newpath, serverNum);
		if(iter1->second.dirOrFile)
		{
		    map<string, objInfo> result1;
		    if(!serverArr->at(serverNum).getMessage("make directory", newpath, 0, result1)) // it's possible that one of the destination directory has already existed one sub-directory sharing same name of source directory, just ignore it
			fprintf(stderr, " %s %s\n", iter1->first.c_str(), newpath.c_str());
		    fprintf(stdout, "# %u %u %u %u 0 #\n", iter0->getNum(), serverNum, MSGSIZE, MSGSIZE);
		}
		else
		{
		    map<string, objInfo> result1;
		    serverArr->at(serverNum).getMessage("write file", newpath, iter1->second.size, result1);
		    fprintf(stdout, "# %u %u %u %u %lu #\n", iter0->getNum(), serverNum, MSGSIZE, MSGSIZE, iter1->second.size);
		}
	    }
	}
    }
    return true;
}

bool Gateway::rmrMessage(const string path)
{
    map<string, objInfo> result;
    for(vector<Server>::iterator iter = serverArr->begin(); iter != serverArr->end(); iter++){
	if(iter->getState()){
	    iter->getMessage("delete directory", path, 0, result);
	    fprintf(stdout, "# %u %u #\n", iter->getNum(), MSGSIZE);
	    }
	}
    return true;
}

bool Gateway::cpMessage(const string path1, const string path2)
{
    if(!path1.compare(path2)){
	fprintf(stderr, "file %s exists\n", path1.c_str());
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

    fprintf(stdout, "# %u %u %u %u %lu #\n", serverNum11, serverNum21, MSGSIZE, MSGSIZE, result1.begin()->second.size);
    fprintf(stdout, "# %u %u %u %u %lu #\n", serverNum12, serverNum22, MSGSIZE, MSGSIZE, result2.begin()->second.size);
    fprintf(stdout, "# %u %u %u %u %lu #\n", serverNum13, serverNum23, MSGSIZE, MSGSIZE, result3.begin()->second.size);
}


bool Gateway::mkdirMessage(const string path)
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

    fprintf(stdout, "# %u %u #\n", serverNum1, MSGSIZE);
    fprintf(stdout, "# %u %u #\n", serverNum2, MSGSIZE);
    fprintf(stdout, "# %u %u #\n", serverNum3, MSGSIZE);

    return true;
}

#endif
