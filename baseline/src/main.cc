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

    gateWay.testConsHash();

    /*
    for(consistent_hash_t::iterator iter = consistent_hash_.begin(); iter != consistent_hash_.end(); iter++ )
	cout<<iter->first<<" "<<iter->second.node_id<<endl;
    boost::crc_32_type ret;
    ret.process_bytes("/", totalServer);
    cout << ret.checksum()<<endl;
    consistent_hash_t::iterator iter;
    iter = consistent_hash_.find(ret.checksum());
    cout<<iter->second.node_id<<" "<<iter->second.vnode_id<<endl;
    */

    /*
    stack<string> st;
    st.push("/");
    uint16_t serverAcceCnt = 0;
    uint8_t dcAcceCnt = 0;
    map<string, uint16_t> resultMap;
    vector<FileBlock> info;
    serverArr[0].getMessage("store directory file", st, "", "", resultMap , false, 0, info, 0, serverAcceCnt, dcAcceCnt);

*/
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
