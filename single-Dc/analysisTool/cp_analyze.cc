#include<fstream>
#include<sstream>
#include<cstdlib>
#include<assert.h>

using namespace std;

int main(int argc, char* argv[]) {
  ifstream infile(argv[1], ios::in); 
  uint32_t cnt = 0;
  uint64_t access_cnt = 0;
  unsigned long long trans = 0;
  while (!infile.eof()) {
    string line;
    getline(infile, line);
    stringstream new_line(line);

    string term;
    new_line >> term;

    if (term == "cp") {
      cnt++;
    } else if(term == "#") {
	  string info;
	  new_line >> info;
	  if(info == "cp-access")
		access_cnt++;
	  if(info == "cp-data-transmission") {
		unsigned long long this_time = 0;
		new_line >> this_time;
		unsigned long long old_trans = trans;
		trans += this_time;
		assert(old_trans <= trans);
	  }
	}
  }
  fprintf(stdout, "cp: %u times\n", cnt);
  fprintf(stdout, "access: %lu per time\n", access_cnt/cnt);
  fprintf(stdout, "trans data: %llu KB per time\n", trans/cnt/1024);
  return 0;
}
