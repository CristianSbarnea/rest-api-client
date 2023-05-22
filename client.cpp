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

int main(int argc, char *argv[]) {
    string readLine, token;
    bool loggedIn = false, libraryAccess = false;
    vector<Cookie> cookies;
    string host = string(IP) + string(PORT_CHAR);

    while (true) {
        cin >> readLine;
        if (readLine == "register") {
            if (loggedIn) {
                cout << endl << "You are logged in. If you want to register, please log out." << endl << endl;
                continue;
            }
            registerFunct((char *)host.c_str());
        } else if (readLine == "login") {
            if (loggedIn) {
                cout << endl << "You are logged in. If you want to login again, please log out." << endl << endl;
                continue;
            }

            string session_cookie = loginFunct((char *)host.c_str());

            if (session_cookie == "") {
                loggedIn = false;
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
        } else if (readLine == "exit") {
            if (loggedIn) {
                logout((char *)host.c_str(), cookies);
            }
            cookies.clear();
            loggedIn = false;
            libraryAccess = false;
            token = "";
            break;
        } else if (readLine == "enter_library") {
            if (!loggedIn) {
                cout << endl << "You are not logged in." << endl << endl;
                continue;
            }

            token = getLibraryAccess((char *)host.c_str(), cookies);
            if (token == "") {
                libraryAccess = false;
                continue;
            }
            libraryAccess = true;
        } else if (readLine == "get_books") {
            if (!loggedIn) {
                cout << endl << "You are not logged in." << endl << endl;
                continue;
            }

            if (!libraryAccess) {
                cout << endl << "You do not have access to the library." << endl << endl;
                continue;
            }
            getBooks((char *)host.c_str(), token);
        } else if (readLine == "add_book") {
            if (!loggedIn) {
                cout << endl << "You are not logged in." << endl << endl;
                continue;
            }

            if (!libraryAccess) {
                cout << endl << "You do not have access to the library." << endl << endl;
                continue;
            }
            addBook((char *)host.c_str(), token);
        } else if (readLine == "get_book") {
            if (!loggedIn) {
                cout << endl << "You are not logged in." << endl << endl;
                continue;
            }

            if (!libraryAccess) {
                cout << endl << "You do not have access to the library." << endl << endl;
                continue;
            }
            getBook((char *)host.c_str(), token);
        } else if (readLine == "delete_book") {
            if (!loggedIn) {
                cout << endl << "You are not logged in." << endl << endl;
                continue;
            }

            if (!libraryAccess) {
                cout << endl << "You do not have access to the library." << endl << endl;
                continue;
            }
            deleteBook((char *)host.c_str(), token);
        } else if (readLine == "logout") {
            if (!loggedIn) {
                cout << endl << "You are not logged in." << endl << endl;
                continue;
            }

            logout((char *)host.c_str(), cookies);
            loggedIn = false;
            libraryAccess = false;
            cookies.clear();
            token = "";
        }
        else {
            cout << endl << "Invalid command" << endl << endl;
            continue;
        }
    }

    return 0;
}
