#include "authentication.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int safe_getline(char *buff, int size, FILE *f) {
    if (!fgets(buff, size, f)) return 0;
    size_t len = strlen(buff);
    if (len && buff[len-1]=='\n') buff[len-1]=0;
    return 1;
}

bool register_user(const char *username, const char *password) {
    // Check if user exists
    FILE *f = fopen(USERFILE, "a+");
    if (!f) return false;
    rewind(f);
    char line[256];
    while (safe_getline(line, sizeof(line), f)) {
        char stored_user[MAX_UNAME];
        if (sscanf(line, "%63[^:]:%*s", stored_user) == 1) {
            if (strcmp(stored_user, username) == 0) {
                fclose(f);
                return false; // user exists
            }
        }
    }
    // append
    fprintf(f, "%s:%s\n", username, password); // plain text for prototype
    fclose(f);
    return true;
}

bool login_user(const char *username, const char *password) {
    FILE *f = fopen(USERFILE, "r");
    if (!f) return false;
    char line[256];
    while (safe_getline(line, sizeof(line), f)) {
        char stored_user[MAX_UNAME], stored_pass[MAX_PASS];
        if (sscanf(line, "%63[^:]:%63s", stored_user, stored_pass) == 2) {
            if (strcmp(stored_user, username) == 0 && strcmp(stored_pass, password) == 0) {
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}
