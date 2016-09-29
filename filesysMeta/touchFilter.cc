#include<fstream>
#include<sstream>
using namespace std;

int main(int argc, char* argv[])
{
    ifstream infile(argv[1], ios::in);
    ofstream outfile(argv[2], ios::out);

    while(!infile.eof()){
	string line;
	getline(infile, line);
	stringstream newline(line);

	string term;
	newline >> term;

	if(!term.compare("d")){
	    newline >> term;
	    newline >> term;
	    outfile << "mkdir "<< term << '\n';
	}
	else if(!term.compare("f")){
	    newline >> term;
	    newline >> term;
	    outfile << "touch " << term << '\n';
	}
    }

    infile.close();
    outfile.close();
}
