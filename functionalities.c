#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "constants.h"

int signup(int option, char *username, char *password, int amt)
{
    char filename[BUF_SIZE];
    strcpy(filename, username);

    char extension[5] = ".txt";
    int fd;

    strncat(filename, extension, sizeof(extension));

    fd = open(filename, O_WRONLY);
    if (fd != -1)
        return -1; // Account already exists...!

    fd = open(filename, O_WRONLY | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("signup");
        return -1;
    }

    struct user usr;
    strcpy(usr.username, username);
    strcpy(usr.password, password);

    switch (option)
    {
    case SIGN_UP_AS_USER:
        strcpy(usr.type, "normal");
        break;
    case ADD_USER:
        strcpy(usr.type, "normal");
        break;
    case SIGN_UP_AS_JOINT:
        strcpy(usr.type, "joint");
        break;
    case SIGN_UP_AS_ADMIN:
        strcpy(usr.type, "admin");
        break;
    }

    write(fd, &usr, sizeof(struct user));
    struct account acc;
    acc.balance = amt; //generally 0
    write(fd, &acc, sizeof(struct account));
    close(fd);
    return 0;
}

int signin(int option, char *username, char *password)
{
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user); // we are not locking account operations...!
    lock.l_pid = getpid();

    char filename[BUF_SIZE];
    strcpy(filename, username);

    char extension[5] = ".txt";
    int fd;

    strncat(filename, extension, sizeof(extension));

    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("signin");
        return -1;
    }

    struct user u;
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
        return -1;
    }

    // start of critical section
    lseek(fd, 0, SEEK_SET);
    read(fd, &u, sizeof(struct user));
    if ((strcmp(u.password, password) != 0) || (option == SIGN_IN_AS_USER && (strcmp(u.type, "normal") != 0)) || (option == SIGN_IN_AS_ADMIN && (strcmp(u.type, "admin") != 0)) || (option == SIGN_IN_AS_JOINT && (strcmp(u.type, "joint") != 0)))
        return -1;
    // end of critical section
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 0;
}

int transaction(char *username, int amt, int operation)
{
    static struct flock lock;
    if (operation == DEPOSIT || operation == WITHDRAW)
    {
        lock.l_type = F_WRLCK;
    }

    else if (operation == BALANCE_ENQUIRY)
    {
        lock.l_type = F_RDLCK;
    }

    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();

    char filename[BUF_SIZE];
    strcpy(filename, username);

    char extension[5] = ".txt";
    int fd;

    strncat(filename, extension, sizeof(extension));
    fd = open(filename, O_RDWR);

    if (fd == -1)
    {
        perror("signin");
        return -1;
    }

    struct account acc;
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
        return -1;
    }
    // start of critical section
    lseek(fd, sizeof(struct user), SEEK_SET);

    if (read(fd, &acc, sizeof(struct account)) == -1)
    {
        perror("unable to read account balance...!\n");
        return -1;
    }

    printf("balance = %d\n", acc.balance);

    if (operation == DEPOSIT)
    {
        acc.balance += amt;
    }
    else if (operation == WITHDRAW)
    {
        if (acc.balance - amt < 0)
        {
            return -1;
        }
        else
        {
            acc.balance -= amt;
        }
    }

    lseek(fd, sizeof(struct user), SEEK_SET);
    if (write(fd, &acc, sizeof(struct account)) == -1)
    {
        perror("unable to update account balance...!\n");
        return -1;
    }
    // end of critical section

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return acc.balance;
}

int change_password(char *username, char *pwd)
{
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user);
    lock.l_pid = getpid();

    char filename[BUF_SIZE];
    strcpy(filename, username);

    char extension[5] = ".txt";
    int fd;

    strncat(filename, extension, sizeof(extension));

    fd = open(filename, O_RDWR);
    if (fd == -1)
    {
        perror("change pwd");
        return -1;
    }

    struct user u;
    lseek(fd, 0, SEEK_SET);
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
        return -1;
    }
    // start of critical section
    if (read(fd, &u, sizeof(struct user)) == -1)
    {
        perror("read");
        return -1;
    }
    strcpy(u.password, pwd);
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &u, sizeof(struct user)) == -1)
    {
        perror("write");
        return -1;
    }
    // end of critical section
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 0;
}

char *get_details(char *username)
{
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[BUF_SIZE];
    strcpy(filename, username);

    char extension[5] = ".txt";
    int fd;

    strncat(filename, extension, sizeof(extension));
    fd = open(filename, O_RDWR);

    if (fd == -1)
    {
        perror("open");
        return "user does not exist\n";
    }
    struct account acc;
    struct user u;

    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
        return "sorry, section is locked\n";
    }
    // start of critical section
    lseek(fd, 0, SEEK_SET);
    if (read(fd, &u, sizeof(struct user)) == -1)
    {
        perror("read");
        return "unable to read file\n";
    }
    if (read(fd, &acc, sizeof(struct account)) == -1)
    {
        perror("read");
        return "unable to read file\n";
    }
    // end of critical section
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);

    char *return_string = (char *)malloc(BUF_SIZE * sizeof(char));
    sprintf(return_string, "username : %s \npassword : %s \ntype : %s\nbalance : %d\n",
            u.username, u.password, u.type, acc.balance);

    return return_string;
}

int del_user(char *username)
{
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[BUF_SIZE];
    strcpy(filename, username);
    char extension[5] = ".txt";
    strncat(filename, extension, sizeof(extension));
    int fd = open(filename, O_RDWR);
    if (fd == -1)
    {
        perror("open");
    }
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
    }

    return unlink(filename);
}

int modify_user(char *username, char *new_username, char *password)
{
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[BUF_SIZE];
    strcpy(filename, username);
    char extension[5] = ".txt";
    int fd, option;
    strncat(filename, extension, sizeof(extension));
    fd = open(filename, O_RDWR);

    if (fd == -1)
    {
        perror("mod user");
        return -1;
    }

    struct user u;
    struct account acc;
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
    }
    // start of critical section
    lseek(fd, 0, SEEK_SET);
    if (read(fd, &u, sizeof(struct user)) == -1)
    {
        perror("unable to read user details...!");
        return -1;
    }

    if (read(fd, &acc, sizeof(struct account)) == -1)
    {
        perror("unable to read user's account details...!");
        return -1;
    }

    del_user(username);
    if (strcmp(u.type, "normal") == 0)
        option = SIGN_UP_AS_USER;
    else
        option = SIGN_UP_AS_JOINT;
    strcpy(u.username, new_username);
    signup(option, new_username, password, acc.balance);
    // end of critical section
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 0;
}