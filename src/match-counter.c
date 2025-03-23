/*
Match Counter for OBS
Copyright (C) 2025 Yudai Udagawa

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include "match-counter.h"
#include <plugin-support.h>
#include <util/platform.h>

// グローバルな試合カウンター
static match_counter_t *global_counter = NULL;

match_counter_t *match_counter_create(void)
{
    match_counter_t *counter = bzalloc(sizeof(match_counter_t));
    counter->wins = 0;
    counter->losses = 0;
    counter->format = bstrdup("%n: %w - %l");
    counter->player_name = bstrdup("Player");
    return counter;
}

void match_counter_destroy(match_counter_t *counter)
{
    if (!counter)
        return;

    bfree(counter->format);
    bfree(counter->player_name);
    bfree(counter);
}

void match_counter_add_win(match_counter_t *counter)
{
    if (!counter)
        return;

    counter->wins++;
}

void match_counter_add_loss(match_counter_t *counter)
{
    if (!counter)
        return;

    counter->losses++;
}

void match_counter_subtract_win(match_counter_t *counter)
{
    if (!counter || counter->wins <= 0)
        return;

    counter->wins--;
}

void match_counter_subtract_loss(match_counter_t *counter)
{
    if (!counter || counter->losses <= 0)
        return;

    counter->losses--;
}

void match_counter_reset(match_counter_t *counter)
{
    if (!counter)
        return;

    counter->wins = 0;
    counter->losses = 0;
}

void match_counter_set_format(match_counter_t *counter, const char *format)
{
    if (!counter || !format)
        return;

    bfree(counter->format);
    counter->format = bstrdup(format);
}

void match_counter_set_player_name(match_counter_t *counter, const char *name)
{
    if (!counter || !name)
        return;

    bfree(counter->player_name);
    counter->player_name = bstrdup(name);
}

char *match_counter_get_formatted_text(match_counter_t *counter)
{
    if (!counter)
        return bstrdup("");

    struct dstr str = {0};
    const char *format = counter->format;
    const char *player_name = counter->player_name;
    int wins = counter->wins;
    int losses = counter->losses;

    dstr_init(&str);

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == 'w') {
                dstr_catf(&str, "%d", wins);
            } else if (*format == 'l') {
                dstr_catf(&str, "%d", losses);
            } else if (*format == 'n') {
                dstr_cat(&str, player_name);
            } else {
                dstr_catf(&str, "%%%c", *format);
            }
        } else {
            dstr_catf(&str, "%c", *format);
        }
        format++;
    }

    return str.array;
}

match_counter_t *match_counter_get_global(void)
{
    if (!global_counter) {
        global_counter = match_counter_create();
    }
    return global_counter;
}

void match_counter_free_global(void)
{
    if (global_counter) {
        match_counter_destroy(global_counter);
        global_counter = NULL;
    }
}
