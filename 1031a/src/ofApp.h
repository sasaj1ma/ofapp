#pragma once

#include "ofMain.h"

// "Come on you Spurs" の 4 語を、拍に合わせてランダムな大きさで出す。
// 動かし方を 4 通り用意して、数字キーで切り替えて見比べる。
class ofApp : public ofBaseApp {
public:
  ofApp(std::string snapshotPath, int mode, float snapshotTime)
      : snapshotPath(std::move(snapshotPath)), mode(mode), snapshotTime(snapshotTime) {}

  void setup() override;
  void update() override;
  void draw() override;
  void keyPressed(int key) override;

private:
  // 作品の絵。t は秒。スナップショットでも同じ関数を通す。
  void drawScene(float t);

  void drawPop(double beat);      // 1: 4 語がぱっぱっと積み上がる
  void drawSlide(double beat);    // 2: 1 語ずつ飛び込んで、飛び去る
  void drawStretch(double beat);  // 3: 画面いっぱいの 1 語が伸び縮みする
  void drawBlink(double beat);    // 4: 4 語が点滅し、拍で白黒が反転する
  void drawFlow(double beat);     // 5: 矢印の列と 4 語が左から右へ流れる

  // 4 語を 2×2 に置く。上段に Come / on、下段に you / Spurs。
  // seed が同じなら同じ並びになる。
  struct Placement {
    glm::vec2 center;
    float size;
  };
  std::array<Placement, 4> layout(int seed) const;

  // word の中心を (x, y) に合わせ、文字の大きさ size（px）で描く。sx / sy は伸び縮み。
  void drawWord(const std::string& word, float x, float y, float size, float sx = 1, float sy = 1);
  // 文字の大きさ size のときの幅と高さ。
  glm::vec2 wordSize(const std::string& word, float size) const;
  // word の字形を取り出し、トラッキング込みの範囲を wordBoxes に測り直す。
  void measureWord(const std::string& word);

  ofTrueTypeFont font;
  // 語ごとの、輪郭から測った範囲（文字の大きさ fontSize のとき）。トラッキング込み。
  std::map<std::string, ofRectangle> wordBoxes;
  // 語ごとの字形。1 文字 1 つで、フォント本来の字送りの位置に置いてある。
  std::map<std::string, std::vector<ofPath>> wordGlyphs;
  // 語ごとのトラッキング。文字の大きさの 1/1000 単位で、文字と文字の間に足す。
  // 負で詰め、正で広げる。キーで調整し、p で出した値をここに書き写す。
  std::map<std::string, int> tracking = {{"Come", 0}, {"on", 0}, {"you", 0}, {"Spurs", 0}};
  int trackingWord = 0;  // [ ] で調整している語の番号
  // 右向きの矢印。長さ 1・高さ 1 で、左端が x = 0、先端が x = 1、上下の中心が y = 0。
  ofPath arrow;
  std::string snapshotPath;
  int mode;  // 0 は自動で 1〜5 を巡る
  float snapshotTime;
  float bpm = 120;
  bool showHud = true;
  bool inverted = false;  // drawBlink が拍で反転させる
};
