#include<fstream>
#include<sstream>
using namespace std;

int main()
{
    ifstream infile("brief", ios::in);
    ofstream outfile("dirinfo", ios::out);

    while(!infile.eof()){
	string line;
	getline(infile, line);
	stringstream newline(line);

	string term;
	newline >> term;

	if(!term.compare("d")){
	    newline >> term;
	    newline >> term;
	    outfile << term << '\n';
	}
    }
}