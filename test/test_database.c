#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "database.h"

void test_db_init() {
    printf("Testing database initialization...\n");
    int result = db_init("test.db");
    assert(result == 0);
    db_close();
    printf("✓ Database initialization passed\n");
}

void test_user_operations() {
    printf("Testing user operations...\n");
    db_init("test.db");
    
    // Test create user
    User user = {0};
    strcpy(user.username, "testuser");
    strcpy(user.password_hash, "hashedpassword");
    user.role = ROLE_MAKLER;
    
    int user_id = db_create_user(&user);
    assert(user_id > 0);
    
    // Test get user by username
    User *retrieved_user = db_get_user_by_username("testuser");
    assert(retrieved_user != NULL);
    assert(strcmp(retrieved_user->username, "testuser") == 0);
    assert(retrieved_user->role == ROLE_MAKLER);
    
    db_free_user(retrieved_user);
    db_close();
    printf("✓ User operations passed\n");
}

void test_good_operations() {
    printf("Testing good operations...\n");
    db_init("test.db");
    
    // Test create good
    Good good = {0};
    strcpy(good.name, "Test Perfume");
    strcpy(good.type, "парфюмерия");
    good.unit_price = 1000.0;
    strcpy(good.supplier, "Test Supplier");
    strcpy(good.expiry_date, "2025-12-31");
    good.quantity = 10;
    
    int good_id = db_create_good(&good);
    assert(good_id > 0);
    
    // Test get good by id
    Good *retrieved_good = db_get_good_by_id(good_id);
    assert(retrieved_good != NULL);
    assert(strcmp(retrieved_good->name, "Test Perfume") == 0);
    assert(retrieved_good->quantity == 10);
    
    // Test check availability
    int available = db_check_good_availability(good_id, 5);
    assert(available == 1);
    
    available = db_check_good_availability(good_id, 20);
    assert(available == 0);
    
    db_free_good(retrieved_good);
    db_close();
    printf("✓ Good operations passed\n");
}

void test_deal_operations() {
    printf("Testing deal operations...\n");
    db_init("test.db");
    
    // Create prerequisite data
    User user = {0};
    strcpy(user.username, "testmakler");
    strcpy(user.password_hash, "hashedpassword");
    user.role = ROLE_MAKLER;
    int user_id = db_create_user(&user);
    
    Makler makler = {0};
    strcpy(makler.name, "Test Makler");
    makler.user_id = user_id;
    int makler_id = db_create_makler(&makler);
    
    Good good = {0};
    strcpy(good.name, "Test Good");
    strcpy(good.type, "type");
    good.unit_price = 100.0;
    good.quantity = 20;
    int good_id = db_create_good(&good);
    
    // Test create deal
    Deal deal = {0};
    deal.deal_date = time(NULL);
    strcpy(deal.good_name, "Test Good");
    strcpy(deal.good_type, "type");
    deal.quantity = 5;
    deal.total_amount = 500.0;
    deal.makler_id = makler_id;
    deal.good_id = good_id;
    strcpy(deal.buyer, "Test Buyer");
    
    int deal_id = db_create_deal(&deal);
    assert(deal_id > 0);
    
    // Test get deals
    int count;
    Deal **deals = db_get_deals_by_makler(makler_id, &count);
    assert(count > 0);
    assert(deals[0]->quantity == 5);
    
    // Check good quantity reduced
    Good *updated_good = db_get_good_by_id(good_id);
    assert(updated_good->quantity == 15); // 20 - 5
    
    for (int i = 0; i < count; i++) {
        db_free_deal(deals[i]);
    }
    free(deals);
    db_free_good(updated_good);
    
    db_close();
    printf("✓ Deal operations passed\n");
}

void test_stats_operations() {
    printf("Testing statistics operations...\n");
    db_init("test.db");
    
    // Create test data (reusing previous test setup)
    User user = {0};
    strcpy(user.username, "statsmakler");
    strcpy(user.password_hash, "hashedpassword");
    user.role = ROLE_MAKLER;
    int user_id = db_create_user(&user);
    
    Makler makler = {0};
    strcpy(makler.name, "Stats Makler");
    makler.user_id = user_id;
    int makler_id = db_create_makler(&makler);
    
    Good good = {0};
    strcpy(good.name, "Stats Good");
    strcpy(good.type, "type");
    good.unit_price = 200.0;
    good.quantity = 50;
    int good_id = db_create_good(&good);
    
    Deal deal = {0};
    deal.deal_date = time(NULL);
    strcpy(deal.good_name, "Stats Good");
    strcpy(deal.good_type, "type");
    deal.quantity = 10;
    deal.total_amount = 2000.0;
    deal.makler_id = makler_id;
    deal.good_id = good_id;
    strcpy(deal.buyer, "Stats Buyer");
    
    db_create_deal(&deal);
    
    // Test get makler stats
    int count;
    MaklerStats *stats = db_get_makler_stats(makler_id, &count);
    assert(count > 0);
    assert(stats[0].total_quantity == 10);
    assert(stats[0].total_amount == 2000.0);
    
    free(stats);
    db_close();
    printf("✓ Statistics operations passed\n");
}

int main() {
    printf("Starting database tests...\n\n");
    
    test_db_init();
    test_user_operations();
    test_good_operations();
    test_deal_operations();
    test_stats_operations();
    
    // Cleanup
    remove("test.db");
    
    printf("\n✅ All database tests passed!\n");
    return 0;
}
