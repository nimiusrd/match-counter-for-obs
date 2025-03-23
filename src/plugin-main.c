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
#include "match-counter.h"
#include "match-counter-source.c"

// C++関数の宣言
#ifdef __cplusplus
extern "C" {
#endif
extern void match_counter_ui_init(void);
#ifdef __cplusplus
}
#endif

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);

	// テキストソースの登録
	obs_register_source(&match_counter_source_info);

	// UIの初期化（フロントエンドAPIが有効な場合）
#ifdef ENABLE_FRONTEND_API
	match_counter_ui_init();
#endif

	return true;
}

void obs_module_unload(void)
{
	// グローバルカウンターの解放
	match_counter_free_global();
	
	obs_log(LOG_INFO, "plugin unloaded");
}
