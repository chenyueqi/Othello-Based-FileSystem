#include<fstream>
#include<sstream>
#include<cstdlib>
using namespace std;

int main(int argc, char* argv[])
{
  ifstream infile(argv[1], ios::in);
  uint32_t cnt = 0;
  uint32_t error_num= 0;

  while (!infile.eof()) {
    string line;
    getline(infile, line);
    stringstream new_line(line);

    string term;
    new_line >> term;

    if (term == "read") 
      cnt++;
    else if(term == "ERROR!")
	  error_num++;
  }
  fprintf(stdout, "read: %u times\n", cnt);
  fprintf(stdout, "error: %u times\n", error_num);
}
