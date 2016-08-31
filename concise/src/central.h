#ifndef __CENTRAL_H__
#define __CENTRAL_H__

#include<iostream>
#include<sstream>
#include<vector>
#include"common.h"
using namespace std;

class Central
{
    private:
	vector<Server>* serverArr;
	Gateway* gateway;
	//TODO Othello
	
    private:
	bool mkdirProcess(const string dirName);
	bool mvrProcess(const string path1, const string path2);
	bool updateToGateway();

    public:
	Central(vector<Server>* s = NULL, Gateway* g = NULL):serverArr(s), gateway(g){}
	bool setting(vector<Server>* s, Gateway* g){serverArr = s; gateway = g;}

	bool getMessage(const string op, const string path1, const string path2);
};

bool Central::getMessage(const string op, const string path1, const string path2)
{
    if(!op.compare("mkdir"))
	return mkdirProcess(path1);
    else if(!op.compare("mvr"))
	return mvrProcess(path1, path2);
    else
	return false;
}

bool Central::mkdirProcess(const string dirName)
{

}

bool Central::mvrProcess(const string path1, const string path2)
{

}


#endif
