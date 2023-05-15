#include <stdlib.h> /* exit, atoi, malloc, free */
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

char *compute_get_request(char *host, char *url, char *query_params,
                          vector<Cookie> cookies)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, string content,
                           vector<string> cookies)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

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

    sprintf(line, "Content-Length: %ld", content.size());
    compute_message(message, line);

    if (!cookies.empty())
    {
    }

    strcat(message, "\r\n");
    compute_message(message, content.c_str());

    free(line);
    return message;
}

Cookie::Cookie(string key, string value)
{
    this->key = key;
    this->value = value;
}

void registerFunct(char *host)
{
    char *message, *response;
    json j, response_json_parsed;
    string username, password;

    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    cout << endl
         << "======== Register ========" << endl
         << endl;
    // take credentials
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    // create json
    j["username"] = username;
    j["password"] = password;

    // create message
    message = compute_post_request(host, PATH_REGISTER, APPLICATION_JSON, j.dump(), vector<string>());

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);
    string resp = string(response);
    if (response == NULL || resp.size() == 0)
    {
        cout << "Error receiving response from server." << endl;
        free(message);
        free(response);
        close(sockfd);
        return;
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);
    if (code == 201)
    {
        cout << "Account created successfully." << endl;
    }
    else if (code == 400)
    {
        char *p = strstr(response, "\r\n\r\n"); // skip headers
        if (p != NULL)
        {
            p += 4;
        }
        // parse json
        response_json_parsed = json::parse(p);
        cout << response_json_parsed["error"] << endl;
    }
    else
    {
        cout << "Error creating account." << endl;
    }

    free(response);
    free(message);
    close(sockfd);
}

string loginFunct(char *host)
{
    char *message, *response;
    string username, password;
    json j, response_json_parsed;

    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    cout << endl
         << "======== Login ========" << endl
         << endl;
    // take credentials
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    // create json
    j["username"] = username;
    j["password"] = password;

    // create message
    message = compute_post_request(host, PATH_LOGIN, APPLICATION_JSON, j.dump(), vector<string>());

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);

    string resp = string(response);

    // check if response is null
    if (response == NULL || resp.size() == 0)
    {
        cout << "Server did not respond!" << endl;
        free(message);
        free(response);
        close(sockfd);
        return "";
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);
    if (code == 200)
    {
        cout << "Login successful." << endl;
        free(message);
        free(response);
        close(sockfd);
        // take session cookie
        string session_cookie = resp.substr(resp.find("connect.sid="));
        session_cookie = session_cookie.substr(0, session_cookie.find(";"));
        return session_cookie;
    }
    else if (code == 400)
    {
        char *p = strstr(response, "\r\n\r\n"); // skip headers
        if (p != NULL)
        {
            p += 4;
        }
        // parse json
        response_json_parsed = json::parse(p);
        cout << response_json_parsed["error"] << endl;
    }
    else
    {
        cout << "Error logging in." << endl;
    }

    free(response);
    free(message);
    close(sockfd);
    return "";
}

string getLibraryAccess(char* host, vector<Cookie> cookies)
{
    char *message, *response;
    json j, response_json_parsed;

    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    // create message
    message = compute_get_request(host, PATH_LIBRARY_ACCESS, NULL, cookies);

    // add cookie to message
    string cookie = "Cookie: ";
    for (int i = 0; i < cookies.size(); i++)
    {
        cookie += cookies[i].key + "=" + cookies[i].value;
        if (i != cookies.size() - 1)
        {
            cookie += "; ";
        }
    }

    // add cookie to message
    compute_message(message, cookie.c_str());

    cout << message << endl;
    return "";
}
