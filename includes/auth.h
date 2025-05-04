#ifndef AUTH_H
#define AUTH_H

#include "types.h"

// Authentication functions
User* auth_login(const char *username, const char *password);
void auth_logout(User *current_user);
int auth_check_permission(const User *user, UserRole required_role);
char* auth_hash_password(const char *password);
int auth_verify_password(const char *password, const char *hash);

// Session management
void auth_start_session(User *user);
void auth_end_session();
User* auth_get_current_user();
int auth_is_logged_in();

#endif // AUTH_H
