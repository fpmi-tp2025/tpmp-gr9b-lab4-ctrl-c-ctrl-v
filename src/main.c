#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "auth.h"
#include "ui.h"
#include "deals.h"
#include "reports.h"

#define DB_PATH "parfum_bazaar.db"

void admin_menu() {
    int choice;
    
    do {
        ui_show_admin_menu();
        choice = ui_get_int("Enter your choice: ");
        
        switch (choice) {
            case 1: {
                // Add makler
                Makler makler = {0};
                ui_get_string("Name: ", makler.name, sizeof(makler.name));
                ui_get_string("Address: ", makler.address, sizeof(makler.address));
                makler.birth_year = ui_get_int("Birth year: ");
                
                // Create user for makler
                User user = {0};
                ui_get_string("Username: ", user.username, sizeof(user.username));
                char password[50];
                ui_get_string("Password: ", password, sizeof(password));
                strcpy(user.password_hash, password);  // Direct password copy for testing
                user.role = ROLE_MAKLER;
                
                int user_id = db_create_user(&user);
                if (user_id > 0) {
                    makler.user_id = user_id;
                    if (db_create_makler(&makler) > 0) {
                        ui_show_success("Makler added successfully!");
                    } else {
                        ui_show_error("Failed to add makler.");
                    }
                } else {
                    ui_show_error("Failed to create user for makler.");
                }
                break;
            }
            case 2: {
                // Add good
                Good good = {0};
                ui_get_string("Name: ", good.name, sizeof(good.name));
                ui_get_string("Type: ", good.type, sizeof(good.type));
                good.unit_price = ui_get_double("Unit price: ");
                ui_get_string("Supplier: ", good.supplier, sizeof(good.supplier));
                ui_get_string("Expiry date (YYYY-MM-DD): ", good.expiry_date, sizeof(good.expiry_date));
                good.quantity = ui_get_int("Quantity: ");
                
                if (db_create_good(&good) > 0) {
                    ui_show_success("Good added successfully!");
                } else {
                    ui_show_error("Failed to add good.");
                }
                break;
            }
            case 3: {
                int count;
                Deal **deals = db_get_all_deals(&count);
                printf("\nAll Deals:\n");
                for (int i = 0; i < count; i++) {
                    ui_display_deal(deals[i]);
                    db_free_deal(deals[i]);
                }
                free(deals);
                break;
            }
            case 4: {
                char start[11], end[11];
                ui_get_date("Start date (YYYY-MM-DD): ", start);
                ui_get_date("End date (YYYY-MM-DD): ", end);
                reports_sales_by_good(start, end);
                break;
            }
            case 5: {
                reports_popular_good_type();
                break;
            }
            case 6: {
                reports_max_deals_makler();
                break;
            }
            case 7: {
                auth_logout(auth_get_current_user());
                return;
            }
        }
        ui_wait_enter();
    } while (choice != 7);
}

void makler_menu() {
    int choice;
    User *current_user = auth_get_current_user();
    Makler *makler = db_get_makler_by_user_id(current_user->id);
    
    if (!makler) {
        ui_show_error("Error getting makler information!");
        return;
    }
    
    do {
        ui_show_makler_menu();
        choice = ui_get_int("Enter your choice: ");
        
        switch (choice) {
            case 1: {
                // Create deal
                int good_id = ui_get_int("Good ID: ");
                int quantity = ui_get_int("Quantity: ");
                char buyer[100];
                ui_get_string("Buyer company: ", buyer, sizeof(buyer));
                
                if (deals_create_deal(good_id, quantity, buyer, makler->id) > 0) {
                    ui_show_success("Deal created successfully!");
                } else {
                    ui_show_error("Failed to create deal.");
                }
                break;
            }
            case 2: {
                int count;
                Deal **deals = deals_get_makler_deals(makler->id, &count);
                printf("\nYour Deals:\n");
                for (int i = 0; i < count; i++) {
                    ui_display_deal(deals[i]);
                    db_free_deal(deals[i]);
                }
                free(deals);
                break;
            }
            case 3: {
                int count;
                MaklerStats *stats = db_get_makler_stats(makler->id, &count);
                printf("\nYour Statistics:\n");
                for (int i = 0; i < count; i++) {
                    ui_display_stats(&stats[i]);
                }
                break;
            }
            case 4: {
                int count;
                Good **goods = db_get_all_goods(&count);
                printf("\nAvailable Goods:\n");
                for (int i = 0; i < count; i++) {
                    ui_display_good(goods[i]);
                    db_free_good(goods[i]);
                }
                free(goods);
                break;
            }
            case 5: {
                auth_logout(current_user);
                db_free_makler(makler);
                return;
            }
        }
        ui_wait_enter();
    } while (choice != 5);
    
    db_free_makler(makler);
}

int main() {
    // Initialize database
    if (db_init(DB_PATH) != 0) {
        ui_show_error("Failed to initialize database!");
        return 1;
    }
    
    User *current_user = NULL;
    
    while (1) {
        ui_login_screen();
        
        char username[50], password[50];
        ui_get_string("Username: ", username, sizeof(username));
        ui_get_string("Password: ", password, sizeof(password));
        
        current_user = auth_login(username, password);
        
        if (current_user) {
            ui_show_success("Login successful!");
            ui_wait_enter();
            
            if (current_user->role == ROLE_ADMIN) {
                admin_menu();
            } else if (current_user->role == ROLE_MAKLER) {
                makler_menu();
            }
        } else {
            ui_show_error("Invalid credentials!");
            ui_wait_enter();
        }
    }
    
    db_close();
    return 0;
}
