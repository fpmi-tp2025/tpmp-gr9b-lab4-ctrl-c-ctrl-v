#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sqlite3 *db = NULL;

int db_init(const char *db_path) {
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    // Create tables if they don't exist
    const char *sql[] = {
        "CREATE TABLE IF NOT EXISTS PERFUME_USERS ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    username VARCHAR(50) NOT NULL UNIQUE,"
        "    password_hash VARCHAR(255) NOT NULL,"
        "    role VARCHAR(20) NOT NULL,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");",
        
        "CREATE TABLE IF NOT EXISTS PERFUME_MAKLERS ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name VARCHAR(100) NOT NULL,"
        "    address VARCHAR(200),"
        "    birth_year INTEGER,"
        "    user_id INTEGER,"
        "    FOREIGN KEY (user_id) REFERENCES PERFUME_USERS(id)"
        ");",
        
        "CREATE TABLE IF NOT EXISTS PERFUME_GOODS ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name VARCHAR(100) NOT NULL,"
        "    type VARCHAR(50) NOT NULL,"
        "    unit_price DECIMAL(10,2) NOT NULL,"
        "    supplier VARCHAR(100),"
        "    expiry_date DATE,"
        "    quantity INTEGER NOT NULL DEFAULT 0,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");",
        
        "CREATE TABLE IF NOT EXISTS PERFUME_DEALS ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    deal_date DATETIME NOT NULL,"
        "    good_name VARCHAR(100) NOT NULL,"
        "    good_type VARCHAR(50) NOT NULL,"
        "    quantity INTEGER NOT NULL,"
        "    total_amount DECIMAL(12,2) NOT NULL,"
        "    makler_id INTEGER NOT NULL,"
        "    good_id INTEGER NOT NULL,"
        "    buyer VARCHAR(100) NOT NULL,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY (makler_id) REFERENCES PERFUME_MAKLERS(id),"
        "    FOREIGN KEY (good_id) REFERENCES PERFUME_GOODS(id)"
        ");",
        
        "CREATE TABLE IF NOT EXISTS PERFUME_MAKLERSTATS ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    makler_id INTEGER NOT NULL,"
        "    good_name VARCHAR(100) NOT NULL,"
        "    good_type VARCHAR(50) NOT NULL,"
        "    total_quantity INTEGER NOT NULL DEFAULT 0,"
        "    total_amount DECIMAL(12,2) NOT NULL DEFAULT 0,"
        "    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY (makler_id) REFERENCES PERFUME_MAKLERS(id),"
        "    UNIQUE(makler_id, good_name, good_type)"
        ");"
    };
    
    char *err_msg = 0;
    for (int i = 0; i < 5; i++) {
        rc = sqlite3_exec(db, sql[i], 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
            return -1;
        }
    }
    
    return 0;
}

void db_close() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

sqlite3* db_get_connection() {
    return db;
}

int db_create_user(const User *user) {
    char *sql = "INSERT INTO PERFUME_USERS (username, password_hash, role) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, user->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user->password_hash, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user->role == ROLE_ADMIN ? "admin" : "makler", -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    int user_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return user_id;
}

User* db_get_user_by_username(const char *username) {
    char *sql = "SELECT id, username, password_hash, role, created_at FROM PERFUME_USERS WHERE username = ?;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    User *user = (User *)malloc(sizeof(User));
    if (!user) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    user->id = sqlite3_column_int(stmt, 0);
    strcpy(user->username, (const char *)sqlite3_column_text(stmt, 1));
    strcpy(user->password_hash, (const char *)sqlite3_column_text(stmt, 2));
    const char *role_str = (const char *)sqlite3_column_text(stmt, 3);
    user->role = strcmp(role_str, "admin") == 0 ? ROLE_ADMIN : ROLE_MAKLER;
    user->created_at = (time_t)sqlite3_column_int64(stmt, 4);
    
    sqlite3_finalize(stmt);
    return user;
}

int db_create_makler(const Makler *makler) {
    char *sql = "INSERT INTO PERFUME_MAKLERS (name, address, birth_year, user_id) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, makler->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, makler->address, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, makler->birth_year);
    sqlite3_bind_int(stmt, 4, makler->user_id);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    int makler_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return makler_id;
}

Makler* db_get_makler_by_user_id(int user_id) {
    char *sql = "SELECT id, name, address, birth_year, user_id FROM PERFUME_MAKLERS WHERE user_id = ?;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    Makler *makler = (Makler *)malloc(sizeof(Makler));
    if (!makler) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    makler->id = sqlite3_column_int(stmt, 0);
    strcpy(makler->name, (const char *)sqlite3_column_text(stmt, 1));
    strcpy(makler->address, (const char *)sqlite3_column_text(stmt, 2));
    makler->birth_year = sqlite3_column_int(stmt, 3);
    makler->user_id = sqlite3_column_int(stmt, 4);
    
    sqlite3_finalize(stmt);
    return makler;
}

