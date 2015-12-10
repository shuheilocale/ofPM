#pragma once
#pragma warning(disable:4819)
#include "ofMain.h"
#include "ofxQuadWarp.h"
#include "ofxOpenCv.h"
#include "fireBall.h"
#include "aura.h"
#include "afterimage.h"
#include "cosmo.h"
#include "autoBuilder.h"
#include <vector>

class ofApp : public ofBaseApp {

public:

	static const unsigned int WINDOW_W = 640;
	static const unsigned int WINDOW_H = 480;

	
	ofPoint corners[4];
	int selectedCorner;

	ofPoint cornersCalib[4];
	int selectedCornerCalib;
	int existsCornes;
	bool displayCalib;


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

	void drawAura();

private:
	ofVideoGrabber camera;
	ofxCvColorImage colorImage;
	ofxCvColorImage maskImage;
	ofxCvGrayscaleImage grayImage, edgeImage;
	ofxCvGrayscaleImage grayBg; //キャプチャーした背景画像
	ofxCvGrayscaleImage grayDiff; //現在の画像と背景との差分
	ofxCvContourFinder contourFinder; //輪郭抽出のためのクラス
	int maxid; // 輪郭抽出の最大面積を格納した番号（1フレーム毎に更新）
	ofRectangle         preBoundingRect;
	ofPoint				preCentroid;
	
	bool				preBlobsExist;
	vector<ofxCvBlob>	preBlobs;
	int preMaxid;

	float ratioW, ratioH;
	ofFbo displayfbo;

	bool bLearnBakground; //背景画像を登録したか否か
	bool showCvAnalysis; //解析結果を表示するか
	bool showFullScreen; //フルスクリーン
	bool showBenchmark;
	int threshold; //2値化の際の敷居値
	int videoMode; //表示する画像を選択

	vector<FireBall> fs; 
	vector<Aura> aura; //!< Kimrepo
	AfterImage preAf;
	Cosmo cosmo;
	AutoBuilder ab;

	int invisiCol;

	ofShader shaderBlurX;
	ofShader shaderBlurY;

	ofFbo drawParticles;
	ofFbo fboBlurOnePass;
	ofFbo fboBlurTwoPass;

	ofxQuadWarp warper;

};
