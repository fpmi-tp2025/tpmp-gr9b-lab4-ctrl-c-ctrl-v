#include "reports.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reports_sales_by_good(const char *start_date, const char *end_date) {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT good_name, good_type, SUM(quantity) as total_quantity, SUM(total_amount) as total_amount "
                "FROM PERFUME_DEALS "
                "WHERE date(deal_date) BETWEEN ? AND ? "
                "GROUP BY good_name, good_type;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    sqlite3_bind_text(stmt, 1, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, end_date, -1, SQLITE_STATIC);
    
    printf("\nSales Report by Good (from %s to %s):\n", start_date, end_date);
    printf("%-30s %-20s %-15s %-15s\n", "Good Name", "Type", "Total Quantity", "Total Amount");
    printf("--------------------------------------------------------------------------------\n");
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *good_name = (const char *)sqlite3_column_text(stmt, 0);
        const char *good_type = (const char *)sqlite3_column_text(stmt, 1);
        int total_quantity = sqlite3_column_int(stmt, 2);
        double total_amount = sqlite3_column_double(stmt, 3);
        
        printf("%-30s %-20s %-15d %-15.2f\n", good_name, good_type, total_quantity, total_amount);
    }
    
    sqlite3_finalize(stmt);
}

void reports_buyers_by_good(const char *good_name) {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT buyer, COUNT(*) as deal_count, SUM(quantity) as total_quantity, SUM(total_amount) as total_amount "
                "FROM PERFUME_DEALS "
                "WHERE good_name = ? "
                "GROUP BY buyer;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    sqlite3_bind_text(stmt, 1, good_name, -1, SQLITE_STATIC);
    
    printf("\nBuyers for '%s':\n", good_name);
    printf("%-30s %-12s %-15s %-15s\n", "Buyer", "Deal Count", "Total Quantity", "Total Amount");
    printf("--------------------------------------------------------------------------------\n");
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *buyer = (const char *)sqlite3_column_text(stmt, 0);
        int deal_count = sqlite3_column_int(stmt, 1);
        int total_quantity = sqlite3_column_int(stmt, 2);
        double total_amount = sqlite3_column_double(stmt, 3);
        
        printf("%-30s %-12d %-15d %-15.2f\n", buyer, deal_count, total_quantity, total_amount);
    }
    
    sqlite3_finalize(stmt);
}

void reports_popular_good_type() {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    const char *sql = "SELECT good_type, SUM(quantity) as total_quantity, SUM(total_amount) as total_amount "
                      "FROM PERFUME_DEALS "
                      "GROUP BY good_type "
                      "ORDER BY total_quantity DESC "
                      "LIMIT 1;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *good_type = (const char *)sqlite3_column_text(stmt, 0);
        int total_quantity = sqlite3_column_int(stmt, 1);
        double total_amount = sqlite3_column_double(stmt, 2);
        
        printf("\nMost Popular Good Type:\n");
        printf("%-20s %-15s %-15s\n", "Type", "Total Quantity", "Total Amount");
        printf("-----------------------------------------------------------\n");
        printf("%-20s %-15d %-15.2f\n", good_type, total_quantity, total_amount);
        
        // Show buyers by firm for type
        const char *buyers_sql = "SELECT buyer, COUNT(*) as deal_count, SUM(quantity) as total_quantity, SUM(total_amount) as total_amount "
                                 "FROM PERFUME_DEALS "
                                 "WHERE good_type = ? "
                                 "GROUP BY buyer;";
        
        sqlite3_finalize(stmt);
        rc = sqlite3_prepare_v2(db, buyers_sql, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            return;
        }
        
        sqlite3_bind_text(stmt, 1, good_type, -1, SQLITE_STATIC);
        
        printf("\nBuyers by Firm for Type '%s':\n", good_type);
        printf("%-30s %-12s %-15s %-15s\n", "Buyer", "Deal Count", "Total Quantity", "Total Amount");
        printf("--------------------------------------------------------------------------------\n");
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *buyer = (const char *)sqlite3_column_text(stmt, 0);
            int deal_count = sqlite3_column_int(stmt, 1);
            int total_quantity = sqlite3_column_int(stmt, 2);
            double total_amount = sqlite3_column_double(stmt, 3);
            
            printf("%-30s %-12d %-15d %-15.2f\n", buyer, deal_count, total_quantity, total_amount);
        }
    }
    
    sqlite3_finalize(stmt);
}

