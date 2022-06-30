#define PORT 5000

// possible menus
#define STARTING_OPTIONS 0
#define SIGN_UP_OPTIONS 1
#define SIGN_IN_OPTIONS 2
#define USER_OPTIONS 3
#define ADMIN_OPTIONS 4

// possible options
#define SIGN_UP 5
#define SIGN_IN 6
#define SIGN_UP_AS_USER 7
#define SIGN_UP_AS_JOINT 8
#define SIGN_UP_AS_ADMIN 9
#define SIGN_IN_AS_USER 10
#define SIGN_IN_AS_JOINT 11
#define SIGN_IN_AS_ADMIN 12
#define DEPOSIT 13
#define WITHDRAW 14
#define BALANCE_ENQUIRY 15
#define CHANGE_PASSWORD 16
#define VIEW_DETAILS 17
#define EXIT 18
#define ADD_USER 19
#define DEL_USER 20
#define MOD_USER 21
#define SEARCH_USER 22
#define INVALID -1
#define BUF_SIZE 1000

//struct user
struct user
{
    char type[BUF_SIZE];
    char password[BUF_SIZE];
    char username[BUF_SIZE];
};

//struct details
struct account
{
    int balance;
};