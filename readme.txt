constants.h

->Holds constant values for every user and admin commands. 
->These are used throughout for the sake of readability. 
->Also contains constants to indicate which option form the menu is chosen. 
->The structs used to define user and account are also sepcified here.

client.c

->Program for client. 
->This can be thought of as the "front end", and contains code for the user and admin interface. 
->Messages are sent to the server via a socket.

server.c

->Program for a concurrent server. 
->This can be thought of as the "back end". 
->For every client, the server creates a new thread in order to service it further. 
->This helps ensure smooth querying of server from various clients independent of each other.

Working

->The server creates a new file for every user who signs up. 
->The name of this file is username.txt. 
->If a file with a given username already exists, then that user cannot sign up. 
->Every file contains first a struct which has user details - username, password and type - followed by a struct which has account details - the balance amount. 
->All queries are answered by opening the corresponding file, and reading from it. 
->In order to delete a user, the corresponding file is deleted.

Compilation

To compile for server : make server
To compile for client : make client

Execute

To execute server : ./server
To execute client : ./client

Clean

make clean