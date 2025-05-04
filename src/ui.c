#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

void ui_clear_screen() {
    system(CLEAR_SCREEN);
}

void ui_wait_enter() {
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

void ui_show_main_menu() {
    ui_clear_screen();
    printf("===================================\n");
    printf("    PARFUM BAZAAR MANAGEMENT      \n");
    printf("===================================\n");
    printf("1. Login\n");
    printf("2. Exit\n");
    printf("===================================\n");
}

void ui_show_admin_menu() {
    ui_clear_screen();
    printf("==================================\n");
    printf("       ADMINISTRATOR MENU       \n");
    printf("==================================\n");
    printf("1. Add Makler\n");
    printf("2. Add Good\n");
    printf("3. View All Deals\n");
    printf("4. Sales Report by Period\n");
    printf("5. Popular Good Type\n");
    printf("6. Top Makler\n");
    printf("7. Logout\n");
    printf("==================================\n");
}

void ui_show_makler_menu() {
    ui_clear_screen();
    printf("==================================\n");
    printf("         MAKLER MENU           \n");
    printf("==================================\n");
    printf("1. Create Deal\n");
    printf("2. View My Deals\n");
    printf("3. View My Statistics\n");
    printf("4. View Available Goods\n");
    printf("5. Logout\n");
    printf("==================================\n");
}

void ui_login_screen() {
    ui_clear_screen();
    printf("==================================\n");
    printf("          LOGIN SCREEN          \n");
    printf("==================================\n");
}

int ui_get_int(const char *prompt) {
    int value;
    char buffer[100];
    
    printf("%s", prompt);
    fgets(buffer, sizeof(buffer), stdin);
    sscanf(buffer, "%d", &value);
    return value;
}

double ui_get_double(const char *prompt) {
    double value;
    char buffer[100];
    
    printf("%s", prompt);
    fgets(buffer, sizeof(buffer), stdin);
    sscanf(buffer, "%lf", &value);
    return value;
}

void ui_get_string(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    // Remove newline if present
    buffer[strcspn(buffer, "\n")] = 0;
}

void ui_get_date(const char *prompt, char *buffer) {
    printf("%s", prompt);
    fgets(buffer, 11, stdin);
    // Remove newline if present
    buffer[strcspn(buffer, "\n")] = 0;
}

void ui_display_user(const User *user) {
    printf("User: %s (Role: %s)\n", 
           user->username, 
           user->role == ROLE_ADMIN ? "Admin" : "Makler");
}

void ui_display_makler(const Makler *makler) {
    printf("Makler: %s\n", makler->name);
    printf("Address: %s\n", makler->address);
    printf("Birth Year: %d\n", makler->birth_year);
    printf("------------------------\n");
}

void ui_display_good(const Good *good) {
    printf("ID: %d - %s (%s)\n", good->id, good->name, good->type);
    printf("Price: %.2f, Stock: %d\n", good->unit_price, good->quantity);
    printf("Supplier: %s\n", good->supplier);
    printf("Expires: %s\n", good->expiry_date);
    printf("------------------------\n");
}

void ui_display_deal(const Deal *deal) {
    printf("Deal #%d - Date: %s\n", deal->id, ctime(&deal->deal_date));
    printf("Good: %s (%s) - Qty: %d\n", deal->good_name, deal->good_type, deal->quantity);
    printf("Total: %.2f - Buyer: %s\n", deal->total_amount, deal->buyer);
    printf("------------------------\n");
}

void ui_display_stats(const MaklerStats *stats) {
    printf("Good: %s (%s)\n", stats->good_name, stats->good_type);
    printf("Total Sold: %d units for %.2f\n", stats->total_quantity, stats->total_amount);
    printf("------------------------\n");
}

void ui_show_error(const char *message) {
    printf("\n[ERROR] %s\n", message);
}

void ui_show_success(const char *message) {
    printf("\n[SUCCESS] %s\n", message);
}
