#include <stdio.h>
#include <stdlib.h>

// External test declarations
extern int test_database();
extern int test_auth();
extern int test_deals();

int main() {
    printf("==========================================\n");
    printf("   RUNNING ALL TESTS FOR PARFUM BAZAAR   \n");
    printf("==========================================\n\n");
    
    int failures = 0;
    
    printf("========================================\n");
    printf("Running database tests...\n");
    printf("========================================\n");
    failures += system("./test/test_database");
    
    printf("\n========================================\n");
    printf("Running authentication tests...\n");
    printf("========================================\n");
    failures += system("./test/test_auth");
    
    printf("\n========================================\n");
    printf("Running deals tests...\n");
    printf("========================================\n");
    failures += system("./test/test_deals");
    
    printf("\n==========================================\n");
    if (failures == 0) {
        printf("✅ ALL TESTS PASSED SUCCESSFULLY!\n");
    } else {
        printf("❌ SOME TESTS FAILED (%d failure(s))\n", failures / 256);
    }
    printf("==========================================\n");
    
    return failures;
}
