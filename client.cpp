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

    char* host = (char*) calloc(50, sizeof(char));
    if (host == NULL) {
        perror("calloc");
        exit(1);
    }

    // create host
    strcat(host, IP);
    strcat(host, PORT_CHAR);
    
    // ask user if he wants to register or login
    string readLine;
    
    while (true) {
        cin >> readLine;

        if (readLine == "register") {
            registerFct(sockfd, host);
        } else if (readLine == "login") {
            loginFct(sockfd, host);
        } else {
            cout << "Invalid command" << endl;
        }
        

    }
    
    free(host);
    return 0;
}
