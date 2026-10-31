#include "ofApp.h"

namespace {

const std::array<std::string, 4> words = {"Come", "on", "you", "Spurs"};
// フォントはこの大きさで読み、描くときに拡大縮小する。輪郭で描くので拡大してもぼけない。
constexpr float fontSize = 200;

// 拍の番号 step と用途 salt から、毎回同じ 0〜1 の乱数を返す。
// スナップショットでも同じ絵になるように、ofRandom は使わない。
float rnd(int step, int salt) {
  std::mt19937 gen(static_cast<uint32_t>(step * 7919 + salt * 104729 + 1));
  return std::uniform_real_distribution<float>(0, 1)(gen);
}

double fract(double x) { return x - std::floor(x); }

float easeOutExpo(float p) { return p >= 1 ? 1 : 1 - std::pow(2.0f, -10 * p); }
float easeInExpo(float p) { return p <= 0 ? 0 : std::pow(2.0f, 10 * p - 10); }

}  // namespace

void ofApp::setup() {
  ofSetFrameRate(60);
  ofSetWindowTitle("1031a");
  // 第 5 引数で輪郭を作る。getStringAsPoints で字形を取り出すのに要る。
  if (!font.load("Helvetica", fontSize, true, true, true)) ofLogError() << "Helvetica を読めない";

  for (const auto& word : words) measureWord(word);

  // 非常口の誘導灯の矢印。太い「く」の字を 2 本の腕で作り、その付け根から軸を伸ばす。
  // 腕と軸は同じくらいの太さで、腕の端は水平に切る。
  const float shaft = 0.3f;        // 軸の太さ
  const float arm = 0.3f * 1.414f;  // 腕の太さ（45 度なので横方向には √2 倍になる）
  const float joint = 1 - arm - shaft / 2;  // 腕の内側と軸の上下の辺が交わる x
  arrow.moveTo(1, 0);
  arrow.lineTo(0.5f, -0.5f);
  arrow.lineTo(0.5f - arm, -0.5f);
  arrow.lineTo(joint, -shaft / 2);
  arrow.lineTo(0, -shaft / 2);
  arrow.lineTo(0, shaft / 2);
  arrow.lineTo(joint, shaft / 2);
  arrow.lineTo(0.5f - arm, 0.5f);
  arrow.lineTo(0.5f, 0.5f);
  arrow.close();
  arrow.setFillColor(ofColor(145));
}

void ofApp::update() {}

void ofApp::draw() {
  if (!snapshotPath.empty()) {
    showHud = false;
    drawScene(snapshotTime);
    ofSaveScreen(snapshotPath);
    ofExit();
    return;
  }
  drawScene(ofGetElapsedTimef());
}

void ofApp::keyPressed(int key) {
  // s で今の画面を bin/data/ に保存する。
  if (key == 's') ofSaveScreen("screenshot-" + ofGetTimestampString() + ".png");
  if (key >= '0' && key <= '5') mode = key - '0';
  if (key == OF_KEY_UP) bpm = std::min(bpm + 10, 240.0f);
  if (key == OF_KEY_DOWN) bpm = std::max(bpm - 10, 40.0f);
  if (key == 'h') showHud = !showHud;

  // トラッキング。t で語を選び、[ ] で 10、{ } で 1 ずつ動かす。p で今の値を書き出す。
  const std::string& word = words[trackingWord];
  if (key == 't') trackingWord = (trackingWord + 1) % 4;
  if (key == '[' || key == ']' || key == '{' || key == '}') {
    const int step = key == '[' ? -10 : key == ']' ? 10 : key == '{' ? -1 : 1;
    tracking[word] += step;
    measureWord(word);
  }
  if (key == 'p') {
    std::string line = "  std::map<std::string, int> tracking = {";
    for (int i = 0; i < 4; i++) {
      line += (i ? ", " : "") + std::string("{\"") + words[i] + "\", " + ofToString(tracking[words[i]]) + "}";
    }
    std::cout << line << "};" << std::endl;
  }
}

