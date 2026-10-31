#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
  explicit ofApp(std::string snapshotPath) : snapshotPath(std::move(snapshotPath)) {}

  void setup() override;
  void update() override;
  void draw() override;
  void keyPressed(int key) override;

private:
  // 作品の絵。t は秒。スナップショットでも同じ関数を通す。
  void drawScene(float t);

  // 空でなければ、t = 1 秒の 1 フレームを PNG に書いて終わる。見た目の確認用。
  std::string snapshotPath;
};
