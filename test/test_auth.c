#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
#include "database.h"

void test_password_hashing() {
    printf("Testing password hashing...\n");
    
    char *hash1 = auth_hash_password("test123");
    char *hash2 = auth_hash_password("test123");
    char *hash3 = auth_hash_password("different");
    
    assert(strcmp(hash1, hash2) == 0);
    assert(strcmp(hash1, hash3) != 0);
    
    free(hash1);
    free(hash2);
    free(hash3);
    
    printf("✓ Password hashing passed\n");
}

void test_password_verification() {
    printf("Testing password verification...\n");
    
    char *hash = auth_hash_password("mypassword");
    
    assert(auth_verify_password("mypassword", hash) == 1);
    assert(auth_verify_password("wrongpassword", hash) == 0);
    
    free(hash);
    
    printf("✓ Password verification passed\n");
}

void test_login_logout() {
    printf("Testing login/logout...\n");
    
    // Initialize database and create test user
    db_init("test_auth.db");
    
    User user = {0};
    strcpy(user.username, "testuser");
    strcpy(user.password_hash, "password123");
    user.role = ROLE_ADMIN;
    
    db_create_user(&user);
    
    // Test login with correct password
    User *logged_in = auth_login("testuser", "password123");
    assert(logged_in != NULL);
    assert(auth_is_logged_in() == 1);
    assert(auth_get_current_user() == logged_in);
    
    // Test login with wrong password
    auth_logout(logged_in);
    assert(auth_is_logged_in() == 0);
    assert(auth_get_current_user() == NULL);
    
    User *not_logged_in = auth_login("testuser", "wrongpassword");
    assert(not_logged_in == NULL);
    assert(auth_is_logged_in() == 0);
    
    db_close();
    remove("test_auth.db");
    
    printf("✓ Login/logout passed\n");
}

void test_permissions() {
    printf("Testing permission checks...\n");
    
    db_init("test_auth.db");
    
    // Create admin and makler users
    User admin_user = {0};
    strcpy(admin_user.username, "admin");
    strcpy(admin_user.password_hash, "adminpass");
    admin_user.role = ROLE_ADMIN;
    db_create_user(&admin_user);
    
    User makler_user = {0};
    strcpy(makler_user.username, "makler");
    strcpy(makler_user.password_hash, "maklerpass");
    makler_user.role = ROLE_MAKLER;
    db_create_user(&makler_user);
    
    // Test admin permissions
    User *admin = auth_login("admin", "adminpass");
    assert(auth_check_permission(admin, ROLE_ADMIN) == 1);
    assert(auth_check_permission(admin, ROLE_MAKLER) == 1); // Admin has all permissions
    auth_logout(admin);
    
    // Test makler permissions
    User *makler = auth_login("makler", "maklerpass");
    assert(auth_check_permission(makler, ROLE_MAKLER) == 1);
    assert(auth_check_permission(makler, ROLE_ADMIN) == 0);
    auth_logout(makler);
    
    db_close();
    remove("test_auth.db");
    
    printf("✓ Permission checks passed\n");
}

void test_session_management() {
    printf("Testing session management...\n");
    
    db_init("test_auth.db");
    
    User user = {0};
    strcpy(user.username, "session_user");
    strcpy(user.password_hash, "session123");
    user.role = ROLE_MAKLER;
    db_create_user(&user);
    
    // Test session start/end
    User *logged_in = auth_login("session_user", "session123");
    assert(logged_in != NULL);
    assert(auth_get_current_user() == logged_in);
    
    auth_end_session();
    assert(auth_get_current_user() == NULL);
    assert(auth_is_logged_in() == 0);
    
    db_free_user(logged_in);
    db_close();
    remove("test_auth.db");
    
    printf("✓ Session management passed\n");
}

int main() {
    printf("Starting authentication tests...\n\n");
    
    test_password_hashing();
    test_password_verification();
    test_login_logout();
    test_permissions();
    test_session_management();
    
    printf("\n✅ All authentication tests passed!\n");
    return 0;
}
