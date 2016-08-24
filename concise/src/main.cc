#include"client.h"
#include"server.h"
#include<stack>

vector<Server> serverArr;
Client c;

bool init();

int main()
{
    Server s(0xd);

    stack<string> st;
    st.push("/home/user/foo");
    s.getMessage("store directory file", st ,0);

    st.pop();
    st.push("/home/user/foo/bar");
    s.getMessage("make directory", st ,0);

    s.testDirFile();
}

bool init()
{
    uint16_t serverPerDc = 128;
    for(int i = 0 ; i < serverPerDc * 1<<dcBit ; i++)
    {
	Server s(i, 0, &serverArr);
	serverArr.push_back(s);
    }

}
