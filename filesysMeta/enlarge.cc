#include<fstream>
#include<sstream>
#include<iostream>

using namespace std;

int main()
{
    ifstream infile("briefdata",ios::in);
    ofstream outfile("enlargeddata", ios::out);

    while(!infile.eof()){
	string term1, term2, term3;
	infile >> term1 >> term2 >> term3;

	if(term3.length() == 0)
	    break;
	string newpath = "/0" + term3.substr(1, term3.length());
	outfile << term1 << "\t" << term2 << "\t" << newpath <<endl;

	newpath = "/1" + term3.substr(1, term3.length());
	outfile << term1 << "\t" << term2 << "\t" << newpath <<endl;
    }
}
