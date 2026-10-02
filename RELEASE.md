# リリース手順

メンテナー向けに、バージョン更新から GitHub Release の公開までをまとめます。基本の流れは、変更と検証を `main` に取り込み、リリースタグを push し、CI が作成したドラフトを確認して手動で公開することです。

CI の定義は [push.yaml](.github/workflows/push.yaml) と [build-project.yaml](.github/workflows/build-project.yaml)、導入方法と OBS の確認項目は [README](README.md#obs-での動作確認) を参照してください。

## 1 バージョンを更新して検証する

1. リリースする変更を揃え、`buildspec.json` のトップレベルの `version` を更新する。依存ライブラリの `dependencies.*.version` はプラグインのバージョンとは別に扱う。
2. バージョン更新を含めた変更を PR で確認し、`main` に取り込む。
3. 対象コミットの書式チェック、各 OS のビルドと CTest の成功を確認する。現在の CI は macOS、Windows x64、Ubuntu 24.04 x86_64 を対象とする。タグの push では書式チェックが実行されないため、PR または `main` のチェック結果を確認する。
4. 配布対象の OS とアーキテクチャで、[README の確認項目](README.md#確認項目と期待値)に沿って OBS のロード、表示、勝敗加算、リセット、ホットキー、保存と再起動を確認する。
5. リリース本文に記載する変更点、互換性への影響、既知の問題、確認結果を用意する。確認結果には OS・アーキテクチャ・OBS のバージョン・対象コミットと未実施項目を残す。

CI の成功は OBS での動作成功を保証しません。macOS の確認だけで Windows や Ubuntu、Intel Mac の動作まで検証済みとは扱わず、配布対象で未確認の項目がある場合は公開前に扱いを決めて本文に明記します。

### バージョンとタグの対応

| 種類 | `buildspec.json` の `version` | タグの例 | Release の扱い |
| --- | --- | --- | --- |
| 正式版 | `1.2.3` | `1.2.3` | 通常のドラフト |
| ベータ版 | `1.2.3` | `1.2.3-beta1` | プレリリースのドラフト |
| リリース候補版 | `1.2.3` | `1.2.3-rc1` | プレリリースのドラフト |

運用では上記の形式を使います。`v1.2.3` や `1.2.3-alpha1` ではドラフト Release が作成されません。`buildspec.json` の `version` は CMake の `project(... VERSION ...)` に渡すため、候補版でも数値の `X.Y.Z` とし、`-beta1` や `-rc1` はタグだけに付けます。

CI はタグと `buildspec.json` のバージョン一致を検査しません。正式版では完全一致、候補版ではタグの数値部分が一致することを確認してください。配布ファイル名とプラグイン内のバージョンは `buildspec.json` に由来するため、ベータ版・候補版・正式版で同じファイル名になる場合があります。必ずタグと対象コミットも確認します。

## 2 macOS の署名と公証を準備する

タグを push する前に、リポジトリの Actions Secrets に次の情報が設定されていることと、証明書が有効であることを確認します。秘密値や証明書ファイルはリポジトリに追加しません。

| Secret | 用途 |
| --- | --- |
| `MACOS_SIGNING_APPLICATION_IDENTITY` | プラグインの Developer ID Application 署名 ID。チーム ID を含む正式な名前を指定する。 |
| `MACOS_SIGNING_INSTALLER_IDENTITY` | PKG の Developer ID Installer 署名 ID。 |
| `MACOS_SIGNING_CERT` | 署名に使う証明書と秘密鍵を含む PKCS#12 の Base64。Application と Installer の両方の署名に必要な情報を含める。 |
| `MACOS_SIGNING_CERT_PASSWORD` | PKCS#12 のインポート用パスワード。 |
| `MACOS_NOTARIZATION_USERNAME` | 公証に使う Apple ID。 |
| `MACOS_NOTARIZATION_PASSWORD` | 公証に使う App 用パスワード。 |

`MACOS_KEYCHAIN_PASSWORD` と `MACOS_SIGNING_PROVISIONING_PROFILE` も参照されますが、省略可能です。キーチェーンのパスワードは省略時に CI が生成します。

署名 ID や証明書が不足するとパッケージ署名・公証を、公証用情報が不足すると公証をスキップする実装です。CI が成功しても署名・公証が実施されたとは限らないため、後述の成果物確認を行います。ローカル確認用のアドホック署名は配布用の署名として扱いません。

## 3 対象コミットにタグを付ける

リポジトリのルートで `main` を最新にし、作業ツリーが空で、検証したコミットと `HEAD` が一致することを確認します。

```bash
git switch main
git pull --ff-only origin main
git status --short
git log -1 --oneline
git show HEAD:buildspec.json
```

以下の `1.2.3` は実際のリリース番号に置き換えます。候補版では `1.2.3-rc1` などのタグを指定します。ローカルとリモートの両方に同名タグが存在しないことを確認してから、検証した `HEAD` に注釈付きタグを作成します。

```bash
release_tag=1.2.3
git tag --list "$release_tag"
git ls-remote --tags origin "refs/tags/$release_tag" "refs/tags/$release_tag^{}"
```

どちらも出力がなければタグを作成して push します。タグの push により CI のビルドとドラフト作成が開始します。

```bash
git tag -a "$release_tag" -m "Release $release_tag"
git push origin "refs/tags/$release_tag"
```

タグは対象コミットの記録として保持します。公開済みタグの付け替えや強制 push は行わず、ソースを修正した場合は新しいバージョンとタグでリリースします。

## 4 タグの CI とドラフトを確認する

[Actions](https://github.com/nimiusrd/match-counter-for-obs/actions) で、対象タグの `Push` 実行を開きます。対象コミット、各 OS のビルド・CTest・パッケージ生成、`Create Release` の成功を確認します。正式版・候補版のタグでは `Release` 構成を使用し、macOS の公証も設定に応じて実行します。

イベントによる違いは次のとおりです。

| 起動方法 | ビルド構成 | 成果物と Release |
| --- | --- | --- |
| 通常の PR | `RelWithDebInfo` | 確認用の成果物。ドラフト作成なし。 |
| `main` または通常の `release/**` ブランチの push | `RelWithDebInfo` | パッケージを生成。ドラフト作成なし。 |
| 正式版・候補版のタグの push | `Release` | パッケージを生成し、全 OS のビルド成功後にドラフト作成。 |
| `Dispatch` の `build` | `RelWithDebInfo` | 確認用ビルド。ドラフト作成なし。タグの配布パッケージとは形式が異なる。 |

`Seeking Testers` ラベル付き PR は署名とパッケージ生成も要求します。Release 構成の判定は ref 名に対して行われるため、ブランチ名にバージョン形式を含めた場合は Actions の実際の設定も確認してください。

[Releases](https://github.com/nimiusrd/match-counter-for-obs/releases) の対象ドラフトを開き、タグとコミット、以下の添付ファイルを確認します。`<version>` は `buildspec.json` の数値バージョンです。

| 用途 | タグビルドで生成するファイル |
| --- | --- |
| Windows x64 | `match-counter-<version>-windows-x64.zip` |
| macOS Universal | `match-counter-<version>-macos-universal.pkg` |
| Ubuntu 24.04 x86_64 | `match-counter-<version>-x86_64-linux-gnu.deb` と `match-counter-<version>-x86_64-linux-gnu-dbgsym.ddeb` |
| ソース | `match-counter-<version>-source.tar.xz` |

Windows の EXE は現在生成しません。macOS の `match-counter-<version>-macos-universal-dSYMs.tar.xz` は別の Actions artifact として保存され、Release には自動添付されません。調査用に必要な場合は Actions から保存します。

### 配布ファイルの確認

- ドラフトから取得したファイルの SHA-256 を本文の `Checksums` と照合する。macOS では `shasum -a 256 <ファイル>`、Windows PowerShell では `Get-FileHash <ファイル> -Algorithm SHA256` を使う。`CHECKSUMS.txt` は添付されず、値は本文に記載される。
- Windows ZIP に DLL と英語・日本語のロケールが含まれ、[README の導入手順](README.md#インストール方法)で配置できることを確認する。
- macOS の Actions ログで Developer ID 署名、公証の `Accepted`、staple の成功を確認し、ダウンロードした PKG に対して次のコマンドが成功することを確認する。署名者が意図した Developer ID Installer であることも確認する。

  ```bash
  pkgutil --check-signature match-counter-1.2.3-macos-universal.pkg
  xcrun stapler validate match-counter-1.2.3-macos-universal.pkg
  ```

- 配布パッケージを導入して OBS のロードと操作を確認する。ローカル検証用の別版が残っていない状態で行い、macOS の導入されたバンドルには `codesign --verify --strict --all-architectures --verbose=2 <バンドルのパス>` で署名検証も行う。
- macOS は arm64 と x86_64 の Universal バイナリを生成するが、各アーキテクチャの OBS 動作確認結果は別に記録する。Ubuntu も CI 対象というだけで OBS 動作検証済みとは扱わない。

## 5 本文を整えて公開する

ドラフト本文には自動生成された `Checksums` だけが入ります。その部分を残し、変更点、互換性、確認結果、既知の問題を追記します。本文の構成例は次のとおりです。

```markdown
## 変更点

- 利用者に影響する変更を記載する。

## 互換性と動作確認

- OS、アーキテクチャ、OBS のバージョン、確認結果と未実施項目を記載する。
- 更新時に必要な操作があれば記載する。

## 既知の問題

- 制限事項や回避策を記載する。
```

公開直前に次を確認し、ドラフトの公開操作を行います。

- [ ] タグ、対象コミット、`buildspec.json`、配布ファイルのバージョンが対応している。
- [ ] 全 OS の CI が成功し、必要な添付ファイルと SHA-256 が揃っている。
- [ ] macOS の配布用署名と公証を確認した。
- [ ] 配布ファイルを使った OBS の確認結果と未実施項目を本文に記載した。
- [ ] ベータ版・候補版ではプレリリース指定を維持し、正式版では外した。正式版を Latest として公開するかも確認した。
- [ ] README のダウンロード名・導入方法が配布物と一致している。

公開後は Release ページのタグ、本文、添付ファイルとダウンロードを確認します。正式版では README のリリースページから目的の版へ到達できることも確認します。

## 失敗時の対応

GitHub Actions の再実行は[元の実行から 30 日以内](https://docs.github.com/en/actions/how-tos/manage-workflow-runs/re-run-workflows-and-jobs)で、同じコミットと ref を使用します。期限を過ぎて再実行できない場合は、新しいバージョンとタグで検証・生成し直します。`Create Release` の再実行は未公開のドラフトを対象とし、公開済み Release に対しては行いません。

- **ビルド・CTest・署名・公証が失敗した場合**: 対象タグの `Push` ログから原因を確認する。Secrets や一時的な障害の対応でソース変更が不要なら、その実行の失敗ジョブを再実行する。ソース変更が必要なら、新しいコミットとバージョンで検証し、新しいタグを作る。
- **ビルド成功後にドラフト作成が失敗した場合**: 同じタグの `Push` 実行で `Create Release` を再実行する。`Dispatch` は Release を作成しないため代替にならない。成果物が失効している場合は、再実行期限内ならビルドから再実行する。
- **ドラフトが作成されない場合**: タグの形式、各 OS のビルド結果、`Create Release` の実行結果を確認する。タグ形式が不適切な場合は規定の形式で新しいタグを作る。
- **ドラフト作成を再実行した場合**: 添付物とチェックサムを再確認し、手動追記した本文も確認する。再実行前に追記内容を控えておく。
- **公開後に問題が見つかった場合**: 既知の問題と回避策を本文に追記し、修正版を新しいバージョンで公開する。
