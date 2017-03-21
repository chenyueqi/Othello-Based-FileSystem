#include<fstream>
#include<sstream>
#include<cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
  ifstream infile(argv[1], ios::in);
  uint32_t cnt = 0;
  uint64_t access_num = 0;
  unsigned long long trans = 0;
  while (!infile.eof()) {
	string line;
	getline(infile, line);
	stringstream newline(line);

	string term;
	newline >> term;

	if (term == "write"){
		cnt++;
	} else if(term == "#") {
		string info;
		newline >> info;
		if (info == "write-access")
		  access_num ++;
		if (info == "write-data-transmission") {
		  unsigned long long this_time = 0;
		  newline >> this_time;
		  trans += this_time;
		}
	}
	if(cnt == 1000)
	  break;
  }

  fprintf(stdout, "write: %u times\n", cnt);
  fprintf(stdout, "access: %f server(s) per time\n", (double)access_num/(double)cnt);
  fprintf(stdout, "trans data: %llu KB per time\n", trans/cnt/1024);

  return 0;
}
