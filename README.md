# ofapp

openFrameworks の作品を溜めていく置き場です。1 作品 = 1 フォルダで、どれも同じ形をしています。

## はじめの準備（一度だけ）

1. openFrameworks 0.12.1 の macOS 版を `~/Developer/of_v0.12.1_osx_release` に置く。別の場所に置いたときは、各作品の `config.make` の `OF_ROOT` を書き換える。
2. Xcode は要らない。コマンドラインツール（`xcode-select --install`）だけでビルドできる。

## 新しい作品を作る

```bash
./new.sh 作品名
```

アドオンを使うときは、名前のあとに並べます。`addons.make` に書き込まれます。

```bash
./new.sh flowfield ofxGui ofxOsc
```

`_template/` をコピーして、名前を差し込むだけです。絵は `src/ofApp.cpp` の `drawScene(t)` に書きます。

## ビルドと実行

```bash
cd 作品名 && make Release -j8 && make RunRelease
```

- `s` キーで、そのときの画面を `bin/data/screenshot-日時.png` に保存する。
- `--snapshot out.png` を付けて起動すると、t = 1 秒の 1 フレームを PNG に書いて終わる。見た目の確認に使う。

```bash
作品名/bin/作品名.app/Contents/MacOS/作品名 --snapshot out.png
```

## フォルダ

- `_template/` — 新しい作品の雛形。`new.sh` がここからコピーする
- `new.sh` — 作品フォルダを作るスクリプト

`bin/`（`bin/data/` は除く）と `obj/` はビルドで作られるので git に入れません。素材は `bin/data/` に置きます。