void reports_max_deals_makler() {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    const char *sql = "SELECT m.name, COUNT(d.id) as deal_count "
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
        
        printf("\nMakler with Maximum Deals:\n");
        printf("%-30s %-15s\n", "Makler Name", "Deal Count");
        printf("------------------------------------------------\n");
        printf("%-30s %-15d\n", makler_name, deal_count);
        
        printf("\nSuppliers for '%s':\n", makler_name);
        
        const char *suppliers_sql = "SELECT DISTINCT g.supplier "
                                    "FROM PERFUME_DEALS d "
                                    "JOIN PERFUME_GOODS g ON d.good_id = g.id "
                                    "JOIN PERFUME_MAKLERS m ON d.makler_id = m.id "
                                    "WHERE m.name = ?;";
        
        sqlite3_finalize(stmt);
        rc = sqlite3_prepare_v2(db, suppliers_sql, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            return;
        }
        
        sqlite3_bind_text(stmt, 1, makler_name, -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *supplier = (const char *)sqlite3_column_text(stmt, 0);
            printf("- %s\n", supplier);
        }
    }
    
    sqlite3_finalize(stmt);
}

void reports_sales_by_supplier() {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT g.supplier, COUNT(d.id) as deal_count, SUM(d.quantity) as total_quantity, SUM(d.total_amount) as total_amount "
                "FROM PERFUME_DEALS d "
                "JOIN PERFUME_GOODS g ON d.good_id = g.id "
                "GROUP BY g.supplier;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    printf("\nSales by Supplier:\n");
    printf("%-30s %-12s %-15s %-15s\n", "Supplier", "Deal Count", "Total Quantity", "Total Amount");
    printf("--------------------------------------------------------------------------------\n");
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *supplier = (const char *)sqlite3_column_text(stmt, 0);
        int deal_count = sqlite3_column_int(stmt, 1);
        int total_quantity = sqlite3_column_int(stmt, 2);
        double total_amount = sqlite3_column_double(stmt, 3);
        
        printf("%-30s %-12d %-15d %-15.2f\n", supplier, deal_count, total_quantity, total_amount);
        
        // For each supplier, show their maklers
        char subsql[] = "SELECT DISTINCT m.name "
                       "FROM PERFUME_DEALS d "
                       "JOIN PERFUME_MAKLERS m ON d.makler_id = m.id "
                       "JOIN PERFUME_GOODS g ON d.good_id = g.id "
                       "WHERE g.supplier = ?;";
        
        sqlite3_stmt *substmt;
        rc = sqlite3_prepare_v2(db, subsql, -1, &substmt, 0);
        if (rc != SQLITE_OK) {
            continue;
        }
        
        sqlite3_bind_text(substmt, 1, supplier, -1, SQLITE_STATIC);
        
        printf("  Maklers: ");
        int first = 1;
        while (sqlite3_step(substmt) == SQLITE_ROW) {
            const char *makler_name = (const char *)sqlite3_column_text(substmt, 0);
            if (!first) printf(", ");
            printf("%s", makler_name);
            first = 0;
        }
        printf("\n");
        
        sqlite3_finalize(substmt);
    }
    
    sqlite3_finalize(stmt);
}