int db_create_good(const Good *good) {
    char *sql = "INSERT INTO PERFUME_GOODS (name, type, unit_price, supplier, expiry_date, quantity) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, good->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, good->type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, good->unit_price);
    sqlite3_bind_text(stmt, 4, good->supplier, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, good->expiry_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, good->quantity);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    int good_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return good_id;
}

Good* db_get_good_by_id(int id) {
    char *sql = "SELECT id, name, type, unit_price, supplier, expiry_date, quantity, created_at FROM PERFUME_GOODS WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    Good *good = (Good *)malloc(sizeof(Good));
    if (!good) {
        sqlite3_finalize(stmt);
        return NULL;
    }
    
    good->id = sqlite3_column_int(stmt, 0);
    strcpy(good->name, (const char *)sqlite3_column_text(stmt, 1));
    strcpy(good->type, (const char *)sqlite3_column_text(stmt, 2));
    good->unit_price = sqlite3_column_double(stmt, 3);
    strcpy(good->supplier, (const char *)sqlite3_column_text(stmt, 4));
    strcpy(good->expiry_date, (const char *)sqlite3_column_text(stmt, 5));
    good->quantity = sqlite3_column_int(stmt, 6);
    good->created_at = (time_t)sqlite3_column_int64(stmt, 7);
    
    sqlite3_finalize(stmt);
    return good;
}

Good** db_get_all_goods(int *count) {
    char *sql = "SELECT id, name, type, unit_price, supplier, expiry_date, quantity, created_at FROM PERFUME_GOODS;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        *count = 0;
        return NULL;
    }
    
    *count = 0;
    Good **goods = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        goods = (Good **)realloc(goods, sizeof(Good *) * (*count + 1));
        if (!goods) {
            *count = 0;
            sqlite3_finalize(stmt);
            return NULL;
        }
        
        goods[*count] = (Good *)malloc(sizeof(Good));
        if (!goods[*count]) {
            sqlite3_finalize(stmt);
            return goods;
        }
        
        goods[*count]->id = sqlite3_column_int(stmt, 0);
        strcpy(goods[*count]->name, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(goods[*count]->type, (const char *)sqlite3_column_text(stmt, 2));
        goods[*count]->unit_price = sqlite3_column_double(stmt, 3);
        strcpy(goods[*count]->supplier, (const char *)sqlite3_column_text(stmt, 4));
        strcpy(goods[*count]->expiry_date, (const char *)sqlite3_column_text(stmt, 5));
        goods[*count]->quantity = sqlite3_column_int(stmt, 6);
        goods[*count]->created_at = (time_t)sqlite3_column_int64(stmt, 7);
        
        (*count)++;
    }
    
    sqlite3_finalize(stmt);
    return goods;
}

int db_check_good_availability(int good_id, int quantity_needed) {
    char *sql = "SELECT quantity FROM PERFUME_GOODS WHERE id = ?;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    sqlite3_bind_int(stmt, 1, good_id);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 0;
    }
    
    int available = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    
    return available >= quantity_needed;
}

int db_create_deal(const Deal *deal) {
    char *sql = "INSERT INTO PERFUME_DEALS (deal_date, good_name, good_type, quantity, total_amount, makler_id, good_id, buyer) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    
    sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0);
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, ctime(&deal->deal_date), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, deal->good_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, deal->good_type, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, deal->quantity);
    sqlite3_bind_double(stmt, 5, deal->total_amount);
    sqlite3_bind_int(stmt, 6, deal->makler_id);
    sqlite3_bind_int(stmt, 7, deal->good_id);
    sqlite3_bind_text(stmt, 8, deal->buyer, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        return -1;
    }
    
    int deal_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    // Update goods quantity
    sql = "UPDATE PERFUME_GOODS SET quantity = quantity - ? WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, deal->quantity);
    sqlite3_bind_int(stmt, 2, deal->good_id);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    
    // Update makler stats
    db_update_makler_stats(deal);
    
    sqlite3_exec(db, "COMMIT", 0, 0, 0);
    return deal_id;
}

