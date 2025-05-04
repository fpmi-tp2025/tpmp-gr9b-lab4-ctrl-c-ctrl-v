#ifndef TYPES_H
#define TYPES_H

#include <time.h>

// User roles
typedef enum {
    ROLE_ADMIN,
    ROLE_MAKLER
} UserRole;

// User structure
typedef struct {
    int id;
    char username[50];
    char password_hash[256];
    UserRole role;
    time_t created_at;
} User;

// Makler structure
typedef struct {
    int id;
    char name[100];
    char address[200];
    int birth_year;
    int user_id;
} Makler;

// Good structure
typedef struct {
    int id;
    char name[100];
    char type[50];
    double unit_price;
    char supplier[100];
    char expiry_date[11]; // YYYY-MM-DD
    int quantity;
    time_t created_at;
} Good;

// Deal structure
typedef struct {
    int id;
    time_t deal_date;
    char good_name[100];
    char good_type[50];
    int quantity;
    double total_amount;
    int makler_id;
    int good_id;
    char buyer[100];
    time_t created_at;
} Deal;

// Makler statistics structure
typedef struct {
    int id;
    int makler_id;
    char good_name[100];
    char good_type[50];
    int total_quantity;
    double total_amount;
    time_t updated_at;
} MaklerStats;

#endif // TYPES_H