void ofApp::drawScene(float t) {
  const double beat = t * bpm / 60.0;
  // 自動のときは 8 拍ごとに次のモードへ。
  const int current = mode == 0 ? static_cast<int>(beat / 8) % 5 + 1 : mode;

  inverted = false;
  if (current == 4) {
    // 拍の頭の 1/8 だけ、3 回に 1 回くらい白黒を反転する。
    inverted = fract(beat) < 0.125 && rnd(static_cast<int>(beat), 9) < 0.35;
  }
  ofBackground(inverted ? 0 : 255);
  ofSetColor(inverted ? 255 : 0);

  if (current == 1) drawPop(beat);
  if (current == 2) drawSlide(beat);
  if (current == 3) drawStretch(beat);
  if (current == 4) drawBlink(beat);
  if (current == 5) drawFlow(beat);

  if (showHud) {
    // ビットマップ文字は拡大されないので、Retina では倍率をかけて読める大きさにする。
    const float scale = dynamic_cast<ofAppGLFWWindow*>(ofGetWindowPtr())->getPixelScreenCoordScale();
    ofPushMatrix();
    ofScale(scale);
    ofSetColor(150);
    ofDrawBitmapString("mode " + ofToString(current) + (mode == 0 ? " (auto)" : "") + "   " +
                           ofToString(bpm, 0) + " bpm   [0-5] mode  [up/down] bpm  [h] hide",
                       12, ofGetHeight() / scale - 12);
    std::string line = "tracking";
    for (int i = 0; i < 4; i++) {
      const std::string item = words[i] + " " + ofToString(tracking[words[i]]);
      line += "  " + (i == trackingWord ? "[" + item + "]" : item);
    }
    ofDrawBitmapString(line + "   [t] word  [ ] +-10  { } +-1  [p] print", 12, ofGetHeight() / scale - 30);
    ofPopMatrix();
  }
}

// 1 拍に 1 語ずつ出し、4 語そろったら 1 拍止めて消す。5 拍で 1 周。
void ofApp::drawPop(double beat) {
  const int cycle = static_cast<int>(beat / 5);
  const double inCycle = beat - cycle * 5;
  const auto places = layout(cycle);
  for (int i = 0; i < 4; i++) {
    if (inCycle < i) break;
    // 出た瞬間だけ少し大きく、すぐ元に戻す。
    const float p = static_cast<float>(inCycle - i);
    const float pop = p < 0.12f ? ofMap(p, 0, 0.12f, 1.15f, 1.0f) : 1.0f;
    drawWord(words[i], places[i].center.x, places[i].center.y, places[i].size * pop);
  }
}

// 1 拍に 1 語。画面の外から飛び込み、少し止まって、同じ向きへ飛び去る。
void ofApp::drawSlide(double beat) {
  const int step = static_cast<int>(beat);
  const float p = static_cast<float>(fract(beat));
  const std::string& word = words[step % 4];
  const float w = ofGetWidth();
  const float h = ofGetHeight();

  const float size = ofLerp(h * 0.25f, h * 0.9f, rnd(step, 1));
  const glm::vec2 box = wordSize(word, size);
  // 止まる位置。文字がはみ出さない範囲で選ぶ。
  const glm::vec2 rest(ofLerp(box.x / 2, std::max(box.x / 2, w - box.x / 2), rnd(step, 2)),
                       ofLerp(box.y / 2, std::max(box.y / 2, h - box.y / 2), rnd(step, 3)));
  // 0:左から 1:右から 2:上から 3:下から
  const int dir = static_cast<int>(rnd(step, 4) * 4);
  const glm::vec2 travel = dir == 0 ? glm::vec2(w + box.x, 0)
                           : dir == 1 ? glm::vec2(-w - box.x, 0)
                           : dir == 2 ? glm::vec2(0, h + box.y)
                                      : glm::vec2(0, -h - box.y);

  glm::vec2 pos = rest;
  if (p < 0.35f) pos = rest - travel * (1 - easeOutExpo(p / 0.35f));
  if (p > 0.75f) pos = rest + travel * easeInExpo((p - 0.75f) / 0.25f);
  drawWord(word, pos.x, pos.y, size);
}

