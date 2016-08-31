#ifndef __GATE_H__
#define __GATE_H__

#include<iostream>
#include<sstream>
#include<vector>
#include"common.h"
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

bool Gateway::getMessage(const string inst, const string path1, const string path2)
{
    //TODO
    if(!inst.compare("list") || !inst.compare("write") || !inst.compare("read"))
	sendMessageToServer(inst, path1, path2);
    else
	sendMessageToCentral(inst, path1, path2);
}

bool Gateway::sendMessageToServer(const string inst, const string path1, const string path2)
{

}

bool Gateway::sendMessageToCentral(const string inst, const string path1, const string path2)
{

}


#endif
