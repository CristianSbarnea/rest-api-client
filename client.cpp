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

#define IP (char*) "34.254.242.81"
#define PORT_CHAR (char*) ":8080"
#define PORT (int) 8080

int main(int argc, char *argv[]) {
    
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("open_connection");
        exit(1);
    }

    string host = string(IP) + string(PORT_CHAR);

    // ask user if he wants to register or login
    string readLine;
    
    while (true) {

        cin >> readLine;
        if (readLine == "register") {
            registerFct(sockfd, (char*)host.c_str());
        } else if (readLine == "login") {
            string responseLog = loginFunct(sockfd, (char*)host.c_str());
            if (responseLog == "") {
                cout << "Invalid username or password" << endl;
                continue;
            }
        } else if (readLine == "exit") {
            break;
        } else {
            cout << "Invalid command" << endl;
            continue;
        }
        

    }
    
    return 0;
}
