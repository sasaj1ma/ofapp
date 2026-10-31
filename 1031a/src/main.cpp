#include "ofMain.h"
#include "ofApp.h"

// 使い方
//   1031a                                       ウィンドウを開いて動かす
//   1031a --snapshot out.png                    t = 1 秒の絵を PNG に書いて終わる
//   1031a --snapshot out.png --mode 3 --time 2.3   モードと時刻を指定して書く
int main(int argc, char* argv[]) {
  std::string snapshotPath;
  int mode = 0;
  float time = 1.0f;
  for (int i = 1; i < argc; i++) {
    const std::string arg = argv[i];
    if (arg == "--snapshot" && i + 1 < argc) snapshotPath = argv[++i];
    if (arg == "--mode" && i + 1 < argc) mode = std::stoi(argv[++i]);
    if (arg == "--time" && i + 1 < argc) time = std::stof(argv[++i]);
  }

  ofGLFWWindowSettings settings;
  // 輪郭をなめらかにする。既定は 4。
  settings.numSamples = 8;
  settings.setSize(1280, 720);
  settings.windowMode = OF_WINDOW;
  auto window = ofCreateWindow(settings);
  // Retina では大きさがピクセル数として扱われ、画面上では半分に見える。見た目の大きさに直す。
  const int scale = std::static_pointer_cast<ofAppGLFWWindow>(window)->getPixelScreenCoordScale();
  window->setWindowShape(1280 * scale, 720 * scale);
  ofRunApp(window, std::make_shared<ofApp>(snapshotPath, mode, time));
  ofRunMainLoop();
}
