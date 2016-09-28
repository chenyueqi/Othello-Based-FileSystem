#include<fstream>
#include<sstream>
using namespace std;

int main()
{
    ifstream infile("enlargeddata", ios::in);
    ofstream outfile("mkdirTestcase", ios::out);

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
    }
}
