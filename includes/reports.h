#ifndef REPORTS_H
#define REPORTS_H

#include "types.h"

// Report functions
void reports_sales_by_good(const char *start_date, const char *end_date);
void reports_buyers_by_good(const char *good_name);
void reports_popular_good_type();
void reports_max_deals_makler();
void reports_sales_by_supplier();
void reports_makler_deals(int makler_id, const char *date);
void reports_update_stock(const char *date);

// Statistics functions
int stats_update_on_deal(const Deal *deal);
void stats_show_makler_stats(int makler_id);
void stats_show_all_stats();

#endif // REPORTS_H
