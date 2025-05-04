#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "types.h"

// Database initialization
int db_init(const char *db_path);
void db_close();
sqlite3* db_get_connection();

// User operations
int db_create_user(const User *user);
User* db_get_user_by_username(const char *username);
int db_update_user(const User *user);
int db_delete_user(int user_id);

// Makler operations
int db_create_makler(const Makler *makler);
Makler* db_get_makler_by_id(int id);
Makler* db_get_makler_by_user_id(int user_id);
Makler** db_get_all_maklers(int *count);
int db_update_makler(const Makler *makler);
int db_delete_makler(int id);

// Good operations
int db_create_good(const Good *good);
Good* db_get_good_by_id(int id);
Good** db_get_all_goods(int *count);
int db_update_good(const Good *good);
int db_delete_good(int id);
int db_check_good_availability(int good_id, int quantity_needed);

// Deal operations
int db_create_deal(const Deal *deal);
Deal** db_get_deals_by_makler(int makler_id, int *count);
Deal** db_get_all_deals(int *count);
Deal** db_get_deals_by_date_range(time_t start_date, time_t end_date, int *count);
int db_update_stats_on_deal(const Deal *deal);

// Makler statistics operations
MaklerStats* db_get_makler_stats(int makler_id, int *count);
int db_update_makler_stats(const Deal *deal);

// Helper functions
void db_free_user(User *user);
void db_free_makler(Makler *makler);
void db_free_good(Good *good);
void db_free_deal(Deal *deal);
void db_free_makler_stats(MaklerStats *stats);

#endif // DATABASE_H
