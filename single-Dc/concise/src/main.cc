#include"client.h"
#include"server.h"
#include"gateway.h"
#include"central.h"
#include"common.h"
#include<stack>
#include<fstream>
using namespace std;

vector<Server> serverArr;
Client client;
Gateway gateWay;
Central central;

bool init();

int main(int argc, char* argv[])
{
    init();

    stack<string> st;
    st.push("/");
    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;
    map<string, uint16_t> resultMap;
    vector<FileBlock> info;
    serverArr[0].getMessage("store directory file", st, "", "", resultMap , false, 0, info, 0, serverAcceCnt, dcAcceCnt);

    ifstream file(argv[1], ios::in);
    string message;
    serverArr[0].testDirFile();
    fprintf(stdout, "\n");

    while(!file.eof()){
	getline(file, message);
	if(!message.compare(""))
	    continue;
	fprintf(stdout, "\n");
	fprintf(stdout, "%s\n", message.c_str());
	client.send_msg(message);
	serverArr[0].testDirFile();
	fprintf(stdout, "\n");
	client.test_obj_id();
	fprintf(stdout, "\n");
    }
}

bool init()
{
    //init servers
    uint16_t serverPerDc = 1<<serverPerDcBit;
    for(int i = 0 ; i < serverPerDc * 1<<dcBit ; i++){
	Server s(i, defaultCapacity, &serverArr);
	serverArr.push_back(s);
    }
    //init central
    central.setting(&serverArr, &gateWay);
    central.testOthello();
    //init gateway
    gateWay.set_central_server_arr(&central, &serverArr);
    //init client
    client.set_gateway(&gateWay);
}
