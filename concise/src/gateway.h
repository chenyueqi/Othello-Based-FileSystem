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
	//TODO Othello
	
    public:
	Gateway(Central* c = NULL, vector<Server>* server = NULL): central(c), serverArr(server){}

	bool setting(Central* p1, vector<Server>* p2){central = p1; serverArr = p2;}

	bool getMessage(const string inst, const string path1, const string path2);

	bool sendMessageToCentral(const string inst, const string path1, const string path2);

	bool sendMessageToServer(const string inst, const string path1, const string path2);
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

bool Gateway::sendMessageToServer(const string inst, const string path1, const string path2)
{
}

#endif
