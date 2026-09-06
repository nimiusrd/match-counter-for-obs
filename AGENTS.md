# 開発ガイド

## プロジェクト構成

- OBS Studio 向けの C プラグイン。CMake と各 OS のプリセットでビルドする。
- `src/match-counter.c` と `.h`: 勝敗の管理、表示文字列の生成。
- `src/match-counter-source.c`: OBS ソース、描画、設定、ホットキー。
- `src/plugin-main.c`: プラグイン登録。`match-counter-source.c` を直接 include しているため、同ファイルを CMake の独立したコンパイル対象に重複追加しない。
- `data/locale/`: 英語・日本語の UI 文言。文言キーを追加・変更する場合は両言語を更新する。
- `buildspec.json`: プラグイン情報と固定された依存バージョン・ハッシュ。

## macOS でのセットアップとビルド

リポジトリのルートで実行する。必要なものは Xcode 16.0 以上、macOS SDK 15.0 以上、CMake 3.28 以上。詳細は README を参照する。

```bash
bash build-aux/setup-macos.sh
cmake --build --preset macos
```

- セットアップは SDKROOT と CMAKE_OSX_SYSROOT を設定し、OBS・Qt の取得と OBS 開発用ライブラリのビルドを行う。初回はネットワーク接続が必要。
- `.deps` と `build_macos` は作業ディレクトリごとの生成物。別の worktree からビルドキャッシュをコピーしない。
- 通常のソース変更後はビルドのみ実行する。CMake や依存設定の変更後はセットアップも再実行する。
- Debug ビルド: `cmake --build build_macos --config Debug`。
- 成果物: `build_macos/rundir/RelWithDebInfo/match-counter.plugin`（Debug は `rundir/Debug`）。
- Xcode のキャッシュ書き込みや依存取得がサンドボックスで拒否された場合は、許可された昇格手段で同じコマンドを実行する。

## 変更後の検証

- C/CMake の変更は対象 OS で構成・ビルドする。macOS で Windows の動作まで検証済みとは扱わない。
- `git diff --check` を実行する。ドキュメントのみの変更は不要なフルビルドを行わない。
- C の書式は `.clang-format` に従う。変更ファイルを `clang-format --dry-run --Werror <files>` で確認する。既存のフォーマッタースクリプトが要求するバージョンは 17.0.3。
- CMake の書式は `.gersemirc` に従い、変更ファイルを `gersemi --check <files>` で確認する。
- 現在、自動ユニットテストは整備されていない。テストを実行していないのに合格と報告しない。
- 表示や操作の変更は OBS でソース追加、勝敗加算、リセット、表示変数 `%w` / `%l` / `%t` / `%r`、ホットキーを確認する。実施できなかった確認は明示する。
- ビルド成功と OBS でのロード・動作成功、配布用の署名・公証は別の検証として扱う。

## 変更の扱い

- 依存アーカイブ、ビルド生成物、個人の署名情報をコミットしない。
- ルートの `.gitignore` は許可リスト方式。新しいルートファイルを追加したら追跡対象になっていることを確認する。
- 日本語で変更点と検証結果を説明する。
