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

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    //add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, string content,
                           vector<Cookie> cookies, string token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add necessary headers 
    if (token != "") {
        sprintf(line, "Authorization: Bearer %s", token.c_str());
        compute_message(message, line);
    }

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", content.size());
    compute_message(message, line);

    strcat(message, "\r\n");
    compute_message(message, content.c_str());

    free(line);
    return message;
}

// constructor for Cookie class
Cookie::Cookie(string key, string value)
{
    this->key = key;
    this->value = value;
}

// check if string is number
bool isNumber(string s)
{
    for (unsigned int i = 0; i < s.size(); ++i) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

bool findSpace(string s) {
    for (unsigned int i = 0; i < s.size(); ++i) {
        if (s[i] == ' ') {
            return true;
        }
    }
    return false;
}

// function that will register a user
void registerFunct(char *host)
{
    char *message, *response;
    json j, response_json_parsed;
    string username, password;
    
    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    cout << endl << "========= Register =========" << endl << endl;
    // take credentials
    getchar();
    cout << "username="; getline(cin, username);
    cout << "password="; getline(cin, password);

    // check
    if (username.size() == 0 || password.size() == 0) {
        cout << endl << "Username and password cannot be empty." << endl << endl;
        close(sockfd);
        return;
    }

    if (findSpace(username) || findSpace(password)) {
        cout << endl << "Username and password cannot contain spaces." << endl << endl;
        close(sockfd);
        return;
    }

    // create json
    j["username"] = username;
    j["password"] = password;

    // create message
    message = compute_post_request(host, PATH_REGISTER, APPLICATION_JSON, j.dump(), vector<Cookie>(), "");

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);
    string resp = string(response);
    if (response == NULL || resp.size() == 0) {
        cout << endl << "Error receiving response from server." << endl << endl;
        free(message);
        free(response);
        close(sockfd);
        return;
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);
    if (code >= 200 && code < 300) {
        cout << endl << "Account created successfully." << endl << endl;
    } else if (code >= 400) { 
        response_json_parsed = json::parse(resp.substr(resp.find("{")));
        string error = response_json_parsed["error"];
        cout << endl << error << endl << endl;
    } else {
        cout << endl << "Error creating account." << endl << endl;
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
    if (sockfd < 0) {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    cout << endl << "========= Login =========" << endl << endl;
    // take credentials
    getchar();
    cout << "username="; getline(cin, username);
    cout << "password="; getline(cin, password);

    // check
    if (username.size() == 0 || password.size() == 0) {
        cout << endl << "Username and password cannot be empty." << endl << endl;
        close(sockfd);
        return "";
    }

    if (findSpace(username) || findSpace(password)) {
        cout << endl << "Username and password cannot contain spaces." << endl << endl;
        close(sockfd);
        return "";
    }
    
    // create json
    j["username"] = username;
    j["password"] = password;

    // create message
    message = compute_post_request(host, PATH_LOGIN, APPLICATION_JSON, j.dump(), vector<Cookie>(), "");

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);
    string resp = string(response);
    if (response == NULL || resp.size() == 0) {
        cout << endl << "Server did not respond!" << endl << endl;
        free(message);
        free(response);
        close(sockfd);
        return "";
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);
    if (code >= 200 && code < 300) {
        cout << endl << "Login successful." << endl << endl;
        // take session cookie
        string session_cookie = resp.substr(resp.find("connect.sid="));
        session_cookie = session_cookie.substr(0, session_cookie.find(";"));
        free(message); // free memory
        free(response);
        close(sockfd);
        return session_cookie;
    } else if (code >= 400) {
        response_json_parsed = json::parse(resp.substr(resp.find("{")));
        string error = response_json_parsed["error"];
        cout << endl << error << endl << endl;
    } else {
        cout << endl << "Error logging in." << endl << endl;
    }

    free(response); // free memory
    free(message);
    close(sockfd);
    return "";
}

string getLibraryAccess(char *host, vector<Cookie> cookies)
{

    cout << endl << "========= Library Access =========" << endl;
    char *message, *response;

    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    // create message
    message = compute_get_request(host, PATH_LIBRARY_ACCESS, NULL, cookies);

    // add cookie to message
    string cookie = "Cookie: ";
    for (unsigned int i = 0; i < cookies.size(); ++i) {
        cookie += cookies[i].key + "=" + cookies[i].value;
        if (i != cookies.size() - 1) {
            cookie += "; ";
        }
    }

    // add cookie to message
    compute_message(message, cookie.c_str());

    // add authorization to message
    string auth = "Authorization: ";
    compute_message(message, auth.c_str());

    strcat(message, "\n");

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);
    string resp = string(response);
    if (response == NULL || resp.size() == 0) {
        cout << "Server did not respond!" << endl;
        free(message);
        free(response);
        close(sockfd);
        return "";
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);
    if (code >= 200 && code < 300) {
        cout << endl << "Access granted." << endl << endl;
        free(message);
        free(response);
        close(sockfd);
        json response_json_parsed = json::parse(resp.substr(resp.find("{"))); // parse json
        string token = response_json_parsed["token"]; // take token
        return token;
    } else if (code >= 400) {
        cout << endl << "Access denied." << endl;
        free(message);
        free(response);
        close(sockfd);
        json response_json_parsed = json::parse(resp.substr(resp.find("{")));
        string error = response_json_parsed["error"];
        cout << error << endl << endl;
        return "";
    } else {
        cout << endl << "Error getting library access." << endl << endl;
    }

    free(message);
    free(response);
    close(sockfd);
    return "";
}

