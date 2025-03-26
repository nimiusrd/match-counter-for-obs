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

struct MatchCounterSource {
	obs_source_t *source;
	obs_hotkey_id win_hotkey;
	obs_hotkey_id loss_hotkey;
	obs_hotkey_id reset_hotkey;
	char *format;

	// テキスト描画用の設定
	gs_texrender_t *texrender;
	gs_stagesurf_t *stagesurface;
	uint32_t cx;
	uint32_t cy;
	char *text;

	// テキストソース
	obs_source_t *text_source;

	// テキストのスタイル設定
	uint32_t color;
	uint32_t color2;
	uint32_t custom_width;
	uint32_t outline_width;
	uint32_t outline_color;
	char *font_name;
	uint16_t font_size;
	uint32_t font_flags;
	bool outline;
	bool gradient;
	bool align_center;
	bool vertical_align_center;

	match_counter_t *counter;
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
static void match_counter_source_render(void *data, gs_effect_t *effect);

static const char *match_counter_source_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("MatchCounterSource");
}

static void match_counter_source_update(void *data, obs_data_t *settings)
{
	blog(LOG_INFO, "match_counter_source_update: Updating match counter source");

	struct MatchCounterSource *context = data;
	context->counter = match_counter_create();

	const char *format = obs_data_get_string(settings, "format");

	// フォント設定の取得
	obs_data_t *font_obj = obs_data_get_obj(settings, "font");
	const char *font_name = obs_data_get_string(font_obj, "face");
	uint16_t font_size = (uint16_t)obs_data_get_int(font_obj, "size");
	uint32_t font_flags = (uint32_t)obs_data_get_int(font_obj, "flags");

	blog(LOG_DEBUG, "match_counter_source_update: Font settings - name='%s', size=%d, flags=%d",
	     font_name && strlen(font_name) ? font_name : "Arial", font_size, font_flags);

	if (font_size <= 0)
		font_size = 256;

	bfree(context->format);
	bfree(context->font_name);

	context->format = bstrdup(format);
	context->font_name = bstrdup(font_name && strlen(font_name) ? font_name : "Arial");
	context->font_size = font_size;
	context->font_flags = font_flags;

	context->counter->wins = (int)obs_data_get_int(settings, "wins");
	context->counter->losses = (int)obs_data_get_int(settings, "losses");

	obs_data_release(font_obj);

	match_counter_set_format(context->counter, format);

	match_counter_source_render(context, NULL);

	blog(LOG_DEBUG, "match_counter_source_update: Updated with format='%s'", format);
}

static void *match_counter_source_create(obs_data_t *settings, obs_source_t *source)
{
	blog(LOG_INFO, "match_counter_source_create: Creating match counter source");

	struct MatchCounterSource *context = bzalloc(sizeof(struct MatchCounterSource));
	context->source = source;
	context->format = bstrdup("%w - %l (%r)");

	// テキスト描画用の設定
	context->texrender = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
	context->font_name = bstrdup("Arial");
	context->font_size = 32;
	context->font_flags = 0;

	blog(LOG_DEBUG, "match_counter_source_create: Initializing with format='%s'", context->format);

	match_counter_source_update(context, settings);

	// ホットキーの設定
	context->win_hotkey = obs_hotkey_register_source(source, "match_counter_win", obs_module_text("AddWin"),
							 match_counter_win_hotkey, context);

	context->loss_hotkey = obs_hotkey_register_source(source, "match_counter_loss", obs_module_text("AddLoss"),
							  match_counter_loss_hotkey, context);

	context->reset_hotkey = obs_hotkey_register_source(
		source, "match_counter_reset", obs_module_text("ResetCounter"), match_counter_reset_hotkey, context);

	blog(LOG_INFO, "match_counter_source_create: Match counter source created successfully");
	return context;
}

