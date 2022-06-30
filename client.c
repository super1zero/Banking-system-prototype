#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"

int printMenu(int);
void interface(int);
int authentication(int, int);
void userInterface(int);
void adminInterface();

int main(int argc, char const *argv[])
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error...! \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported...! \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed...! \n");
        return -1;
    }

    interface(sock);
    return 0;
}

int printMenu(int menu)
{
    int option;
    switch (menu)
    {
    case STARTING_OPTIONS:
        printf("Welcome! Please enter one of the following options...!\n");
        printf("1 : Sign Up\n");
        printf("2 : Sign In\n");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            return SIGN_UP;
        case 2:
            return SIGN_IN;
        default:
            return INVALID;
        }

    case SIGN_UP_OPTIONS:
        printf("Enter type of user you would like to sign up as....!\n");
        printf("1 : Normal User\n");
        printf("2 : Joint Account User\n");
        printf("3 : Administrator\n");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            return SIGN_UP_AS_USER;
        case 2:
            return SIGN_UP_AS_JOINT;
        case 3:
            return SIGN_UP_AS_ADMIN;
        default:
            return INVALID;
        }
    case SIGN_IN_OPTIONS:
        printf("Enter type of user you would like to sign in as....!\n");
        printf("1 : Normal User\n");
        printf("2 : Joint Account User\n");
        printf("3 : Administrator\n");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            return SIGN_IN_AS_USER;
        case 2:
            return SIGN_IN_AS_JOINT;
        case 3:
            return SIGN_IN_AS_ADMIN;
        default:
            return -1;
        }
    case USER_OPTIONS:
        printf("Enter the operation you would like to perform...!\n");
        printf("1 : Deposit\n");
        printf("2 : Withdraw\n");
        printf("3 : Check Balance\n");
        printf("4 : Change Password\n");
        printf("5 : View Details\n");
        printf("6 : Exit\n");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            return DEPOSIT;
        case 2:
            return WITHDRAW;
        case 3:
            return BALANCE_ENQUIRY;
        case 4:
            return CHANGE_PASSWORD;
        case 5:
            return VIEW_DETAILS;
        case 6:
            return EXIT;
        default:
            return -1;
        }
    case ADMIN_OPTIONS:
        printf("1 : Add User\n");
        printf("2 : Delete User\n");
        printf("3 : Modify User\n");
        printf("4 : Search for Account Details\n");
        printf("5 : Exit\n");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            return ADD_USER;
        case 2:
            return DEL_USER;
        case 3:
            return MOD_USER;
        case 4:
            return SEARCH_USER;
        case 5:
            return EXIT;
        default:
            return -1;
        }
    default:
        break;
    }
}

void interface(int sock)
{
    int option, rval;
    char *option_string = malloc(10 * sizeof(char));
    option = printMenu(STARTING_OPTIONS);

    switch (option)
    {
    case SIGN_UP:
        rval = authentication(sock, SIGN_UP_OPTIONS);
        while (1)
        {
            switch (rval)
            {
            case SIGN_UP_AS_USER:
                userInterface(sock);
                break;
            case SIGN_UP_AS_JOINT:
                userInterface(sock);
                break;
            case SIGN_UP_AS_ADMIN:
                adminInterface(sock);
                break;
            }
        }
        break;
    case SIGN_IN:
        rval = authentication(sock, SIGN_IN_OPTIONS);
        switch (rval)
        {
        case SIGN_IN_AS_USER:
            while (1)
            {
                userInterface(sock);
            }
            break;
        case SIGN_IN_AS_ADMIN:
            while (1)
            {
                adminInterface(sock);
            }
            break;
        case SIGN_IN_AS_JOINT:
            while (1)
            {
                userInterface(sock);
            }
        }
        break;
    default:
        printf("Invalid Option\n");
        exit(1);
    }
}