// 1 拍に 1 語を画面いっぱいに出す。出た瞬間に縦へ伸び、揺れながら落ち着く。
void ofApp::drawStretch(double beat) {
  const int step = static_cast<int>(beat);
  const float p = static_cast<float>(fract(beat));
  const std::string& word = words[step % 4];

  // 横幅が画面の 85% になる大きさ。高さが収まらなければ高さに合わせる。
  const glm::vec2 unit = wordSize(word, 1);
  const float size = std::min(ofGetWidth() * 0.85f / unit.x, ofGetHeight() * 0.6f / unit.y);

  // 減衰する揺れ。k は 1 から始まり、0 のまわりを揺れて 0 に落ち着く。
  const float k = std::exp(-5 * p) * std::cos(p * 22);
  // 縦に伸ばすか、横に伸ばすかは拍ごとに選ぶ。
  const bool tall = rnd(step, 5) < 0.6f;
  const float sx = tall ? 1 - 0.45f * k : 1 + 0.35f * k;
  const float sy = tall ? 1 + 0.9f * k : 1 - 0.6f * k;
  drawWord(word, ofGetWidth() / 2, ofGetHeight() / 2, size, sx, sy);
}

// drawPop の並びのまま 4 語を出し、語ごとにばらばらに点滅させる。並びは 2 拍ごとに変わる。
void ofApp::drawBlink(double beat) {
  const auto places = layout(static_cast<int>(beat / 2) + 1000);
  // 1 拍を 8 つに刻み、刻みごとに語ごとの表示・非表示を選ぶ。
  const int tick = static_cast<int>(beat * 8);
  for (int i = 0; i < 4; i++) {
    if (rnd(tick, i) < 0.35f) continue;
    drawWord(words[i], places[i].center.x, places[i].center.y, places[i].size);
  }
}

// 画面を横長のレーンに分け、レーンごとに矢印と語を並べた帯を右へ流す。
// 帯は画面より長く、右へ抜けた分が左から戻ってくる。拍で刻まず、時間に沿ってなめらかに動かす。
void ofApp::drawFlow(double beat) {
  const float w = ofGetWidth();
  const float h = ofGetHeight();
  constexpr int lanes = 6;
  const float laneH = h / (lanes + 0.5f);
  const float arrowSize = laneH * 0.78f;

  // 語を入れるレーン。上から on / Come / you / Spurs の順にばらす。
  constexpr std::array<int, 4> wordLane = {1, 0, 2, 3};

  // 帯に並べるもの。word が -1 なら矢印。
  struct Item {
    float x;
    int word;
    float size;
  };

  for (int pass = 0; pass < 2; pass++) {  // 1 周目で矢印、2 周目で語を描き、語を上に重ねる
    for (int lane = 0; lane < lanes; lane++) {
      const float y = laneH * (lane + 0.75f);
      const float length = w * ofLerp(1.4f, 2.0f, rnd(lane, 50));
      const float speed = w * ofLerp(0.08f, 0.16f, rnd(lane, 51));  // 1 拍に進む距離

      // 帯の中身を作る。矢印をほぼ隙間なく並べ、ときどき間を空ける。
      std::vector<Item> items;
      float x = 0;
      int n = 0;
      int wordHere = -1;
      for (int i = 0; i < 4; i++) {
        if (wordLane[i] == lane) wordHere = i;
      }
      // 語は帯の途中の、毎回同じ場所に入れる。
      const float wordAt = length * ofLerp(0.2f, 0.6f, rnd(lane, 52));
      bool wordPlaced = wordHere < 0;
      while (x < length - arrowSize) {
        if (!wordPlaced && x >= wordAt) {
          const float size = arrowSize * ofLerp(0.8f, 2.6f, rnd(lane, 53));
          const float width = wordSize(words[wordHere], size).x;
          items.push_back({x + arrowSize * 0.2f, wordHere, size});
          x += width + arrowSize * 0.4f;
          wordPlaced = true;
          continue;
        }
        items.push_back({x, -1, arrowSize});
        x += arrowSize * 1.08f;
        // 5 本に 1 本くらい、矢印 2〜4 本分の間を空ける。
        if (rnd(lane * 100 + n, 54) < 0.18f) x += arrowSize * ofLerp(1.5f, 4.0f, rnd(lane * 100 + n, 55));
        n++;
      }

      // 流れた距離を帯の長さで折り返し、画面を覆うだけ帯を並べる。
      const float offset = static_cast<float>(std::fmod(beat * speed, static_cast<double>(length)));
      for (float start = offset - length; start < w; start += length) {
        for (const auto& item : items) {
          const float left = start + item.x;
          if (item.word < 0) {
            if (pass != 0 || left > w || left + item.size < 0) continue;
            ofPushMatrix();
            ofTranslate(left, y);
            ofScale(item.size);
            arrow.draw();
            ofPopMatrix();
          } else if (pass == 1) {
            const glm::vec2 box = wordSize(words[item.word], item.size);
            if (left > w || left + box.x < 0) continue;
            ofSetColor(0);
            drawWord(words[item.word], left + box.x / 2, y, item.size);
          }
        }
      }
    }
  }
}

