#include<fstream>
#include<sstream>
#include<cstdlib>

using namespace std;

const float throughPut = 1.9; // 0.9 Gbps
const float fluc = 0.6; // 0.6 Gbps

double getTP()
{
    double offset = rand()%10;
    offset = (offset - 5)/10*fluc;
    return (throughPut + offset);
}

int main(int argc, char* argv[])
{
    ifstream infile(argv[1], ios::in);
//    ofstream outfile(argv[2], ios::out);

    uint32_t cnt = 0;
    double latency_t = 0;
    bool flag = false;

    while(!infile.eof())
    {
	string line;
	getline(infile, line);
	stringstream newline(line);

	string term;
	newline >> term;

	if(!term.compare("write"))
	{
	    cnt++;
	    flag = true;
	}
	else if(!term.compare("#") && flag)
	{
	    uint32_t msgSize = 0;
	    uint16_t tmp = 0;
	    newline >> tmp;

	    newline >> tmp;
	    msgSize += tmp;

	    newline >> tmp;
	    msgSize += tmp;

	    double latency = (double)msgSize/(getTP()*1024); // since we need to translate to ms, we just ignore one 1024. Besides, we need to consider other factors which also influence the performace, we ignore another 1024.
	    latency_t += latency;
	}
	else
	    flag = false;
    }

    fprintf(stdout, "%s latency: %lf\n", argv[1], latency_t/cnt/3);
}
