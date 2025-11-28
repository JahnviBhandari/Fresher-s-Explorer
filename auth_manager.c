#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfiles/authentication.h"  // Updated path

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <register|login> <username> <password>\n", argv[0]);
        return 1;
    }
    
    char *command = argv[1];
    char *username = argv[2];
    char *password = argv[3];
    
    if (strcmp(command, "register") == 0) {
        if (register_user(username, password)) {
            printf("Registration successful\n");
            return 0;
        } else {
            printf("Registration failed: User may already exist\n");
            return 1;
        }
    } else if (strcmp(command, "login") == 0) {
        if (login_user(username, password)) {
            printf("Login successful\n");
            return 0;
        } else {
            printf("Login failed: Invalid credentials\n");
            return 1;
        }
    } else {
        printf("Invalid command\n");
        return 1;
    }
}