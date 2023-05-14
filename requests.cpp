#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "./single_include/nlohmann/json.hpp"
#include <iostream>

using namespace std;
using json = nlohmann::json;

#define PATH_REGISTER (char*) "/api/v1/tema/auth/register"
#define PATH_LOGIN (char*) "/api/v1/tema/auth/login"
#define PATH_LIBRARY_ACCESS (char*) "/api/v1/tema/library/access"
#define PATH_LIBRARY_BOOKS (char*) "/api/v1/tema/library/books"
#define APPLICATION_JSON (char*) "application/json"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1\r\n", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1\r\n", url);
    }

    compute_message(message, line);

    // Step 2: add the host

    sprintf(line, "Host: %s\r\n", host);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
       
    }

    // Step 4: add final new line
    strcat(message, "\n");


    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count)
{
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL) {
       
    }
    
    free(line);
    return message;
}

void registerFct(int sockfd, char* host) {
    
    cout << endl << "Register: " << endl << endl;

    char *message, *response;
    json j, response_json_parsed;
    string username, password;

    do {
        
        // take credentials
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        // create json
        
        j["username"] = username;
        j["password"] = password;

        // create message
        message = compute_post_request(host, PATH_REGISTER, APPLICATION_JSON, NULL, 0, NULL, 0);
        
        // add content length
        string content_length = "Content-Length: ";
        content_length += to_string(j.dump().size());
        compute_message(message, content_length.c_str());
        strcat(message, "\n");

        // add json
        compute_message(message, j.dump().c_str());

        // add final new line
        strcat(message, "\n");

        // send message
        send_to_server(sockfd, message);

        // receive response
        response = receive_from_server(sockfd);

        if (response == NULL) {
            cout << "Error receiving response from server." << endl;
            free(message);
            free(response);
            return;
        }

        // get response code
        string resp_code = strstr(response, "HTTP/1.1 ");
        resp_code = resp_code.substr(9, 3);

        int code = stoi(resp_code);

        if (code == 201) {
            cout << "Account created successfully." << endl;
            free(message);
            free(response);
            return;
        } else if (code == 400) {
            char *p = strstr(response, "\r\n\r\n"); // skip headers
            p += 4; // skip \r\n\r\n
            response_json_parsed = json::parse(p);
            cout << response_json_parsed["error"] << endl;
        } else {
            cout << "Error creating account." << endl;
        }

        free(message);
        free(response);
        
    } while (true);
}

void loginFct(int sockfd, char* host) {

    cout << endl << "Login: " << endl << endl;
    char *message, *response;
    string username, password;
    json j, response_json_parsed;

    do {
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        j["username"] = username;
        j["password"] = password;

        message = compute_post_request(host, PATH_LOGIN, APPLICATION_JSON, NULL, 0, NULL, 0);

        // add content length
        string content_length = "Content-Length: ";
        content_length += to_string(j.dump().size());

        compute_message(message, content_length.c_str());
        strcat(message, "\n");

        // add json
        compute_message(message, j.dump().c_str());

        // add final new line
        strcat(message, "\n");

        cout << message;

        // send message
        send_to_server(sockfd, message);

        // receive response
        response = receive_from_server(sockfd);

        // check if response is null
        if (response == NULL) {
            cout << "Server did not respond!" << endl;
            free(message);
            free(response);
            continue;
        }

        
        free(message);
        free(response);

    } while (true) ;
}



