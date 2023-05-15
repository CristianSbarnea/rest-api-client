#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <iostream>
#include "./single_include/nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[])
{

    string host = string(IP) + string(PORT_CHAR);
    string readLine;
    vector<string> cookies;

    while (true)
    {

        cin >> readLine;
        if (readLine == "register")
        {
            registerFunct((char *)host.c_str());
        }
        else if (readLine == "login")
        {
            string session_cookie = loginFunct((char *)host.c_str());

            if (session_cookie == "")
            {
                continue;
            }

            if (cookies.empty())
            {
                cookies.push_back(session_cookie);
            }
            else
            {
                cookies[0] = session_cookie;
            }
        }
        else if (readLine == "exit")
        {
            break;
        }
        else
        {
            cout << "Invalid command" << endl;
            continue;
        }
    }

    cookies.clear();

    return 0;
}
