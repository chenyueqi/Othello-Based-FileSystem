#include"client.h"
#include"server.h"
#include"gateway.h"
#include"central.h"
#include"common.h"
#include<stack>

vector<Server> serverArr;
Client client;
Gateway gateWay;
Central central;

bool init();

int main()
{
    init();

    fprintf(stderr, "%u\n", serverArr[0].getAvailableCapacity());

    stack<string> st;
    st.push("/home/user/foo");
    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;
    uint16_t serverResult = 0;
    map<string, uint16_t> resultMap;
    serverArr[0].getMessage("store directory file", st, "", "", resultMap , false, 0 ,serverResult, serverAcceCnt, dcAcceCnt, 0);
    fprintf(stderr, "available Capacity:%u server access count:%u dc access count:%u\n", serverArr[0].getAvailableCapacity(), serverAcceCnt, dcAcceCnt);

    serverArr[0].testDirFile();

    st.pop();
    st.push("/home/user/foo/bar");
    serverAcceCnt = 0;
    dcAcceCnt = 0;
    serverArr[0].getMessage("make directory", st , "", "", resultMap, false, 0, serverResult, serverAcceCnt, dcAcceCnt, 0);
    fprintf(stderr, "available Capacity:%u server access count:%u dc access count:%u\n", serverArr[0].getAvailableCapacity(), serverAcceCnt, dcAcceCnt);

    st.pop();
    st.push("/home/user/foo/bar1");
    serverAcceCnt = 0;
    dcAcceCnt = 0;
    serverArr[0].getMessage("write file", st, "", "", resultMap, false, 0, serverResult , serverAcceCnt , dcAcceCnt, 100);

    serverArr[0].testDirFile();
}

bool init()
{
    //init servers
    uint16_t serverPerDc = 128;
    for(int i = 0 ; i < serverPerDc * 1<<dcBit ; i++)
    {
	Server s(i, defaultCapacity, &serverArr);
	serverArr.push_back(s);
    }

    //init central
    central.setServerArr(&serverArr);

    //init gateway
    gateWay.setting(&central, &serverArr);

    //init client

    client.setGateWay(&gateWay);

}
