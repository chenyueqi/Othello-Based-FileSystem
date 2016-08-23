#include"client.h"
#include"server.h"
#include<stack>

int main()
{
    Client c;
    Server s(0);

    stack<string> st;
    st.push("/home/user/foo");
    s.getMessage("store directory file", st ,0);

    st.pop();
    st.push("/home/user/foo/bar");
    s.getMessage("make directory", st ,0);

    s.testDirFile();
}
