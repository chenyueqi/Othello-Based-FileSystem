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
	bool touchMessage(const string path);
	bool writeMessage(const string path);
	bool readMessage(const string path);
	bool rmMessage(const string path);
	
	//directory related message
	bool lsMessage(const string path);
	bool rmrMessage(const string path);
	bool cpMessage(const string path1, const string path2);

	//TODO Othello
	
    public:
	Gateway(Central* c = NULL, vector<Server>* server = NULL): central(c), serverArr(server){}
	bool setting(Central* p1, vector<Server>* p2){central = p1; serverArr = p2;}

	bool getMessage(const string op, const string path1, const string path2);

	bool sendMessageToServer(const string op, const string path1, const string path2);
};

bool Gateway::getMessage(const string op, const string path1, const string path2)
{
    if(!op.compare("ls") || !op.compare("write") || !op.compare("read") || !op.compare("rm") || !op.compare("rmr") || !op.compare("cp") || !op.compare("touch"))
	return sendMessageToServer(op, path1, path2);
    else if(!op.compare("mkdir") || !op.compare("mvr"))
	return central->getMessage(op, path1, path2);
    else
	return false;
}

bool Gateway::sendMessageToServer(const string op, const string path1, const string path2)
{
    if(!op.compare("touch"))
	touchMessage(path1);

    else if(!op.compare("write")) 
	writeMessage(path1);

    else if(!op.compare("read")) 
	readMessage(path1);

    else if(!op.compare("rm")) 
	rmMessage(path1);

    else if(!op.compare("ls")) 
	lsMessage(path1);

    else if(!op.compare("rmr")) 
	rmrMessage(path1);

    else if(!op.compare("cp")) 
	cpMessage(path1, path2);
}

bool Gateway::touchMessage(const string path)
{

}

bool Gateway::writeMessage(const string path)
{

}

bool Gateway::readMessage(const string path)
{

}

bool Gateway::rmMessage(const string path)
{

}

bool Gateway::lsMessage(const string path)
{

}

bool Gateway::rmrMessage(const string path)
{

}

bool Gateway::cpMessage(const string path1, const string path2)
{

}

#endif