static void match_counter_source_destroy(void *data)
{
	blog(LOG_INFO, "match_counter_source_destroy: Destroying match counter source");

	struct MatchCounterSource *context = data;

	obs_hotkey_unregister(context->win_hotkey);
	obs_hotkey_unregister(context->loss_hotkey);
	obs_hotkey_unregister(context->reset_hotkey);

	// テキスト描画リソースの解放
	if (context->texrender) {
		gs_texrender_destroy(context->texrender);
		context->texrender = NULL;
	}
	if (context->stagesurface) {
		gs_stagesurface_destroy(context->stagesurface);
		context->stagesurface = NULL;
	}

	// テキストソースの解放
	if (context->text_source) {
		blog(LOG_DEBUG, "match_counter_source_destroy: Releasing text source");
		obs_source_release(context->text_source);
		context->text_source = NULL;
	}

	bfree(context->format);
	bfree(context->font_name);
	bfree(context->text);
	bfree(context);

	blog(LOG_INFO, "match_counter_source_destroy: Match counter source destroyed");
}

static void match_counter_win_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(hotkey);

	struct MatchCounterSource *context = data;

	if (pressed) {
		blog(LOG_INFO, "match_counter_win_hotkey: Adding win");
		match_counter_add_win(context->counter);
		obs_source_update_properties(context->source);
		blog(LOG_DEBUG, "match_counter_win_hotkey: Current score - wins=%d, losses=%d",
		     match_counter_get_wins(context->counter), match_counter_get_losses(context->counter));
	}
}

static void match_counter_loss_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(hotkey);

	struct MatchCounterSource *context = data;

	if (pressed) {
		blog(LOG_INFO, "match_counter_loss_hotkey: Adding loss");
		match_counter_add_loss(context->counter);
		obs_source_update_properties(context->source);
		blog(LOG_DEBUG, "match_counter_loss_hotkey: Current score - wins=%d, losses=%d",
		     match_counter_get_wins(context->counter), match_counter_get_losses(context->counter));
	}
}

static void match_counter_reset_hotkey(void *data, obs_hotkey_pair_id id, obs_hotkey_t *hotkey, bool pressed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(hotkey);

	struct MatchCounterSource *context = data;

	if (pressed) {
		blog(LOG_INFO, "match_counter_reset_hotkey: Resetting counter");
		match_counter_reset(context->counter);
		obs_source_update_properties(context->source);
		blog(LOG_DEBUG, "match_counter_reset_hotkey: Counter reset - wins=%d, losses=%d",
		     match_counter_get_wins(context->counter), match_counter_get_losses(context->counter));
	}
}

static void match_counter_source_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
	struct MatchCounterSource *context = data;

	// テキストソースがない場合は作成
	if (!context->text_source) {
		blog(LOG_INFO, "match_counter_source_render: Creating text source");
#ifdef _WIN32
		context->text_source = obs_source_create_private("text_gdiplus", "match_counter_text", NULL);
#else
		context->text_source = obs_source_create_private("text_ft2_source", "match_counter_text", NULL);
#endif

		if (!context->text_source) {
			blog(LOG_ERROR, "match_counter_source_render: Failed to create text source");
			return;
		}
		blog(LOG_DEBUG, "match_counter_source_render: Text source created successfully");
	}

	// テキストの更新が必要かチェック
	const char *formatted_text = match_counter_get_formatted_text(context->counter);

	if (!formatted_text || !strlen(formatted_text)) {
		blog(LOG_DEBUG, "match_counter_source_render: Empty text, skipping render");
		bfree((void *)formatted_text);
		return;
	}

	blog(LOG_DEBUG, "match_counter_source_render: Rendering text '%s'", formatted_text);

	// テキストソースの設定を更新
	obs_data_t *settings = obs_data_create();
	obs_data_set_string(settings, "text", formatted_text);

	// フォント設定
	obs_data_t *font_obj = obs_data_create();
	obs_data_set_string(font_obj, "face", context->font_name);
	obs_data_set_int(font_obj, "size", context->font_size);
	obs_data_set_int(font_obj, "flags", context->font_flags);
	obs_data_set_obj(settings, "font", font_obj);
	obs_data_release(font_obj);

	// テキストソースを更新
	obs_source_update(context->text_source, settings);

	// テキストソースのサイズを取得
	context->cx = obs_source_get_width(context->text_source);
	context->cy = obs_source_get_height(context->text_source);

	blog(LOG_DEBUG, "match_counter_source_render: Text dimensions - width=%d, height=%d", context->cx, context->cy);

	// テキストソースをレンダリング
	obs_source_video_render(context->text_source);

	// リソースの解放
	obs_data_release(settings);
	bfree((void *)formatted_text);
}

