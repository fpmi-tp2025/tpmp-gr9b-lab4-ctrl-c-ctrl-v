#ifndef UI_H
#define UI_H

#include "types.h"

// UI functions
void ui_show_main_menu();
void ui_show_admin_menu();
void ui_show_makler_menu();
void ui_login_screen();
void ui_clear_screen();
void ui_wait_enter();

// Input functions
int ui_get_int(const char *prompt);
double ui_get_double(const char *prompt);
void ui_get_string(const char *prompt, char *buffer, size_t size);
void ui_get_date(const char *prompt, char *buffer);

// Display functions
void ui_display_user(const User *user);
void ui_display_makler(const Makler *makler);
void ui_display_good(const Good *good);
void ui_display_deal(const Deal *deal);
void ui_display_stats(const MaklerStats *stats);

// Error handling
void ui_show_error(const char *message);
void ui_show_success(const char *message);

#endif // UI_H
