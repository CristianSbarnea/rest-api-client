
# Rest API Client

  

This is a small project done for faculty, where we had to implement a client for a

Rest API hosted on a server that simulates a virtual library. The client communicates

with the API by HTTP requests.

  

The client is waiting for commands from the keyboard, then computes the HTTP requests to

the server, and it will print the response from the server accordingly.

  

## Commands

  

>register

  

Awaits for the username and password, without spaces.

  

>login

  

Awaits for the username and password. If the login is successful, the client will store

a session cookie.

  

>enter_library

  

If the session cookie is still available, the client will get access to the library. The

client will store a JWT token.

  
  

>get_books

  
  

If the client has access, the books list of the client will be printed.

  
  

>get_book

  
  

Awaits for the id of the book wanted by the client. Details about the book

will be printed.

  

>add_book

  
  

Awaits for title, author, publisher, genre and page count. If the information

provided is correct, the book will be added to the library.

  

>delete_book

  
  

Awaits for the id of the book, and if it exists, it will be deleted from the

library.

  

>logout

  
  

If the client is logged in, he will be logged out and the cookie and JWT token will

be deleted.

  
  

>exit

  
  

The client stops running.

  
  

## Details

  
**JSON Parsing Library:** https://github.com/nlohmann/json.git , open-source library for parsing JSON
in C++.

**Running the project:**
>make

Then:
>./client

Must have g++ compiler locally.