#include "ofApp.h"

void ofApp::setup() {
  ofSetFrameRate(60);
  ofSetWindowTitle("__NAME__");
  ofBackground(12);
}

void ofApp::update() {}

void ofApp::draw() {
  if (!snapshotPath.empty()) {
    drawScene(1.0f);
    ofSaveScreen(snapshotPath);
    ofExit();
    return;
  }
  drawScene(ofGetElapsedTimef());
}

void ofApp::keyPressed(int key) {
  // s で今の画面を bin/data/ に保存する。
  if (key == 's') ofSaveScreen("screenshot-" + ofGetTimestampString() + ".png");
}

void ofApp::drawScene(float t) {
  ofSetColor(255);
  ofDrawCircle(ofGetWidth() / 2 + std::cos(t) * 200, ofGetHeight() / 2 + std::sin(t) * 200, 40);
}
