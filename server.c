#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "constants.h"

extern int signup(int, char *, char *, int);
extern int signin(int, char *, char *);
extern int transaction(char *, int, int);
extern int change_password(char *, char *);
extern char *get_details(char *);
extern int del_user(char *);
extern int modify_user(char *, char *, char *);
void *connection_handler(void *);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed...!");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt...!");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed...!");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen...!");
        exit(EXIT_FAILURE);
    }

    printf("ready to listen...!!\n");

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept...!");
            exit(EXIT_FAILURE);
        }
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, connection_handler, (void *)&new_socket) < 0)
        {
            perror("could not create thread...!");
            return 1;
        }
        puts("Handler assigned...!");
    }
    return 0;
}

void *connection_handler(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    int option, deposit_amt, withdraw_amt, rval, balance_amt;

    char *username = malloc(BUF_SIZE * sizeof(char));
    char *password = malloc(BUF_SIZE * sizeof(char));

    while (1)
    {
        char *type = malloc(BUF_SIZE * sizeof(char));
        char *new_username = malloc(BUF_SIZE * sizeof(char));
        char *rmsg = malloc(BUF_SIZE * sizeof(char));
        char *option_string = malloc(BUF_SIZE * sizeof(char));
        char *amt_string = malloc(BUF_SIZE * sizeof(char));

        read(sock, option_string, sizeof(option_string));
        option = atoi(option_string);

        if (option == SIGN_UP_AS_USER || option == SIGN_UP_AS_ADMIN || option == SIGN_UP_AS_JOINT)
        {
            read(sock, username, sizeof(username));
            read(sock, password, sizeof(password));
            rval = signup(option, username, password, 0);
            if (rval == -1)
                rmsg = "User could not be added\n";
            else
                rmsg = "User added successfully!\n";
        }
        else if (option == SIGN_IN_AS_USER || option == SIGN_IN_AS_ADMIN || option == SIGN_IN_AS_JOINT)
        {
            printf("d\n");
            read(sock, username, sizeof(username));
            read(sock, password, sizeof(password));
            rval = signin(option, username, password);
            if (rval == -1)
                rmsg = "sign in failed\n";
            else
                rmsg = "successfully signed in!\n";
        }
        else if (option == DEPOSIT)
        {
            read(sock, amt_string, sizeof(amt_string));
            deposit_amt = atoi(amt_string);
            rval = transaction(username, deposit_amt, option);
            if (rval != -1)
                sprintf(rmsg, "Deposit successfull...! Balance:%d", rval);
            else
                rmsg = "unable to deposit\n";
        }
        else if (option == WITHDRAW)
        {
            read(sock, amt_string, sizeof(amt_string));
            withdraw_amt = atoi(amt_string);
            rval = transaction(username, withdraw_amt, option);
            if (rval == -1)
                rmsg = "unable to withdraw\n";
            else
                sprintf(rmsg, "Withdraw successfull...! Balance:%d", rval);
        }
        else if (option == BALANCE_ENQUIRY)
        {
            balance_amt = transaction(username, 0, option);
            sprintf(rmsg, "%d", balance_amt);
        }
        else if (option == CHANGE_PASSWORD)
        {
            read(sock, password, sizeof(password));
            rval = change_password(username, password);
            if (rval == -1)
                rmsg = "unable to change password\n";
            else
                rmsg = "changed password successfully\n";
        }
        else if (option == VIEW_DETAILS)
        {
            rmsg = get_details(username);
        }
        else if (option == DEL_USER)
        {
            char *username = malloc(BUF_SIZE * sizeof(char));
            char *password = malloc(BUF_SIZE * sizeof(char));
            read(sock, username, sizeof(username));
            rval = del_user(username);
            printf("unlink returned %d\n", rval);
            if (rval == -1)
                rmsg = "unable to delete user\n";
            else
                rmsg = "user deleted successfully\n";
        }
        else if (option == MOD_USER)
        {
            char *username = malloc(BUF_SIZE * sizeof(char));
            char *password = malloc(BUF_SIZE * sizeof(char));

            read(sock, username, sizeof(username));
            read(sock, new_username, sizeof(new_username));
            read(sock, password, sizeof(password));

            rval = modify_user(username, new_username, password);
            if (rval == -1)
                rmsg = "unable to change user\n";
            else
                rmsg = "changed user successfully\n";
        }
        else if (option == SEARCH_USER)
        {
            char *username = malloc(BUF_SIZE * sizeof(char));
            read(sock, username, sizeof(username));
            rmsg = get_details(username);
        }
        else if (option == ADD_USER)
        {
            char *username = malloc(BUF_SIZE * sizeof(char));
            char *password = malloc(BUF_SIZE * sizeof(char));
            read(sock, type, sizeof(type));
            read(sock, username, sizeof(username));
            read(sock, password, sizeof(password));
            printf("type = %s username = %s pwd = %s\n", type, username, password);
            if (!strcmp(type, "1"))
                option = SIGN_UP_AS_USER;
            else
                option = SIGN_UP_AS_JOINT;
            rval = signup(option, username, password, 0);
            if (rval == -1)
                rmsg = "account could not be added\n";
            else
                rmsg = "successfully added account!\n";
        }
        else if (option == EXIT)
        {
            return 0;
        }
        send(sock, rmsg, BUF_SIZE * sizeof(char), 0);
    }
    return 0;
}