#include "auth.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static User *current_user = NULL;

// Простой "хэш" - просто копируем пароль
char* auth_hash_password(const char *password) {
    char *hash = malloc(strlen(password) + 1);
    strcpy(hash, password);
    return hash;
}

int auth_verify_password(const char *password, const char *hash) {
    return strcmp(password, hash) == 0;
}

User* auth_login(const char *username, const char *password) {
    User *user = db_get_user_by_username(username);
    if (!user) {
        return NULL;
    }
    
    // Простое сравнение паролей без хэшей
    if (strcmp(password, user->password_hash) != 0) {
        db_free_user(user);
        return NULL;
    }
    
    auth_start_session(user);
    return user;
}

void auth_logout(User *user) {
    auth_end_session();
    if (user) {
        db_free_user(user);
    }
}

int auth_check_permission(const User *user, UserRole required_role) {
    if (!user) return 0;
    if (user->role == ROLE_ADMIN) return 1; // Admin has all permissions
    return user->role == required_role;
}

void auth_start_session(User *user) {
    current_user = user;
}

void auth_end_session() {
    current_user = NULL;
}

User* auth_get_current_user() {
    return current_user;
}

int auth_is_logged_in() {
    return current_user != NULL;
}
