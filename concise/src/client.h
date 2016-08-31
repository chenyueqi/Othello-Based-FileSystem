#ifndef __CLIENT_H__
#define __CLIENT_H__

#include<iostream>
#include<sstream>
#include<vector>
#include<ctime>
#include<cstdio>
#include"common.h"

using namespace std;

class Client
{
    private:
	bool sendMessageToGateway(string intr, bool recursion, string path1, string path2);
	Gateway* gateWay;
    public:
	Client(Gateway* p = NULL):gateWay(p){}
	bool sendMessage(string message);
	bool setGateWay(Gateway* p){gateWay = p;}

};

bool sendMessageToGateway(string intr, bool recursion, string path1, string path2)
{
    return true;
}

bool Client::sendMessage(string message)
{
    stringstream me(message);
    string subMe;
    getline(me, subMe, ' ');

    if(!subMe.compare("mkdir"))
    {
	fprintf(stdout, "%s\n",  subMe.c_str());
    }
    else if(!subMe.compare("list"))
    {
	fprintf(stdout, "%s\n",  subMe.c_str());

    }
    else if(!subMe.compare("rm"))
    {
	fprintf(stdout, "%s\n",  subMe.c_str());

    }
    else if(!subMe.compare("mv"))
    {
	fprintf(stdout, "%s\n",  subMe.c_str());

    }
    else if(!subMe.compare("copy"))
    {
	fprintf(stdout, "%s\n",  subMe.c_str());

    }
    return false;

};



#endif
