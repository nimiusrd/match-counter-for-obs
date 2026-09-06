# Match Counter for OBS

## 概要

Match Counter for OBSは、対戦ゲーム配信をサポートするためのOBS Studioプラグインです。このプラグインを使用することで、配信中の勝敗カウントを簡単に管理し、表示することができます。

<img width="515" alt="プレビュー" src="https://github.com/user-attachments/assets/e32b4cd9-3dc1-4ca9-af41-9ed96ec6482b" />

主な機能:

* 勝敗のカウントと表示
* カスタマイズ可能な表示フォーマット
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

<img width="714" alt="設定画面" src="https://github.com/user-attachments/assets/0d92e853-9115-4379-9b9b-9d355e6e69c4" />

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

<img width="721" alt="ホットキーの設定画面" src="https://github.com/user-attachments/assets/d73dd1cd-aea3-4273-ab6d-058fc8a31efa" />

## ビルド方法

### 必要なもの

* CMake 3.28以上
* C/C++コンパイラ (GCC, Clang, MSVC等)
* OBS Studio開発ファイル

### ビルド手順

#### macOS

Xcode 16.0 以上（macOS 15.0 SDK 以上。Command Line Tools のみでは不可）と CMake をインストールし、Xcode の初期セットアップを完了してください。

```bash
xcodebuild -runFirstLaunch
```

リポジトリのルートで macOS 用プリセットを実行します。

```bash
export SDKROOT="$(xcrun --sdk macosx --show-sdk-path)"
cmake --preset macos -DCMAKE_OSX_SYSROOT="$SDKROOT"
cmake --build --preset macos
```

SDK のパスはプラグインと依存ライブラリの両方に明示します。初回の構成時に `buildspec.json` で指定された OBS Studio などの依存ファイルを `.deps` に取得し、OBS の開発用ライブラリをビルドします。インターネット接続と、初回ビルドのための時間・ディスク容量が必要です。

生成されるプラグインは `build_macos/rundir/RelWithDebInfo/match-counter.plugin` です。デバッグ用にビルドする場合は次のコマンドを使用します。

```bash
cmake --build build_macos --config Debug
```

#### その他の環境

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

## OBS での動作確認

### macOS のローカルビルドを導入する

1. 上記の手順でビルドし、OBS Studio を終了します。
2. 既存の `~/Library/Application Support/obs-studio/plugins/match-counter.plugin` がある場合は、復元できるようプラグインフォルダの外へ退避します。
3. リポジトリのルートで次を実行し、ローカル動作用のアドホック署名を付けて検証した後、プラグインのバンドル全体をコピーします。Debug ビルドを確認する場合は `RelWithDebInfo` を `Debug` に置き換えます。

   ```bash
   codesign --force --sign - build_macos/rundir/RelWithDebInfo/match-counter.plugin
   codesign --verify --strict --all-architectures --verbose=2 \
     build_macos/rundir/RelWithDebInfo/match-counter.plugin
   mkdir -p "$HOME/Library/Application Support/obs-studio/plugins"
   ditto build_macos/rundir/RelWithDebInfo/match-counter.plugin \
     "$HOME/Library/Application Support/obs-studio/plugins/match-counter.plugin"
   ```

4. OBS Studio を起動します。ビルドするだけでは導入済みプラグインは更新されません。変更後は OBS の終了、署名・検証、コピー、起動を繰り返します。

未署名のバンドルは `Trying to load an unsigned library` で読み込みを拒否される場合があります。このアドホック署名はローカル確認用であり、配布用の Developer ID 署名・公証とは異なります。

配置先は [OBS 公式のプラグイン導入ガイド](https://obsproject.com/kb/plugins-guide)に従います。確認後に元の版へ戻す場合も、OBS を終了してからバンドルを退避したものに戻してください。

### 確認項目と期待値

確認専用のシーンコレクションを作成し、「試合カウンター」を新規ソースとして追加します。配信を開始する必要はありません。ホットキーは「設定」→「ホットキー」で、確認用ソースの「勝利を追加」「敗北を追加」「カウンターをリセット」に、既存の操作と重複しないキーを割り当てて適用します。

| 確認 | 操作と期待値 |
| --- | --- |
| ソース追加 | ソース一覧に「試合カウンター」があり、新規追加時に `0-0(0.0%)` が描画される。 |
| 表示変数・直接入力 | プロパティの表示フォーマットを `%w勝 %l敗 / %t戦 / %r`、勝利を `3`、敗北を `1` に設定すると、`3勝 1敗 / 4戦 / 75.0%` になる。 |
| 勝利の加算 | 勝利のホットキーを一度押して離すと `4勝 1敗 / 5戦 / 80.0%` になる。プロパティを開き直して勝利が `4` であることも確認する。 |
| 敗北の加算 | 敗北のホットキーを一度押して離すと `4勝 2敗 / 6戦 / 66.7%` になる。プロパティを開き直して敗北が `2` であることも確認する。 |
| リセット | リセットのホットキーで `0勝 0敗 / 0戦 / 0.0%` になり、プロパティの勝利・敗北も `0` になる。 |
| フォント・複数行 | フォントとサイズを変更し、表示フォーマットに改行を入れる。プレビューとシーン上の表示に反映され、文字が欠けないことを確認する。 |
| 保存・再起動 | 勝利 `3`・敗北 `1` を設定してプロパティを確定し、OBS を正常終了して再起動する。勝敗、表示フォーマット、フォント、ホットキーが保持されることを確認する。 |

### ロードとログの確認

OBS の「ヘルプ」→「ログファイル」から現在のログを確認します。macOS のログ保存先は `~/Library/Application Support/obs-studio/logs/` です。

* `[match-counter] plugin loaded successfully (version ...)` があることを確認します。過去の起動ログと取り違えないようにしてください。
* ソースが追加できない場合は、`match-counter` に関するロード失敗や依存ライブラリのエラーを確認します。
* ソースが追加できても描画されない場合は、`Failed to create text source` などを確認します。macOS では OBS の FreeType 2 テキストソースを内部で使用しています。

検証結果には OS・OBS のバージョン、対象コミット、ビルド構成、各項目の成否と未実施項目を記録します。ビルド成功、OBS でのロード成功、表示・操作の成功、配布用の署名・公証はそれぞれ別の確認です。

## ライセンス

このプラグインはGPLv2ライセンスの下で公開されています。詳細はLICENSEファイルを参照してください。
