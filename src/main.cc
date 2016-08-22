#include"client.h"
#include"server.h"

int main()
{
    Client c;
    c.sendMessage("mkdir 2");
    c.sendMessage("list 2");
    c.sendMessage("rm 2");
    c.sendMessage("mv 2");
    c.sendMessage("copy 2");
    c.sendMessage("mkdir 2");
    Server s(0);
    fprintf(stderr, "%u\n", s.getNo());
    fprintf(stderr, "%u\n", s.getServerCapacity());
    fprintf(stderr, "%u\n", s.getAvailableCapacity());
    fprintf(stderr, "%u\n", s.getUsedCapacity());
}
