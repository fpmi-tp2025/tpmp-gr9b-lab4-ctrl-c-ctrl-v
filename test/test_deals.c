#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "deals.h"
#include "database.h"

void setup_test_data() {
    // Create user and makler
    User user = {0};
    strcpy(user.username, "dealsuser");
    strcpy(user.password_hash, "hash");
    user.role = ROLE_MAKLER;
    int user_id = db_create_user(&user);
    
    Makler makler = {0};
    strcpy(makler.name, "Test Makler");
    makler.user_id = user_id;
    db_create_makler(&makler);
    
    // Create goods
    Good good1 = {0};
    strcpy(good1.name, "Good1");
    strcpy(good1.type, "type1");
    good1.unit_price = 100.0;
    good1.quantity = 50;
    db_create_good(&good1);
    
    Good good2 = {0};
    strcpy(good2.name, "Good2");
    strcpy(good2.type, "type2");
    good2.unit_price = 200.0;
    good2.quantity = 30;
    db_create_good(&good2);
}

void test_create_deal() {
    printf("Testing deal creation...\n");
    
    db_init("test_deals.db");
    setup_test_data();
    
    // Test creating a deal
    int deal_id = deals_create_deal(1, 5, "Test Buyer", 1);
    assert(deal_id > 0);
    
    // Verify good quantity decreased
    Good *good = db_get_good_by_id(1);
    assert(good->quantity == 45); // 50 - 5
    db_free_good(good);
    
    // Test creating deal with insufficient quantity
    deal_id = deals_create_deal(1, 100, "Test Buyer", 1);
    assert(deal_id == -1);
    
    db_close();
    remove("test_deals.db");
    
    printf("✓ Deal creation passed\n");
}

void test_deal_retrieval() {
    printf("Testing deal retrieval...\n");
    
    db_init("test_deals.db");
    setup_test_data();
    
    // Create deals
    deals_create_deal(1, 10, "Buyer1", 1);
    deals_create_deal(2, 5, "Buyer2", 1);
    
    // Test getting makler deals
    int count;
    Deal **deals = deals_get_makler_deals(1, &count);
    assert(count == 2);
    assert(deals[0]->quantity == 10);
    assert(deals[1]->quantity == 5);
    
    for (int i = 0; i < count; i++) {
        db_free_deal(deals[i]);
    }
    free(deals);
    
    // Test getting all deals
    deals = deals_get_all_deals(&count);
    assert(count >= 2);
    
    for (int i = 0; i < count; i++) {
        db_free_deal(deals[i]);
    }
    free(deals);
    
    db_close();
    remove("test_deals.db");
    
    printf("✓ Deal retrieval passed\n");
}

void test_deal_calculations() {
    printf("Testing deal calculations...\n");
    
    db_init("test_deals.db");
    setup_test_data();
    
    // Test calculating total
    double total = deals_calculate_total(1, 10);
    assert(total == 1000.0); // 100.0 * 10
    
    total = deals_calculate_total(2, 5);
    assert(total == 1000.0); // 200.0 * 5
    
    db_close();
    remove("test_deals.db");
    
    printf("✓ Deal calculations passed\n");
}

void test_deal_validations() {
    printf("Testing deal validations...\n");
    
    db_init("test_deals.db");
    
    // Create goods with specific quantities
    Good good1 = {0};
    strcpy(good1.name, "LimitedGood");
    strcpy(good1.type, "type");
    good1.unit_price = 100.0;
    good1.quantity = 10;
    int good1_id = db_create_good(&good1);
    
    Good good2 = {0};
    strcpy(good2.name, "ValidGood");
    strcpy(good2.type, "type");
    good2.unit_price = 200.0;
    good2.quantity = 20;
    int good2_id = db_create_good(&good2);
    
    // Test availability validation
    assert(deals_validate_availability(good1_id, 5) == 1);
    assert(deals_validate_availability(good1_id, 15) == 0);
    
    // Skip expiry date validation as strptime is not portable
    
    db_close();
    remove("test_deals.db");
    
    printf("✓ Deal validations passed\n");
}

void test_deal_statistics() {
    printf("Testing deal statistics...\n");
    
    db_init("test_deals.db");
    setup_test_data();
    
    // Create multiple deals for statistics
    deals_create_deal(1, 10, "Buyer1", 1);
    deals_create_deal(1, 10, "Buyer2", 1);
    deals_create_deal(2, 5, "Buyer1", 1);
    
    // Test statistics functions (we can't test output, just ensure they don't crash)
    deals_show_stats_by_good("2024-01-01", "2024-12-31");
    deals_show_popular_good();
    deals_show_max_deals_makler();
    deals_show_sales_by_suppliers();
    
    db_close();
    remove("test_deals.db");
    
    printf("✓ Deal statistics passed\n");
}

int main() {
    printf("Starting deals tests...\n\n");
    
    test_create_deal();
    test_deal_retrieval();
    test_deal_calculations();
    test_deal_validations();
    test_deal_statistics();
    
    printf("\n✅ All deals tests passed!\n");
    return 0;
}