void getBooks(char *host, string token)
{
    char *message, *response;

    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    cout << endl << "========= Books =========" << endl << endl;
    // create message
    message = compute_get_request(host, PATH_LIBRARY_BOOKS, NULL, vector<Cookie>());

    // add token to message
    string auth = "Authorization: Bearer " + token;
    compute_message(message, auth.c_str());

    strcat(message, "\n");

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);

    string resp = string(response);

    // check if response is null
    if (response == NULL || resp.size() == 0) {
        cout << "Server did not respond!" << endl;
        free(message);
        free(response);
        close(sockfd);
        return;
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);
    if (code >= 200 && code < 300) {
        json response_json_parsed = json::parse(resp.substr(resp.find("[")));
        if (response_json_parsed.size() == 0) {
            cout << endl << "No books in library." << endl << endl;
            free(message);
            free(response);
            close(sockfd);
            return;
        }
        for (unsigned int i = 0; i < response_json_parsed.size(); ++i) {
            string book = response_json_parsed[i]["title"];
            cout << i + 1 << ". " << book << " - ID " << response_json_parsed[i]["id"] << endl;
        }
        cout << endl;
    } else if (code >= 400) {
        cout << endl << "Error getting books." << endl;
        json response_json_parsed = json::parse(resp.substr(resp.find("{")));
        string error = response_json_parsed["error"];
        cout << error << endl << endl;
    } else {
        cout << "Error getting books." << endl << endl;
    }
    free(message);
    free(response);
    close(sockfd);
}

void addBook(char *host, string token)
{
    char *message, *response;

    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    cout << endl << "========= Add Book =========" << endl << endl;
    // take book info
    string title, author, genre, publisher, page_count;
    getchar();
    cout << "title=";
    getline(cin, title);
    cout << "author=";
    getline(cin, author);
    cout << "genre=";
    getline(cin, genre);
    cout << "publisher=";
    getline(cin, publisher);
    cout << "page_count=";
    getline(cin, page_count);

    if (title.size() == 0 || author.size() == 0 || genre.size() == 0 || publisher.size() == 0 
                    || (page_count.size() == 0 || !isNumber(page_count) || findSpace(page_count))) {
        cout << endl << "Invalid input." << endl << endl;
        return;
    }

    json j;
    j["title"] = title;
    j["author"] = author;
    j["genre"] = genre;
    j["page_count"] = page_count;
    j["publisher"] = publisher;

    // create message
    message = compute_post_request(host, PATH_LIBRARY_BOOKS, APPLICATION_JSON,
                                    j.dump(), vector<Cookie>(), token);

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);

    string resp = string(response);

    // check if response is null
    if (response == NULL || resp.size() == 0) {
        cout << endl << "Server did not respond! Try again later!" << endl << endl;
        free(message);
        free(response);
        close(sockfd);
        return;
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);
    if (code >= 200 && code < 300) {
        cout << endl
             << "Book added." << endl
             << endl;
    } else if (code >= 400) {
        cout << endl << "Error adding book." << endl;
        json response_json_parsed = json::parse(resp.substr(resp.find("{")));
        string err = response_json_parsed["error"];
        if (err == "Authorization header is missing") {
            cout << "You do not have access to the library!" << endl << endl;
        } else {
            cout << err << endl << endl;
        }
        return;
    } else {
        cout << endl << "Error adding book." << endl
             << endl;
    }

    free(message);
    free(response);
    close(sockfd);
}

