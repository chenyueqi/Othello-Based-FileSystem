#include"client.h"
#include"gateway.h"
#include"server.h"
#include"common.h"
#include<fstream>
using namespace std;

vector<Server> serverArr;

Client client;
Gateway gateWay;

bool init();

int main(int argc, char* argv[])
{
    init();
//    gateWay.testConsHash();

    ifstream file(argv[1], ios::in);
    string message;
    fprintf(stderr, "\n");

    while(!file.eof()){
	getline(file, message);
	if(!message.compare(""))
	    continue;
	fprintf(stdout, "%s\n", message.c_str());
	client.sendMessage(message);
	fprintf(stdout, "\n");
    }
}

bool init()
{
    for(int i = 0 ; i < totalServer ; i++){
	Server s(i, defaultCapacity);
	serverArr.push_back(s);
    }

    //init gateway
    gateWay.setting(&serverArr);
    //init client
    client.setGateWay(&gateWay);
}
