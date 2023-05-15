#ifndef _REQUESTS_
#define _REQUESTS_

using namespace std;

#include <string>
#include <vector>

#define IP (char*) "34.254.242.81"
#define PORT_CHAR (char*) ":8080"
#define PORT (int) 8080

#define PATH_REGISTER (char*) "/api/v1/tema/auth/register"
#define PATH_LOGIN (char*) "/api/v1/tema/auth/login"
#define PATH_LIBRARY_ACCESS (char*) "/api/v1/tema/library/access"
#define PATH_LIBRARY_BOOKS (char*) "/api/v1/tema/library/books"
#define APPLICATION_JSON (char*) "application/json"

class Cookie {
    public:
		Cookie(string key, string value);
        string key;
        string value;
};

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
							vector<Cookie> cookies);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, string content, 
							vector<string> cookies);

// computes a POST REGISTER request
void registerFunct(char* host);

// computes a POST LOGIN request
string loginFunct(char* host);

// computes a GET ENTER_LIBRARY request
string getLibraryAccess(char* host, vector<Cookie> cookies);

#endif
