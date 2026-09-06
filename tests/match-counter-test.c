#include "match-counter.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <util/bmem.h>

static int failures;

#define EXPECT_TRUE(condition)                                                                              \
	do {                                                                                                \
		if (!(condition)) {                                                                         \
			fprintf(stderr, "%s:%d: expectation failed: %s\n", __FILE__, __LINE__, #condition); \
			failures++;                                                                         \
		}                                                                                           \
	} while (false)

static void expect_text(match_counter_t *counter, const char *format, const char *expected)
{
	match_counter_set_format(counter, format);
	char *actual = match_counter_get_formatted_text(counter);

	if (!expected[0]) {
		EXPECT_TRUE(!actual || !actual[0]);
	} else {
		EXPECT_TRUE(actual != NULL);
	}
	if (actual)
		EXPECT_TRUE(strcmp(actual, expected) == 0);

	bfree(actual);
}

static void test_default_counter(void)
{
	match_counter_t *counter = match_counter_create();

	EXPECT_TRUE(counter != NULL);
	EXPECT_TRUE(match_counter_get_wins(counter) == 0);
	EXPECT_TRUE(match_counter_get_losses(counter) == 0);
	EXPECT_TRUE(fabsf(match_counter_get_win_rate(counter)) < 0.0001f);
	expect_text(counter, "%w-%l(%r)", "0-0(0.0%)");

	match_counter_destroy(counter);
}

static void test_counter_operations(void)
{
	match_counter_t *counter = match_counter_create();

	match_counter_add_win(counter);
	match_counter_add_win(counter);
	match_counter_add_win(counter);
	match_counter_add_loss(counter);

	EXPECT_TRUE(match_counter_get_wins(counter) == 3);
	EXPECT_TRUE(match_counter_get_losses(counter) == 1);
	EXPECT_TRUE(fabsf(match_counter_get_win_rate(counter) - 0.75f) < 0.0001f);
	expect_text(counter, "%w勝 %l敗 / %t戦 / %r", "3勝 1敗 / 4戦 / 75.0%");

	match_counter_reset(counter);
	EXPECT_TRUE(match_counter_get_wins(counter) == 0);
	EXPECT_TRUE(match_counter_get_losses(counter) == 0);

	match_counter_destroy(counter);
}

static void test_formatting(void)
{
	match_counter_t *counter = match_counter_create();

	match_counter_add_win(counter);
	expect_text(counter, "", "");
	expect_text(counter, "%x", "%x");
	expect_text(counter, "%%", "%%");

	match_counter_destroy(counter);
}

static void test_null_counter(void)
{
	char *text = match_counter_get_formatted_text(NULL);

	EXPECT_TRUE(match_counter_get_wins(NULL) == 0);
	EXPECT_TRUE(match_counter_get_losses(NULL) == 0);
	EXPECT_TRUE(fabsf(match_counter_get_win_rate(NULL)) < 0.0001f);
	EXPECT_TRUE(text != NULL);
	if (text)
		EXPECT_TRUE(strcmp(text, "") == 0);

	bfree(text);
	match_counter_add_win(NULL);
	match_counter_add_loss(NULL);
	match_counter_reset(NULL);
	match_counter_set_format(NULL, "%w");
	match_counter_destroy(NULL);
}

int main(void)
{
	test_default_counter();
	test_counter_operations();
	test_formatting();
	test_null_counter();

	if (failures) {
		fprintf(stderr, "%d test expectation(s) failed\n", failures);
		return 1;
	}

	return 0;
}
