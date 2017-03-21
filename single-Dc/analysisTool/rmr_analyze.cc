#include<fstream>
#include<sstream>
#include<cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
  ifstream infile(argv[1], ios::in);
  uint32_t cnt = 0;
  uint64_t hit_num = 0;
  while (!infile.eof()) {
	string line;
	getline(infile, line);
	stringstream newline(line);

	string op;
	newline >> op;

	if (op == "rmr") {
	  cnt++;
	} else if (op =="#") {
	  string info;
	  newline >> info;
	  if(info == "rmr-hit")
		hit_num++;
	}
  }
  fprintf(stdout, "rmr: %u times\n", cnt);
  fprintf(stdout, "access: 256 serverr\n");
  fprintf(stdout, "average hit: %f serverr\n", (double)hit_num/(double)cnt);

  return 0;
}
