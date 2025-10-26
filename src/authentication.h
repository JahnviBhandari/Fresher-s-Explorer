#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>

#define USERFILE "users.txt"
#define MAX_UNAME 64
#define MAX_PASS 64

bool register_user(const char *username, const char *password);
bool login_user(const char *username, const char *password);

#endif // AUTH_H