void getBook(char *host, string token)
{
    char *message, *response;

    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    cout << endl << "========= Book Info =========" << endl << endl;
    // take book id
    getchar();
    string id;
    cout << "id=";
    getline(cin, id);
    if (id.size() == 0 || !isNumber(id) || findSpace(id)) {
        cout << endl << "Invalid input." << endl << endl;
        return;
    }

    // create message
    string path = (string)PATH_LIBRARY_BOOKS + "/" + id;
    message = compute_get_request(host, (char*)path.c_str(), NULL, vector<Cookie>());

    string auth = "Authorization: Bearer " + token;
    compute_message(message, auth.c_str());

    strcat(message, "\n");

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);

    string resp = string(response);

    // check if response is null
    if (response == NULL || resp.size() == 0) {
        cout << "Server did not respond!" << endl;
        free(message);
        free(response);
        close(sockfd);
        return;
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);
    if (code >= 200 && code < 300) {
        // parse json
        json resp_json = json::parse(resp.substr(resp.find("{")));
        string title = resp_json["title"];
        string author = resp_json["author"];
        string genre = resp_json["genre"];
        string publisher = resp_json["publisher"];
        int page_count = resp_json["page_count"];
        cout << endl << "Book " << id << ":" << endl;
        cout << "Title: " << title << endl;
        cout << "Author: " << author << endl;
        cout << "Genre: " << genre << endl;
        cout << "Publisher: " << publisher << endl;
        cout << "Page count: " << page_count << endl << endl;
    } else if (code >= 400) {
        cout << endl << "Error getting book." << endl;
        json response_json_parsed = json::parse(resp.substr(resp.find("{")));
        string err = response_json_parsed["error"];
        if (err == "Authorization header is missing") {
            cout << "You do not have acces to the library!" << endl << endl;
        } else {
            cout << err << endl << endl;
        }
    } else {
        cout << endl << "Error getting book." << endl << endl;
    }

    free(message);
    free(response);
    close(sockfd);
}

void deleteBook(char* host, string token) {
    char *message, *response;

    cout << endl << "========= Delete Book =========" << endl << endl;
    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    getchar();
    string id;
    cout << "id=";
    getline(cin, id);

    if (id.size() == 0 || !isNumber(id) || findSpace(id)) {
        cout << endl << "Invalid input." << endl << endl;
        return;
    }

    cout << "Are you sure you want to delete this book? (y/n)" << endl;
    string answer;
    cin >> answer;

    if (answer != "y") {
        cout << endl << "Book not deleted." << endl;
        return;
    }

    // create message
    string path = (string)PATH_LIBRARY_BOOKS + "/" + id;

    message = (char*)calloc(BUFLEN, sizeof(char));
    if (!message) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    char* line = (char*)(calloc(LINELEN, sizeof(char)));
    if (!line) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    sprintf(line, "DELETE %s HTTP/1.1", path.c_str());
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    string auth = "Authorization: Bearer " + token;
    compute_message(message, auth.c_str());

    strcat(message, "\n");

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);

    string resp = string(response);

    // check if response is null
    if (response == NULL || resp.size() == 0) {
        cout << "Server did not respond!" << endl;
        free(line);
        free(message);
        free(response);
        close(sockfd);
        return;
    }

    // take response code

    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);

    if (code >= 200 && code < 300) {
        cout << endl
             << "Book with ID " << id << " deleted." << endl
             << endl;
    }
    else if (code >= 400) {
        cout << endl << "Error deleting book." << endl;
        json response_json_parsed = json::parse(resp.substr(resp.find("{")));
        string err = response_json_parsed["error"];
        if (err == "Authorization header is missing") {
            cout << "You do not have acces to the library!" << endl << endl;
        } else {
            cout << err << "Check the id again!" << endl << endl;
        }
    } else {
        cout << endl << "Error deleting book." << endl << endl;
    }

    free(line);
    free(message);
    free(response);
    close(sockfd);
}

void logout(char* host, vector<Cookie> cookies) {
    char *message, *response;

    // open socket
    int sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("open_connection");
        exit(EXIT_FAILURE);
    }

    // create message
    message = compute_get_request(host, PATH_LOGOUT, NULL, cookies);

    // add cookie to message
    string cookie = "Cookie: ";
    for (unsigned int i = 0; i < cookies.size(); ++i) {
        cookie += cookies[i].key + "=" + cookies[i].value;
        if (i != cookies.size() - 1) {
            cookie += "; ";
        }
    }

    // add cookie to message
    compute_message(message, cookie.c_str());
    strcat(message, "\n");

    // send message
    send_to_server(sockfd, message);

    // receive response
    response = receive_from_server(sockfd);
    string resp = string(response);
    if (response == NULL || resp.size() == 0) {
        cout << "Server did not respond!" << endl;
        free(message);
        free(response);
        close(sockfd);
        return;
    }

    // take response code
    string response_code = resp.substr(9, 3);
    int code = stoi(response_code);

    if (code >= 200 && code < 300) {
        cout << endl << "Logged out." << endl << endl;
    }
    else if (code >= 400) {
        cout << endl << "Error logging out." << endl;
        json response_json_parsed = json::parse(resp.substr(resp.find("{")));
        string err = response_json_parsed["error"];
        cout << err << endl << endl;
    } else {
        cout << endl << "Error logging out." << endl << endl;
    }

    free(message);
    free(response);
    close(sockfd);
}