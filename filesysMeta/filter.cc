#include<fstream>
#include<sstream>
using namespace std;

int main(int argc, char* argv[])
{
    ifstream infile(argv[1], ios::in);
    ofstream outfile(argv[2], ios::out);

    unsigned int cnt_file = 0;
    unsigned int cnt_directory = 0;
    while(!infile.eof())
    {
	string line;
	getline(infile, line);
	stringstream newline(line);

	if(infile.eof())
	    break;

	string term;
	newline >> term;

	if(term.find('d') != string::npos){
	    cnt_directory++;
	    outfile<<"d\t";
	}
	else{
	    cnt_file++;
	    outfile<<"f\t";
	}
	newline >> term;
	newline >> term;
	newline >> term;
	newline >> term;
	outfile << term << "\t";
	newline >> term;
	newline >> term;
	newline >> term;
	outfile << term << "\n";
    }
    infile.close();
    outfile.close();
    fprintf(stderr, "%u\n", cnt_file);
    fprintf(stderr, "%u\n", cnt_directory);
}