Deal** db_get_deals_by_makler(int makler_id, int *count) {
    char *sql = "SELECT id, deal_date, good_name, good_type, quantity, total_amount, makler_id, good_id, buyer, created_at FROM PERFUME_DEALS WHERE makler_id = ?;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        *count = 0;
        return NULL;
    }
    
    sqlite3_bind_int(stmt, 1, makler_id);
    
    *count = 0;
    Deal **deals = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        deals = (Deal **)realloc(deals, sizeof(Deal *) * (*count + 1));
        if (!deals) {
            *count = 0;
            sqlite3_finalize(stmt);
            return NULL;
        }
        
        deals[*count] = (Deal *)malloc(sizeof(Deal));
        if (!deals[*count]) {
            sqlite3_finalize(stmt);
            return deals;
        }
        
        deals[*count]->id = sqlite3_column_int(stmt, 0);
        // Parse date string to time_t (simplified)
        deals[*count]->deal_date = time(NULL); // Simplified for now
        strcpy(deals[*count]->good_name, (const char *)sqlite3_column_text(stmt, 2));
        strcpy(deals[*count]->good_type, (const char *)sqlite3_column_text(stmt, 3));
        deals[*count]->quantity = sqlite3_column_int(stmt, 4);
        deals[*count]->total_amount = sqlite3_column_double(stmt, 5);
        deals[*count]->makler_id = sqlite3_column_int(stmt, 6);
        deals[*count]->good_id = sqlite3_column_int(stmt, 7);
        strcpy(deals[*count]->buyer, (const char *)sqlite3_column_text(stmt, 8));
        deals[*count]->created_at = (time_t)sqlite3_column_int64(stmt, 9);
        
        (*count)++;
    }
    
    sqlite3_finalize(stmt);
    return deals;
}

Deal** db_get_all_deals(int *count) {
    char *sql = "SELECT id, deal_date, good_name, good_type, quantity, total_amount, makler_id, good_id, buyer, created_at FROM PERFUME_DEALS;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        *count = 0;
        return NULL;
    }
    
    *count = 0;
    Deal **deals = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        deals = (Deal **)realloc(deals, sizeof(Deal *) * (*count + 1));
        if (!deals) {
            *count = 0;
            sqlite3_finalize(stmt);
            return NULL;
        }
        
        deals[*count] = (Deal *)malloc(sizeof(Deal));
        if (!deals[*count]) {
            sqlite3_finalize(stmt);
            return deals;
        }
        
        deals[*count]->id = sqlite3_column_int(stmt, 0);
        // Parse date string to time_t (simplified)
        deals[*count]->deal_date = time(NULL); // Simplified for now
        strcpy(deals[*count]->good_name, (const char *)sqlite3_column_text(stmt, 2));
        strcpy(deals[*count]->good_type, (const char *)sqlite3_column_text(stmt, 3));
        deals[*count]->quantity = sqlite3_column_int(stmt, 4);
        deals[*count]->total_amount = sqlite3_column_double(stmt, 5);
        deals[*count]->makler_id = sqlite3_column_int(stmt, 6);
        deals[*count]->good_id = sqlite3_column_int(stmt, 7);
        strcpy(deals[*count]->buyer, (const char *)sqlite3_column_text(stmt, 8));
        deals[*count]->created_at = (time_t)sqlite3_column_int64(stmt, 9);
        
        (*count)++;
    }
    
    sqlite3_finalize(stmt);
    return deals;
}

int db_update_makler_stats(const Deal *deal) {
    char *sql = "INSERT INTO PERFUME_MAKLERSTATS (makler_id, good_name, good_type, total_quantity, total_amount) "
                "VALUES (?, ?, ?, ?, ?) "
                "ON CONFLICT(makler_id, good_name, good_type) DO UPDATE SET "
                "total_quantity = total_quantity + excluded.total_quantity, "
                "total_amount = total_amount + excluded.total_amount, "
                "updated_at = CURRENT_TIMESTAMP;";
    
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, deal->makler_id);
    sqlite3_bind_text(stmt, 2, deal->good_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, deal->good_type, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, deal->quantity);
    sqlite3_bind_double(stmt, 5, deal->total_amount);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

MaklerStats* db_get_makler_stats(int makler_id, int *count) {
    char *sql = "SELECT id, makler_id, good_name, good_type, total_quantity, total_amount, updated_at FROM PERFUME_MAKLERSTATS WHERE makler_id = ?;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        *count = 0;
        return NULL;
    }
    
    sqlite3_bind_int(stmt, 1, makler_id);
    
    *count = 0;
    MaklerStats *stats = NULL;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        stats = (MaklerStats *)realloc(stats, sizeof(MaklerStats) * (*count + 1));
        if (!stats) {
            *count = 0;
            sqlite3_finalize(stmt);
            return NULL;
        }
        
        stats[*count].id = sqlite3_column_int(stmt, 0);
        stats[*count].makler_id = sqlite3_column_int(stmt, 1);
        strcpy(stats[*count].good_name, (const char *)sqlite3_column_text(stmt, 2));
        strcpy(stats[*count].good_type, (const char *)sqlite3_column_text(stmt, 3));
        stats[*count].total_quantity = sqlite3_column_int(stmt, 4);
        stats[*count].total_amount = sqlite3_column_double(stmt, 5);
        stats[*count].updated_at = (time_t)sqlite3_column_int64(stmt, 6);
        
        (*count)++;
    }
    
    sqlite3_finalize(stmt);
    return stats;
}

void db_free_user(User *user) {
    if (user) free(user);
}

void db_free_makler(Makler *makler) {
    if (makler) free(makler);
}

void db_free_good(Good *good) {
    if (good) free(good);
}

void db_free_deal(Deal *deal) {
    if (deal) free(deal);
}
