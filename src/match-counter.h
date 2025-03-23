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

#pragma once

#include <obs-module.h>
#include <util/bmem.h>
#include <util/darray.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 試合結果の構造体
 */
typedef struct match_counter {
    int wins;           // 勝利数
    int losses;         // 敗北数
    char *format;       // 表示フォーマット
    char *player_name;  // プレイヤー名
} match_counter_t;

/**
 * 試合カウンターを初期化する
 * @return 初期化された試合カウンター
 */
match_counter_t *match_counter_create(void);

/**
 * 試合カウンターを破棄する
 * @param counter 破棄する試合カウンター
 */
void match_counter_destroy(match_counter_t *counter);

/**
 * 勝利数を増やす
 * @param counter 試合カウンター
 */
void match_counter_add_win(match_counter_t *counter);

/**
 * 敗北数を増やす
 * @param counter 試合カウンター
 */
void match_counter_add_loss(match_counter_t *counter);

/**
 * 勝利数を減らす
 * @param counter 試合カウンター
 */
void match_counter_subtract_win(match_counter_t *counter);

/**
 * 敗北数を減らす
 * @param counter 試合カウンター
 */
void match_counter_subtract_loss(match_counter_t *counter);

/**
 * 勝敗をリセットする
 * @param counter 試合カウンター
 */
void match_counter_reset(match_counter_t *counter);

/**
 * 表示フォーマットを設定する
 * @param counter 試合カウンター
 * @param format フォーマット文字列
 * 
 * フォーマット文字列では以下の変数が使用可能:
 * %w - 勝利数
 * %l - 敗北数
 * %n - プレイヤー名
 */
void match_counter_set_format(match_counter_t *counter, const char *format);

/**
 * プレイヤー名を設定する
 * @param counter 試合カウンター
 * @param name プレイヤー名
 */
void match_counter_set_player_name(match_counter_t *counter, const char *name);

/**
 * フォーマットされた文字列を取得する
 * @param counter 試合カウンター
 * @return フォーマットされた文字列（呼び出し側で解放する必要あり）
 */
char *match_counter_get_formatted_text(match_counter_t *counter);

/**
 * グローバルな試合カウンターを取得する
 * @return グローバルな試合カウンター
 */
match_counter_t *match_counter_get_global(void);

#ifdef __cplusplus
}
#endif
