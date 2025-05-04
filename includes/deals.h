#ifndef DEALS_H
#define DEALS_H

#include "types.h"

// Deal management
int deals_create_deal(int good_id, int quantity, const char *buyer, int makler_id);
Deal** deals_get_makler_deals(int makler_id, int *count);
Deal** deals_get_all_deals(int *count);
double deals_calculate_total(int good_id, int quantity);

// Deal validation
int deals_validate_availability(int good_id, int quantity);
int deals_validate_expiry(int good_id);

// Deal statistics
void deals_show_stats_by_good(const char *start_date, const char *end_date);
void deals_show_popular_good();
void deals_show_max_deals_makler();
void deals_show_sales_by_suppliers();

#endif // DEALS_H
