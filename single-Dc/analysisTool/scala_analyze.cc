#include<fstream>
#include<sstream>
#include<cstdlib>
using namespace std;

struct server_trans_pair {
  unsigned int server_num;
  uint64_t data;
};

int main(int argc, char* argv[]) {
  struct server_trans_pair scala1[1033878];
  ifstream infile(argv[1], ios::in);
  uint64_t cnt = 0;

  while (!infile.eof()) {
    string line;
    getline(infile, line);
    stringstream new_line(line);

    string term;
    new_line >> term;

    if (term == "check") {
	  for (int i = 0; i < 3; i++) {
  		string line;
  		getline(infile, line);
  		stringstream new_line(line);  
		string tmp;
  		new_line >> tmp;
		new_line >> scala1[cnt].server_num;
		new_line >> scala1[cnt].data;
  		cnt++;
	  }
    } 
  }
  infile.close();

  infile.open(argv[2], ios::in);
  cnt = 0;
  uint64_t uncom_cnt;
  uint64_t tot_data;
  while (!infile.eof()) {
    string line;
    getline(infile, line);
    stringstream new_line(line);

    string term;
    new_line >> term;

    if (term == "check") {
	  for (int i = 0; i < 3; i++) {
  		string line;
  		getline(infile, line);
  		stringstream new_line(line);  
		string tmp;
  		new_line >> tmp;
		unsigned int server_num;
		uint64_t data;
		new_line >> server_num;
		new_line >> data;
		if(server_num !=  scala1[cnt].server_num) {
		  uncom_cnt++;
		  tot_data += data;
		}
  		cnt++;
	  }
    } 
  }
  infile.close();
  fprintf(stdout, "total %lu move\n", uncom_cnt);
  fprintf(stdout, "%lu KB move per file\n", tot_data/cnt/1024);
  
}
