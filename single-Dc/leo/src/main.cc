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
  central.test_othello(0);
  uint64_t server_num[3];
  server_num[0] = 23;
  server_num[1] = 24;
  server_num[2] = 25;
  uint64_t id = central.allocate_id(server_num);
  fprintf(stderr, "id: %u\n", id);
  central.test_othello(id);
  id = central.allocate_id(server_num);
  fprintf(stderr, "id: %u\n", id);
  central.test_othello(id);
//  central.delete_id(0);
  central.delete_id(id);
  /*
  server_num[0] = 43;
  server_num[1] = 44;
  server_num[2] = 45;
  central.allocate_id(server_num);
  fprintf(stderr, "id: %u\n", id);
  central.test_othello(0);

  */
  /*
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
	*/
}

bool init()
{
  for (int i = 0 ; i < server_num ; i++) {
	Server s(i, defaultCapacity, &serverArr);
	serverArr.push_back(s);
  }
  central.set_gateway_server_arr(&serverArr, &gateWay);
  gateWay.set_central_server_arr(&central, &serverArr);
  client.set_gateway(&gateWay);
}
