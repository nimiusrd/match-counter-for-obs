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

#include <obs-module.h>
#include <plugin-support.h>
#include <util/platform.h>
#include "match-counter.h"

struct match_counter_source {
	obs_source_t *source;
	obs_source_t *text_source; // テキストソースを保持
	obs_hotkey_id win_hotkey;
	obs_hotkey_id loss_hotkey;
	obs_hotkey_id reset_hotkey;
	char *format;
	char *player_name;
};

// 前方宣言
static void match_counter_win_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed);
static void match_counter_loss_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed);
static void match_counter_reset_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed);
static bool match_counter_add_win_button(obs_properties_t *props, obs_property_t *property, void *data);
static bool match_counter_add_loss_button(obs_properties_t *props, obs_property_t *property, void *data);
static bool match_counter_subtract_win_button(obs_properties_t *props, obs_property_t *property, void *data);
static bool match_counter_subtract_loss_button(obs_properties_t *props, obs_property_t *property, void *data);
static bool match_counter_reset_button(obs_properties_t *props, obs_property_t *property, void *data);

static const char *match_counter_source_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("MatchCounterSource");
}

static void match_counter_source_update(void *data, obs_data_t *settings)
{
	struct match_counter_source *context = data;
	match_counter_t *counter = match_counter_get_global();

	const char *format = obs_data_get_string(settings, "format");
	const char *player_name = obs_data_get_string(settings, "player_name");

	bfree(context->format);
	bfree(context->player_name);
	context->format = bstrdup(format);
	context->player_name = bstrdup(player_name);

	match_counter_set_format(counter, format);
	match_counter_set_player_name(counter, player_name);
}

static void *match_counter_source_create(obs_data_t *settings, obs_source_t *source)
{
	struct match_counter_source *context = bzalloc(sizeof(struct match_counter_source));
	context->source = source;
	context->format = bstrdup("%n: %w - %l");
	context->player_name = bstrdup("Player");

	// テキストソースの作成
	context->text_source = obs_source_create_private("text_gdiplus", "match_counter_text", NULL);

	match_counter_source_update(context, settings);

	// ホットキーの設定
	context->win_hotkey = obs_hotkey_register_source(source, "match_counter_win", obs_module_text("AddWin"),
							 match_counter_win_hotkey, context);

	context->loss_hotkey = obs_hotkey_register_source(source, "match_counter_loss", obs_module_text("AddLoss"),
							  match_counter_loss_hotkey, context);

	context->reset_hotkey = obs_hotkey_register_source(
		source, "match_counter_reset", obs_module_text("ResetCounter"), match_counter_reset_hotkey, context);

	return context;
}

static void match_counter_source_destroy(void *data)
{
	struct match_counter_source *context = data;

	obs_hotkey_unregister(context->win_hotkey);
	obs_hotkey_unregister(context->loss_hotkey);
	obs_hotkey_unregister(context->reset_hotkey);

	// テキストソースの解放
	if (context->text_source) {
		obs_source_release(context->text_source);
		context->text_source = NULL;
	}

	bfree(context->format);
	bfree(context->player_name);
	bfree(context);
}

static void match_counter_win_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(hotkey);

	struct match_counter_source *context = data;
	match_counter_t *counter = match_counter_get_global();

	if (pressed) {
		match_counter_add_win(counter);
		obs_source_update_properties(context->source);
	}
}

static void match_counter_loss_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(hotkey);

	struct match_counter_source *context = data;
	match_counter_t *counter = match_counter_get_global();

	if (pressed) {
		match_counter_add_loss(counter);
		obs_source_update_properties(context->source);
	}
}

static void match_counter_reset_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(hotkey);

	struct match_counter_source *context = data;
	match_counter_t *counter = match_counter_get_global();

	if (pressed) {
		match_counter_reset(counter);
		obs_source_update_properties(context->source);
	}
}

static void match_counter_source_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
	struct match_counter_source *context = data;

	if (!context->text_source)
		return;

	// テキストソースの設定を更新
	match_counter_t *counter = match_counter_get_global();
	const char *formatted_text = match_counter_get_formatted_text(counter);

	obs_data_t *settings = obs_data_create();
	obs_data_set_string(settings, "text", formatted_text);
	obs_source_update(context->text_source, settings);

	// テキストソースをレンダリング
	obs_source_video_render(context->text_source);

	// リソースの解放
	obs_data_release(settings);
	bfree((void *)formatted_text);
}

