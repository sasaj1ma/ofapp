#!/bin/bash
# 新しい作品のフォルダを _template から作る。
#   ./new.sh 作品名 [アドオン ...]
#   ./new.sh flowfield ofxGui ofxOsc
set -euo pipefail

cd "$(dirname "$0")"
name="${1:-}"
if [[ -z "$name" ]]; then
  echo "使い方: ./new.sh 作品名 [アドオン ...]" >&2
  exit 1
fi
if [[ -e "$name" ]]; then
  echo "$name はもうある" >&2
  exit 1
fi

cp -R _template "$name"
sed -i '' "s/__NAME__/$name/g" "$name/src/main.cpp" "$name/src/ofApp.cpp"
shift
for addon in "$@"; do echo "$addon" >> "$name/addons.make"; done

echo "$name を作った。ビルドと実行:"
echo "  cd $name && make Release -j8 && make RunRelease"