std::array<ofApp::Placement, 4> ofApp::layout(int seed) const {
  const float w = ofGetWidth();
  const float h = ofGetHeight();
  const float margin = h * 0.05f;
  const float rowH = (h - margin * 2) / 2;

  std::array<Placement, 4> places;
  for (int row = 0; row < 2; row++) {
    // 段ごとに左右の境目をずらし、片方を大きく、片方を小さくする。
    const float split = ofLerp(0.3f, 0.7f, rnd(seed, 10 + row));
    const float cellW[2] = {(w - margin * 2) * split, (w - margin * 2) * (1 - split)};
    const float cellX[2] = {margin, margin + cellW[0]};
    const float top = margin + rowH * row;

    for (int col = 0; col < 2; col++) {
      const int i = row * 2 + col;
      // 枠に収まる最大の大きさから、ランダムに縮める。
      const glm::vec2 unit = wordSize(words[i], 1);
      const float fit = std::min(cellW[col] * 0.92f / unit.x, rowH * 0.92f / unit.y);
      const float size = fit * ofLerp(0.45f, 1.0f, rnd(seed, 20 + i));
      const glm::vec2 box = unit * size;
      // 枠の中で、はみ出さない範囲に置く。
      const float x = cellX[col] + box.x / 2 + (cellW[col] - box.x) * rnd(seed, 30 + i);
      const float y = top + box.y / 2 + (rowH - box.y) * rnd(seed, 40 + i);
      places[i] = {{x, y}, size};
    }
  }
  return places;
}

void ofApp::drawWord(const std::string& word, float x, float y, float size, float sx, float sy) {
  const ofRectangle& box = wordBoxes.at(word);
  const float s = size / fontSize;
  const float track = tracking.at(word) * fontSize / 1000;
  ofPushMatrix();
  ofTranslate(x, y);
  ofScale(s * sx, s * sy);
  ofTranslate(-box.x - box.width / 2, -box.y - box.height / 2);
  const auto& glyphs = wordGlyphs.at(word);
  for (size_t i = 0; i < glyphs.size(); i++) {
    // i 文字目は、本来の位置からトラッキング i 回分ずらす。
    ofPushMatrix();
    ofTranslate(track * i, 0);
    glyphs[i].draw();
    ofPopMatrix();
  }
  ofPopMatrix();
}

void ofApp::measureWord(const std::string& word) {
  auto& glyphs = wordGlyphs[word];
  if (glyphs.empty()) {
    glyphs = font.getStringAsPoints(word);
    // 字形の色ではなく、ofSetColor の色で塗る。
    for (auto& glyph : glyphs) glyph.setUseShapeColor(false);
  }

  // getStringBoundingBox は字の枠で測るので、"you" のように実際の形より背が高く出る。
  // 輪郭の点から測り直し、見た目どおりの範囲で並べる。
  const float track = tracking[word] * fontSize / 1000;
  ofRectangle box;
  bool first = true;
  for (size_t i = 0; i < glyphs.size(); i++) {
    for (const auto& command : glyphs[i].getCommands()) {
      if (command.type == ofPath::Command::close) continue;
      const float px = command.to.x + track * i;
      if (first) box.set(px, command.to.y, 0, 0);
      box.growToInclude(px, command.to.y);
      first = false;
    }
  }
  wordBoxes[word] = box;
}

glm::vec2 ofApp::wordSize(const std::string& word, float size) const {
  const ofRectangle& box = wordBoxes.at(word);
  return glm::vec2(box.width, box.height) * (size / fontSize);
}
