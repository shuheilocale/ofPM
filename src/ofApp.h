#pragma once
#pragma warning(disable:4819)
#include "ofMain.h"
#include "ofxOpenCv.h"
#include <vector>


class FireBall;
class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

private:
	ofVideoGrabber camera;
	ofxCvColorImage colorImage;
	ofxCvColorImage maskImage;
	ofxCvGrayscaleImage grayImage, edgeImage;
	ofxCvGrayscaleImage grayBg; //キャプチャーした背景画像
	ofxCvGrayscaleImage grayDiff; //現在の画像と背景との差分
	ofxCvContourFinder contourFinder; //輪郭抽出のためのクラス

	bool bLearnBakground; //背景画像を登録したか否か
	bool showCvAnalysis; //解析結果を表示するか
	int threshold; //2値化の際の敷居値
	int videoMode; //表示する画像を選択

	vector<FireBall> fs;
};


class Particle {
private:

	static ofImage img; // パーティクルの画像を読み込む
	ofPoint p; // 位置
	ofPoint v; // 速度
	ofPoint c; // 炎自体の中心位置
	float size; // 炎の大きさ
	int lt = 40; //消えるまでの時間

public:
	Particle() {};
	Particle(ofPoint _p, float _size);

	static void setup();
	void update();
	void draw();
	bool isDead();
};

class FireBall {
	std::vector<Particle> ps; // パーティクルの配列
	ofPoint pos; // 炎の位置
	float size; // 炎の大きさ
public:
	FireBall() {};
	FireBall(float x, float y);
	void setup();
	void update();
	void update(ofPoint _pos);
	void draw();
};