void reports_makler_deals(int makler_id, const char *date) {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT d.id, d.deal_date, d.good_name, d.good_type, d.quantity, d.total_amount, d.buyer "
                "FROM PERFUME_DEALS d "
                "WHERE d.makler_id = ? AND date(d.deal_date) = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    sqlite3_bind_int(stmt, 1, makler_id);
    sqlite3_bind_text(stmt, 2, date, -1, SQLITE_STATIC);
    
    printf("\nDeals for Makler ID %d on %s:\n", makler_id, date);
    printf("%-5s %-20s %-30s %-20s %-10s %-15s %-30s\n", "ID", "Date", "Good Name", "Type", "Quantity", "Amount", "Buyer");
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    
    int found = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = 1;
        int id = sqlite3_column_int(stmt, 0);
        const char *deal_date = (const char *)sqlite3_column_text(stmt, 1);
        const char *good_name = (const char *)sqlite3_column_text(stmt, 2);
        const char *good_type = (const char *)sqlite3_column_text(stmt, 3);
        int quantity = sqlite3_column_int(stmt, 4);
        double total_amount = sqlite3_column_double(stmt, 5);
        const char *buyer = (const char *)sqlite3_column_text(stmt, 6);
        
        printf("%-5d %-20s %-30s %-20s %-10d %-15.2f %-30s\n", 
               id, deal_date, good_name, good_type, quantity, total_amount, buyer);
    }
    
    if (!found) {
        printf("No deals found for this date.\n");
    }
    
    sqlite3_finalize(stmt);
}

void reports_update_stock(const char *date) {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0);
    
    // Update goods quantities based on deals
    char sql[] = "UPDATE PERFUME_GOODS "
                "SET quantity = quantity - ("
                "   SELECT COALESCE(SUM(d.quantity), 0) "
                "   FROM PERFUME_DEALS d "
                "   WHERE d.good_id = PERFUME_GOODS.id "
                "   AND date(d.deal_date) <= ?"
                ");";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        return;
    }
    
    sqlite3_bind_text(stmt, 1, date, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        return;
    }
    
    sqlite3_finalize(stmt);
    
    // Delete deals up to the specified date
    const char *delete_sql = "DELETE FROM PERFUME_DEALS WHERE date(deal_date) <= ?;";
    
    rc = sqlite3_prepare_v2(db, delete_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        return;
    }
    
    sqlite3_bind_text(stmt, 1, date, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", 0, 0, 0);
        return;
    }
    
    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT", 0, 0, 0);
    
    printf("\nStock updated successfully for date: %s\n", date);
}

int stats_update_on_deal(const Deal *deal) {
    return db_update_makler_stats(deal);
}

void stats_show_makler_stats(int makler_id) {
    int count;
    MaklerStats *stats = db_get_makler_stats(makler_id, &count);
    
    printf("\nStatistics for Makler ID %d:\n", makler_id);
    printf("%-30s %-20s %-15s %-15s\n", "Good Name", "Type", "Total Quantity", "Total Amount");
    printf("------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-30s %-20s %-15d %-15.2f\n", 
               stats[i].good_name, stats[i].good_type, 
               stats[i].total_quantity, stats[i].total_amount);
    }
    
    free(stats);
}

void stats_show_all_stats() {
    sqlite3 *db = db_get_connection();
    if (!db) return;
    
    char sql[] = "SELECT m.name, s.good_name, s.good_type, s.total_quantity, s.total_amount "
                "FROM PERFUME_MAKLERSTATS s "
                "JOIN PERFUME_MAKLERS m ON s.makler_id = m.id "
                "ORDER BY m.name, s.good_name;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    printf("\nAll Makler Statistics:\n");
    printf("%-20s %-30s %-20s %-15s %-15s\n", "Makler", "Good Name", "Type", "Total Quantity", "Total Amount");
    printf("----------------------------------------------------------------------------------------------------------\n");
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *makler_name = (const char *)sqlite3_column_text(stmt, 0);
        const char *good_name = (const char *)sqlite3_column_text(stmt, 1);
        const char *good_type = (const char *)sqlite3_column_text(stmt, 2);
        int total_quantity = sqlite3_column_int(stmt, 3);
        double total_amount = sqlite3_column_double(stmt, 4);
        
        printf("%-20s %-30s %-20s %-15d %-15.2f\n", 
               makler_name, good_name, good_type, total_quantity, total_amount);
    }
    
    sqlite3_finalize(stmt);
}
