# ofapp

openFrameworks 0.12.1（macOS、`~/Developer/of_v0.12.1_osx_release`）の作品集。1 作品 = 1 フォルダ。

- 新しい作品は `./new.sh 名前 [アドオン ...]` で `_template/` から作る。手でフォルダを複製しない。
- ビルドは各作品フォルダで `make Release -j8`。Xcode プロジェクトは使わない。
- 絵は `ofApp::drawScene(float t)` に書く。見た目を確かめるときは `bin/名前.app/Contents/MacOS/名前 --snapshot 出力.png` で t = 1 秒の PNG を書き出して読む。
- `plasma/` と `shared/` は LED パネル向けの特殊な作品。ほかの作品は `shared/` を使わない。
- コメントと README は日本語。
