# Match Counter for OBS

## 概要

Match Counter for OBSは、対戦ゲーム配信をサポートするためのOBS Studioプラグインです。このプラグインを使用することで、配信中の勝敗カウントを簡単に管理し、表示することができます。

主な機能:

* 勝敗のカウントと表示
* カスタマイズ可能な表示フォーマット
* 専用の操作画面からの簡単な勝敗入力
* ホットキーによる素早いカウント操作

## インストール方法

### Windows

1. [リリースページ](https://github.com/nimiusrd/match-counter-for-obs/releases)から`match-counter-x.x.x-windows-x64.zip`をダウンロードします
2. ダウンロードしたファイルを解凍します
3. 解凍したフォルダ内のファイルを以下の場所に配置します:
   * `match-counter\bin\64bit\match-counter.dll` → `C:\Program Files\obs-studio\obs-plugins\64bit\match-counter.dll`
   * `match-counter\bin\64bit\match-counter.pdb` → `C:\Program Files\obs-studio\obs-plugins\64bit\match-counter.pdb`
   * `match-counter\data\locale\en-US.ini` → `C:\Program Files\obs-studio\data\obs-plugins\match-counter\locale\en-US.ini`
   * `match-counter\data\locale\ja-JP.ini` → `C:\Program Files\obs-studio\data\obs-plugins\match-counter\locale\ja-JP.ini`
4. OBS Studioを再起動します

### macOS

1. [リリースページ](https://github.com/nimiusrd/match-counter-for-obs/releases)から`match-counter-x.x.x-macos-universal.pkg`をダウンロードします
2. ダウンロードしたファイルを実行し、インストーラーの指示に従ってインストールします
3. OBS Studioを再起動します

## 使い方

### 勝敗カウンターの追加

1. OBS Studioを起動します
2. ソースリストの「+」ボタンをクリックします
3. 「試合カウンター」を選択します
4. 名前を入力して「OK」をクリックします
5. 設定画面で表示フォーマットを設定します

### 専用画面からの操作

1. OBS Studioのメニューバーから「ツール」→「試合カウンター」を選択します
2. 表示された画面から以下の操作が可能です:
   * 表示フォーマットの変更
   * 勝利/敗北の追加・減少
   * カウンターのリセット

### 表示フォーマットのカスタマイズ

以下の変数を使用して表示フォーマットをカスタマイズできます:
* `%w` - 勝利数
* `%l` - 敗北数
* `%t` - 総試合数（勝利数+敗北数）
* `%r` - 勝率（パーセント表示、例: 75.0%）

例:
* `%w勝 %l敗` → 「3勝 1敗」
* `%w-%l` → 「3-1」
* `%w/%l (勝率: %r)` → 「3/1 (勝率: 75.0%)」
* `%t戦%w勝`　→　「4戦1勝」

## ホットキーの設定

1. OBS Studioの「設定」→「ホットキー」を開きます
2. 「試合カウンター」セクションで以下のホットキーを設定できます:
   * 勝利を追加
   * 敗北を追加
   * カウンターをリセット

## ビルド方法

### 必要なもの

* CMake 3.28以上
* C/C++コンパイラ (GCC, Clang, MSVC等)
* OBS Studio開発ファイル
* Qt6開発ファイル (6.0以上)

### ビルド手順

リポジトリをクローン
```bash
git clone https://github.com/nimiusrd/match-counter-for-obs.git
```
CMakeを実行
```bash
mkdir build && cd build
cmake ..
```
ビルド
```bash
cmake --build .
```

## ライセンス

このプラグインはGPLv2ライセンスの下で公開されています。詳細はLICENSEファイルを参照してください。
