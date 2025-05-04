#include "deals.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int deals_create_deal(int good_id, int quantity, const char *buyer, int makler_id) {
    // Validate availability
    if (!db_check_good_availability(good_id, quantity)) {
        return -1;
    }
    
    // Get good information
    Good *good = db_get_good_by_id(good_id);
    if (!good) {
        return -1;
    }
    
    // Create deal
    Deal deal = {0};
    deal.deal_date = time(NULL);
    strcpy(deal.good_name, good->name);
    strcpy(deal.good_type, good->type);
    deal.quantity = quantity;
    deal.total_amount = good->unit_price * quantity;
    deal.makler_id = makler_id;
    deal.good_id = good_id;
    strcpy(deal.buyer, buyer);
    
    int result = db_create_deal(&deal);
    db_free_good(good);
    
    return result;
}

Deal** deals_get_makler_deals(int makler_id, int *count) {
    return db_get_deals_by_makler(makler_id, count);
}

Deal** deals_get_all_deals(int *count) {
    return db_get_all_deals(count);
}

double deals_calculate_total(int good_id, int quantity) {
    Good *good = db_get_good_by_id(good_id);
    if (!good) {
        return 0.0;
    }
    
    double total = good->unit_price * quantity;
    db_free_good(good);
    
    return total;
}

int deals_validate_availability(int good_id, int quantity) {
    return db_check_good_availability(good_id, quantity);
}

int deals_validate_expiry(int good_id) {
    Good *good = db_get_good_by_id(good_id);
    if (!good) {
        return 0;
    }
    
    // Parse expiry date and check if it's past
    time_t now = time(NULL);
    struct tm expiry_tm = {0};
    if (strptime(good->expiry_date, "%Y-%m-%d", &expiry_tm) == NULL) {
        db_free_good(good);
        return 0;
    }
    
    time_t expiry_time = mktime(&expiry_tm);
    int result = expiry_time > now;
    
    db_free_good(good);
    return result;
}

void deals_show_stats_by_good(const char *start_date, const char *end_date) {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT good_name, SUM(quantity) as total_quantity, SUM(total_amount) as total_amount "
                "FROM PERFUME_DEALS "
                "WHERE date(deal_date) BETWEEN ? AND ? "
                "GROUP BY good_name;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    sqlite3_bind_text(stmt, 1, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, end_date, -1, SQLITE_STATIC);
    
    printf("\nSales by Good (from %s to %s):\n", start_date, end_date);
    printf("%-30s %-15s %-15s\n", "Good Name", "Total Quantity", "Total Amount");
    printf("----------------------------------------------------------------\n");
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *good_name = (const char *)sqlite3_column_text(stmt, 0);
        int total_quantity = sqlite3_column_int(stmt, 1);
        double total_amount = sqlite3_column_double(stmt, 2);
        
        printf("%-30s %-15d %-15.2f\n", good_name, total_quantity, total_amount);
    }
    
    sqlite3_finalize(stmt);
}

void deals_show_popular_good() {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT good_name, SUM(quantity) as total_quantity "
                "FROM PERFUME_DEALS "
                "GROUP BY good_name "
                "ORDER BY total_quantity DESC "
                "LIMIT 1;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *good_name = (const char *)sqlite3_column_text(stmt, 0);
        int total_quantity = sqlite3_column_int(stmt, 1);
        
        printf("\nMost Popular Good:\n");
        printf("%-30s %-15s\n", "Good Name", "Total Quantity");
        printf("------------------------------------------------\n");
        printf("%-30s %-15d\n", good_name, total_quantity);
    }
    
    sqlite3_finalize(stmt);
}

void deals_show_max_deals_makler() {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT m.name, COUNT(d.id) as deal_count "
                "FROM PERFUME_DEALS d "
                "JOIN PERFUME_MAKLERS m ON d.makler_id = m.id "
                "GROUP BY d.makler_id "
                "ORDER BY deal_count DESC "
                "LIMIT 1;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *makler_name = (const char *)sqlite3_column_text(stmt, 0);
        int deal_count = sqlite3_column_int(stmt, 1);
        
        printf("\nMakler with Most Deals:\n");
        printf("%-30s %-15s\n", "Makler Name", "Deal Count");
        printf("------------------------------------------------\n");
        printf("%-30s %-15d\n", makler_name, deal_count);
    }
    
    sqlite3_finalize(stmt);
}

void deals_show_sales_by_suppliers() {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT g.supplier, COUNT(d.id) as deal_count, SUM(d.quantity) as total_quantity, SUM(d.total_amount) as total_amount "
                "FROM PERFUME_DEALS d "
                "JOIN PERFUME_GOODS g ON d.good_id = g.id "
                "GROUP BY g.supplier "
                "ORDER BY total_amount DESC;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    printf("\nSales by Supplier:\n");
    printf("%-20s %-12s %-15s %-15s\n", "Supplier", "Deal Count", "Total Quantity", "Total Amount");
    printf("-------------------------------------------------------------------\n");
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *supplier = (const char *)sqlite3_column_text(stmt, 0);
        int deal_count = sqlite3_column_int(stmt, 1);
        int total_quantity = sqlite3_column_int(stmt, 2);
        double total_amount = sqlite3_column_double(stmt, 3);
        
        printf("%-20s %-12d %-15d %-15.2f\n", supplier, deal_count, total_quantity, total_amount);
    }
    
    sqlite3_finalize(stmt);
}