static uint32_t match_counter_source_get_width(void *data)
{
	struct MatchCounterSource *context = data;

	// テクスチャのサイズを返す
	if (context->cx > 0) {
		return context->cx;
	}

	// テクスチャがまだ作成されていない場合は簡易計算
	const char *text = match_counter_get_formatted_text(context->counter);

	if (!text || !strlen(text)) {
		return 0;
	}

	uint32_t width = (uint32_t)strlen(text) * 10; // 文字幅の簡易計算
	bfree((void *)text);
	return width;
}

static uint32_t match_counter_source_get_height(void *data)
{
	struct MatchCounterSource *context = data;

	// テクスチャのサイズを返す
	if (context->cy > 0) {
		return context->cy;
	}

	// テクスチャがまだ作成されていない場合は固定値
	return context->font_size > 0 ? context->font_size : 256;
}

static obs_properties_t *match_counter_source_get_properties(void *data, void *type_data)
{
	UNUSED_PARAMETER(data);
	UNUSED_PARAMETER(type_data);

	obs_properties_t *props = obs_properties_create();

	// カウンター設定
	obs_properties_add_text(props, "format", obs_module_text("Format"), OBS_TEXT_MULTILINE);
	obs_property_set_long_description(obs_properties_get(props, "format"), obs_module_text("FormatTooltip"));

	// 勝敗数設定
	obs_properties_add_int(props, "wins", obs_module_text("Wins"), 0, INT_MAX, 1);
	obs_properties_add_int(props, "losses", obs_module_text("Losses"), 0, INT_MAX, 1);

	// テキストスタイル設定
	obs_properties_add_font(props, "font", obs_module_text("Font"));

	return props;
}

static void match_counter_source_get_defaults(void *type_data, obs_data_t *settings)
{
	UNUSED_PARAMETER(type_data);
	// カウンター設定のデフォルト値
	obs_data_set_default_string(settings, "format", "%w-%l(%r)");

	// フォント設定のデフォルト値
	obs_data_t *font_obj = obs_data_create();
	obs_data_set_string(font_obj, "face", "Arial");
	obs_data_set_int(font_obj, "size", 256);
	obs_data_set_int(font_obj, "flags", 0);
	obs_data_set_default_obj(settings, "font", font_obj);
	obs_data_release(font_obj);
}

static const char *match_counter_source_get_text(void *data)
{
	struct MatchCounterSource *context = data;
	return match_counter_get_formatted_text(context->counter);
}

struct obs_source_info match_counter_source_info = {.id = "match_counter_source",
						    .type = OBS_SOURCE_TYPE_INPUT,
						    .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW |
								    OBS_SOURCE_SRGB,
						    .icon_type = OBS_ICON_TYPE_TEXT,
						    .get_name = match_counter_source_get_name,
						    .create = match_counter_source_create,
						    .destroy = match_counter_source_destroy,
						    .update = match_counter_source_update,
						    .get_properties2 = match_counter_source_get_properties,
						    .get_defaults2 = match_counter_source_get_defaults,
						    .get_width = match_counter_source_get_width,
						    .get_height = match_counter_source_get_height,
						    .video_render = match_counter_source_render};
