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

    serverArr[0].testDirFile();
    fprintf(stderr, "\n");

    ifstream file(argv[1], ios::in);
    string message;

    while(!file.eof()){
	getline(file, message);
	fprintf(stderr, "%s %s %d\n", message.c_str(),  __FILE__, __LINE__);
	client.sendMessage(message);
	serverArr[0].testDirFile();
	fprintf(stderr, "\n");
    }
    
   /* 
    st.pop();
    st.push("/home/user/foo/bar");
    st.push("/home/user/foo");
    serverAcceCnt = 0;
    dcAcceCnt = 0;
    serverArr[0].getMessage("make directory", st , "", "", resultMap, false, 0, info,  0, serverAcceCnt, dcAcceCnt);

    serverArr[0].testDirFile();
    fprintf(stderr, "\n");

    st.pop();
    st.push("/home/user/foo/bar1");
    serverAcceCnt = 0;
    dcAcceCnt = 0;
    serverArr[0].getMessage("write file", st, "", "", resultMap, false, 0, info,  100 , serverAcceCnt , dcAcceCnt);

    serverArr[0].testDirFile();
    fprintf(stderr, "\n");

    st.pop();
    st.push("/home/user/foo/bar2");
    serverAcceCnt = 0;
    dcAcceCnt = 0;
    serverArr[0].getMessage("touch file", st, "", "", resultMap, false, 0, info,  100 , serverAcceCnt , dcAcceCnt);

    serverArr[0].testDirFile();
    fprintf(stderr, "\n");

    serverArr[0].getMessage("write file", st, "", "", resultMap, false, 0, info,  100 , serverAcceCnt , dcAcceCnt);

    serverArr[0].testDirFile();
    fprintf(stderr, "\n");
    */
}

bool init()
{
    //init servers
    uint16_t serverPerDc = 128;
    for(int i = 0 ; i < serverPerDc * 1<<dcBit ; i++){
	Server s(i, defaultCapacity, &serverArr);
	serverArr.push_back(s);
    }

    //init central
    central.setting(&serverArr, &gateWay);

    //init gateway
    gateWay.setting(&central, &serverArr);

    //init client
    client.setGateWay(&gateWay);
}
