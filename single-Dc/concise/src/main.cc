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

  ifstream file(argv[1], ios::in);
  string message;

  while (!file.eof()) {
	getline(file, message);
	if(message == "")
	  continue;
	fprintf(stdout, "\n");
	fprintf(stdout, "%s\n", message.c_str());
	client.send_msg(message);
	fprintf(stdout, "\n");
	client.test_obj_id();
	fprintf(stdout, "\n");
    }
}

bool init()
{
  uint64_t serverPerDc = 1<<serverPerDcBit;
  for (int i = 0 ; i < serverPerDc * 1<<dcBit ; i++) {
	Server s(i, defaultCapacity, &serverArr);
	serverArr.push_back(s);
  }
  central.set_gateway_server_arr(&serverArr, &gateWay);
  gateWay.set_central_server_arr(&central, &serverArr);
  client.set_gateway(&gateWay);
}