static uint32_t match_counter_source_get_width(void *data)
{
	UNUSED_PARAMETER(data);
	match_counter_t *counter = match_counter_get_global();
	const char *text = match_counter_get_formatted_text(counter);

	if (!text || !strlen(text)) {
		return 0;
	}

	uint32_t width = (uint32_t)strlen(text) * 10; // 文字幅の簡易計算
	bfree((void *)text);
	return width;
}

static uint32_t match_counter_source_get_height(void *data)
{
	UNUSED_PARAMETER(data);
	return 20; // 固定の高さ
}

static obs_properties_t *match_counter_source_get_properties(void *data)
{
	UNUSED_PARAMETER(data);

	obs_properties_t *props = obs_properties_create();

	obs_properties_add_text(props, "player_name", obs_module_text("PlayerName"), OBS_TEXT_DEFAULT);

	obs_properties_add_text(props, "format", obs_module_text("Format"), OBS_TEXT_DEFAULT);

	obs_properties_add_button(props, "add_win", obs_module_text("AddWin"), match_counter_add_win_button);

	obs_properties_add_button(props, "add_loss", obs_module_text("AddLoss"), match_counter_add_loss_button);

	obs_properties_add_button(props, "subtract_win", obs_module_text("SubtractWin"),
				  match_counter_subtract_win_button);

	obs_properties_add_button(props, "subtract_loss", obs_module_text("SubtractLoss"),
				  match_counter_subtract_loss_button);

	obs_properties_add_button(props, "reset", obs_module_text("Reset"), match_counter_reset_button);

	return props;
}

static bool match_counter_add_win_button(obs_properties_t *props, obs_property_t *property, void *data)
{
	UNUSED_PARAMETER(props);
	UNUSED_PARAMETER(property);
	UNUSED_PARAMETER(data);

	match_counter_t *counter = match_counter_get_global();
	match_counter_add_win(counter);
	return true;
}

static bool match_counter_add_loss_button(obs_properties_t *props, obs_property_t *property, void *data)
{
	UNUSED_PARAMETER(props);
	UNUSED_PARAMETER(property);
	UNUSED_PARAMETER(data);

	match_counter_t *counter = match_counter_get_global();
	match_counter_add_loss(counter);
	return true;
}

static bool match_counter_subtract_win_button(obs_properties_t *props, obs_property_t *property, void *data)
{
	UNUSED_PARAMETER(props);
	UNUSED_PARAMETER(property);
	UNUSED_PARAMETER(data);

	match_counter_t *counter = match_counter_get_global();
	match_counter_subtract_win(counter);
	return true;
}

static bool match_counter_subtract_loss_button(obs_properties_t *props, obs_property_t *property, void *data)
{
	UNUSED_PARAMETER(props);
	UNUSED_PARAMETER(property);
	UNUSED_PARAMETER(data);

	match_counter_t *counter = match_counter_get_global();
	match_counter_subtract_loss(counter);
	return true;
}

static bool match_counter_reset_button(obs_properties_t *props, obs_property_t *property, void *data)
{
	UNUSED_PARAMETER(props);
	UNUSED_PARAMETER(property);
	UNUSED_PARAMETER(data);

	match_counter_t *counter = match_counter_get_global();
	match_counter_reset(counter);
	return true;
}

static void match_counter_source_get_defaults(obs_data_t *settings)
{
	obs_data_set_default_string(settings, "format", "%n: %w - %l");
	obs_data_set_default_string(settings, "player_name", "Player");
}

static const char *match_counter_source_get_text(void *data)
{
	UNUSED_PARAMETER(data);
	match_counter_t *counter = match_counter_get_global();

	return match_counter_get_formatted_text(counter);
}

struct obs_source_info match_counter_source_info = {.id = "match_counter_source",
						    .type = OBS_SOURCE_TYPE_INPUT,
						    .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW,
							.icon_type = OBS_ICON_TYPE_TEXT,
						    .get_name = match_counter_source_get_name,
						    .create = match_counter_source_create,
						    .destroy = match_counter_source_destroy,
						    .update = match_counter_source_update,
						    .get_properties = match_counter_source_get_properties,
						    .get_defaults = match_counter_source_get_defaults,
						    .get_width = match_counter_source_get_width,
						    .get_height = match_counter_source_get_height,
						    .video_render = match_counter_source_render};
