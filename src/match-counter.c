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
#include <util/dstr.h>

match_counter_t *match_counter_create(void)
{
	match_counter_t *counter = bzalloc(sizeof(match_counter_t));
	counter->wins = 0;
	counter->losses = 0;
	counter->format = bstrdup("%w-%l(%r)");
	return counter;
}

void match_counter_destroy(match_counter_t *counter)
{
	if (!counter)
		return;

	bfree(counter->format);
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

int match_counter_get_wins(match_counter_t *counter)
{
	if (!counter)
		return 0;

	return counter->wins;
}

int match_counter_get_losses(match_counter_t *counter)
{
	if (!counter)
		return 0;

	return counter->losses;
}

void match_counter_set_wins(match_counter_t *counter, int wins)
{
	if (!counter)
		return;

	counter->wins = wins < 0 ? 0 : wins;
}

void match_counter_set_losses(match_counter_t *counter, int losses)
{
	if (!counter)
		return;

	counter->losses = losses < 0 ? 0 : losses;
}

float match_counter_get_win_rate(match_counter_t *counter)
{
	if (!counter)
		return 0.0f;

	int total = counter->wins + counter->losses;
	if (total == 0)
		return 0.0f;

	return (float)counter->wins / (float)total;
}

void match_counter_set_format(match_counter_t *counter, const char *format)
{
	if (!counter || !format)
		return;

	bfree(counter->format);
	counter->format = bstrdup(format);
}

const char *match_counter_get_format(match_counter_t *counter)
{
	if (!counter)
		return "";

	return counter->format;
}

char *match_counter_get_formatted_text(match_counter_t *counter)
{
	if (!counter)
		return bstrdup("");

	struct dstr str = {0};
	const char *format = counter->format;
	int wins = counter->wins;
	int losses = counter->losses;
	float win_rate = match_counter_get_win_rate(counter);

	dstr_init(&str);

	while (*format) {
		if (*format == '%') {
			format++;
			if (*format == 'w') {
				dstr_catf(&str, "%d", wins);
			} else if (*format == 'l') {
				dstr_catf(&str, "%d", losses);
			} else if (*format == 'r') {
				// 勝率をパーセント表示（小数点以下1桁）
				dstr_catf(&str, "%.1f%%", win_rate * 100.0f);
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
