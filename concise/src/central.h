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
	//TODO Othello
    public:
	Central(vector<Server>* s = NULL):serverArr(s){}
	bool setServerArr(vector<Server>* s){serverArr = s;}


};

#endif
