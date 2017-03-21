#include<fstream>
#include<sstream>
#include<cstdlib>
using namespace std;

int main(int argc, char* argv[])
{
  ifstream infile(argv[1], ios::in);
  uint32_t cnt = 0;
  uint64_t access_num = 0;
  unsigned long long trans = 0;

  while (!infile.eof()) {
    string line;
    getline(infile, line);
    stringstream new_line(line);

    string term;
    new_line >> term;

    if (term == "read") {
      cnt++;
    } else if(term == "#") {      
	  string tmp;
      new_line >> tmp;
	  if (tmp == "read-access")
		access_num++;
	  else if (tmp  == "read-data-transmission") {
		unsigned long long this_time = 0;
		new_line >> this_time;
		trans += this_time;
	//	fprintf(stdout, "%llu\n", this_time);
	  }
    }
  }
  fprintf(stdout, "read: %u times\n", cnt);
  fprintf(stdout, "access: %lu server(s) per time\n",access_num/cnt);
  fprintf(stdout, "trans data: %llu KB per time\n", trans/cnt/1024);
}
