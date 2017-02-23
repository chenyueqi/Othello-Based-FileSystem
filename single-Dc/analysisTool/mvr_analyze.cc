#include<fstream>
#include<sstream>
#include<cstdlib>

using namespace std;

/*
const float throughPut = 1.9; // 0.9 Gbps
const float fluc = 0.6; // 0.6 Gbps

double getTP()
{
    double offset = rand()%10;
    offset = (offset - 5)/10*fluc;
    return (throughPut + offset);
}
*/

int main(int argc, char* argv[]) {
  ifstream infile(argv[1], ios::in); 
  uint32_t cnt = 0;
  //double latency_t = 0;
  uint64_t tot_msg_num = 0;
  bool flag = false;
  while (!infile.eof()) {
    string line;
    getline(infile, line);
    stringstream new_line(line);

    string term;
    new_line >> term;

    if (!term.compare("mvr")) {
      cnt++;
      flag = true;
    } else if(!term.compare("#") && flag) {      
      uint32_t msg_size = 0;	   
      uint16_t tmp = 0;
      new_line >> tmp; // source server
      new_line >> tmp; // dest server

      new_line >> tmp; // message size 1
      msg_size += tmp;
      new_line >> tmp; // message size 2
      msg_size += tmp; 

      new_line >> tmp; // file size
      msg_size += tmp; 
      tot_msg_num += msg_size;
      // double latency = (double)msgSize/(getTP()*1024); 
      // since we need to translate to ms, we just ignore one 1024. Besides, 
      // we need to consider other factors which also influence the performace, 
      // we ignore another 1024.
      // latency_t += latency;
    } else {
      flag = false;
    }
  }

  //fprintf(stdout, "%s latency: %lf\n", argv[1], latency_t/cnt);
  fprintf(stdout, "%s data transmission intra-datacenter: %.2lf Byte, count number: %u\n", 
          argv[1], (double)tot_msg_num/(double)cnt, cnt);
}
