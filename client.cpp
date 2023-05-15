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
#include <unordered_map>

int main(int argc, char *argv[])
{

    string host = string(IP) + string(PORT_CHAR);
    string readLine;
    vector<Cookie> cookies;
    string token;
    bool loggedIn = false;

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
            // take string before '='
            string key = session_cookie.substr(0, session_cookie.find("="));
            // take string after '='
            string value = session_cookie.substr(session_cookie.find("=") + 1);
            Cookie cookie(key, value);
            if (!cookies.empty()) {
                cookies[0] = cookie;
            } else {
                cookies.push_back(cookie);
            }
            loggedIn = true;
        }
        else if (readLine == "exit")
        {
            break;
        }
        else if (readLine == "enter_library") 
        {
            if (!loggedIn) {
                cout << "You are not logged in." << endl;
                continue;
            }
            token = getLibraryAccess((char *)host.c_str(), cookies);
            if (token == "") {
                continue;
            }
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
