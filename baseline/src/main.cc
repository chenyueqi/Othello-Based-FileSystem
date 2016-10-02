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

    client.setTestOp(argv[2]);
//    gateWay.testConsHash();

    ifstream file(argv[1], ios::in);
    string message;

    while(!file.eof()){
	getline(file, message);
	if(!message.compare(""))
	    continue;
//	fprintf(stderr, "%s\n", message.c_str());
	client.sendMessage(message);
//	fprintf(stderr, "\n");
    }

    fprintf(stdout, "%s\t", argv[2]);

    client.getStat();
}

bool init()
{
    uint16_t cnt = 0;
    for(int i = 0 ; i < dcNum ; i++){
	for(int j = 0; j < (1 << serverPerDcBit); j++){
	    if(j < datacenter[i]){
		Server s(cnt, true, defaultCapacity);
		serverArr.push_back(s);
	    }
	    else{
		Server s(cnt, false, defaultCapacity);
		serverArr.push_back(s);
	    }
	    cnt++;
	}
    }

    /*

    for(vector<Server>::iterator iter = serverArr.begin(); iter != serverArr.end(); iter++){
	if(iter->getState())
	    fprintf(stdout, "%u\n", iter->getNum());
    }
    */

    //init gateway
    gateWay.setting(&serverArr);
    //init client
    client.setGateWay(&gateWay);
}