int authentication(int sock, int option)
{
    char *username = malloc(BUF_SIZE * sizeof(char));
    char *password = malloc(BUF_SIZE * sizeof(char));
    char *rmsg = malloc(BUF_SIZE * sizeof(char));
    char *option_string = malloc(10 * sizeof(char));

    printf("Entered authentication phase...!\n");
    printf("option = %d\n", option);

    int ret_option = printMenu(option);

    printf("Enter username : ");
    scanf("%s", username);
    printf("Enter password : ");
    scanf("%s", password);
    sprintf(option_string, "%d", ret_option);

    send(sock, option_string, sizeof(option_string), 0);
    send(sock, username, sizeof(username), 0);
    send(sock, password, sizeof(password), 0);
    read(sock, rmsg, BUF_SIZE * sizeof(char));

    printf("%s\n", rmsg);
    if (!strcmp(rmsg, "sign in failed\n"))
        exit(1);

    return ret_option;
}

void userInterface(int sock)
{
    int option = printMenu(USER_OPTIONS);
    int deposit_amt, withdraw_amt;

    char *option_string = malloc(10 * sizeof(char));
    char *rmsg = malloc(BUF_SIZE * sizeof(char));
    char *amt_string = malloc(BUF_SIZE * sizeof(char));
    char *password = malloc(BUF_SIZE * sizeof(char));

    sprintf(option_string, "%d", option);
    send(sock, option_string, sizeof(option_string), 0);

    switch (option)
    {
    case DEPOSIT:
        printf("Enter amount to be deposited : ");
        scanf("%d", &deposit_amt);
        sprintf(amt_string, "%d", deposit_amt);
        send(sock, amt_string, sizeof(amt_string), 0);
        break;

    case WITHDRAW:
        printf("Enter amount to be withdrawn : ");
        scanf("%d", &withdraw_amt);
        sprintf(amt_string, "%d", withdraw_amt);
        send(sock, amt_string, sizeof(amt_string), 0);
        break;

    case CHANGE_PASSWORD:
        printf("Enter new password\n");
        scanf("%s", password);
        send(sock, password, sizeof(password), 0);
        break;

    case EXIT:
        exit(0);

    default:
        break;
    }
    read(sock, rmsg, BUF_SIZE * sizeof(char));
    printf("%s\n", rmsg);
}

void adminInterface(int sock)
{
    int option = printMenu(ADMIN_OPTIONS);
    int type;

    char *option_string = malloc(10 * sizeof(char));
    char *username = malloc(BUF_SIZE * sizeof(char));
    char *new_username = malloc(BUF_SIZE * sizeof(char));
    char *password = malloc(BUF_SIZE * sizeof(char));
    char *rmsg = malloc(BUF_SIZE * sizeof(char));

    sprintf(option_string, "%d", option);
    send(sock, option_string, sizeof(option_string), 0);

    switch (option)
    {
    case ADD_USER:
        printf("Enter User Type\n");
        printf("1 : Normal\n");
        printf("2 : Joint\n");
        scanf("%d", &type);
        printf("Enter username : ");
        scanf("%s", username);
        printf("Enter password : ");
        scanf("%s", password);
        switch (type)
        {
        case 1:
            send(sock, "Normal", sizeof("Normal"), 0);
            break;
        case 2:
            send(sock, "Joint", sizeof("Joint"), 0);
            break;
        default:
            printf("Invalid Type\n");
            exit(1);
        }

        send(sock, username, sizeof(username), 0);
        send(sock, password, sizeof(password), 0);
        break;

    case DEL_USER:
        printf("Enter username : ");
        scanf("%s", username);
        send(sock, username, sizeof(username), 0);
        break;

    case MOD_USER:
        printf("Enter old username : ");
        scanf("%s", username);
        send(sock, username, sizeof(username), 0);

        printf("Enter new username : ");
        scanf("%s", new_username);
        send(sock, new_username, sizeof(new_username), 0);

        printf("Enter new password : ");
        scanf("%s", password);
        send(sock, password, sizeof(password), 0);
        break;

    case SEARCH_USER:
        printf("Enter username : ");
        scanf("%s", username);
        send(sock, username, sizeof(username), 0);
        break;

    case EXIT:
        exit(0);

    default:
        printf("Invalid input\n");
        exit(0);
    }

    read(sock, rmsg, BUF_SIZE * sizeof(char));
    printf("%s\n", rmsg);
}