#include "ofMain.h"
#include "ofApp.h"

// 使い方
//   __NAME__                       ウィンドウを開いて動かす
//   __NAME__ --snapshot out.png    t = 1 秒の絵を PNG に書いて終わる
int main(int argc, char* argv[]) {
  std::string snapshotPath;
  for (int i = 1; i < argc; i++) {
    const std::string arg = argv[i];
    if (arg == "--snapshot" && i + 1 < argc) snapshotPath = argv[++i];
  }

  ofGLFWWindowSettings settings;
  // 輪郭をなめらかにする。既定は 4。
  settings.numSamples = 8;
  settings.setSize(1024, 768);
  settings.windowMode = OF_WINDOW;
  auto window = ofCreateWindow(settings);
  // Retina では大きさがピクセル数として扱われ、画面上では半分に見える。見た目の大きさに直す。
  const int scale = std::static_pointer_cast<ofAppGLFWWindow>(window)->getPixelScreenCoordScale();
  window->setWindowShape(1024 * scale, 768 * scale);
  ofRunApp(window, std::make_shared<ofApp>(snapshotPath));
  ofRunMainLoop();
}
