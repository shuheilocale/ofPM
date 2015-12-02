#include "ofApp.h"
#include "ofAppGlutWindow.h"

using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {
	// window
	ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	ofSetFrameRate(60);

	//カメラから映像を取り込んで表示
	camera.setVerbose(true);
	camera.initGrabber(640, 480);

	//使用する画像の領域を確保
	colorImage.allocate(640, 480);
	grayImage.allocate(640, 480);
	grayBg.allocate(640, 480);
	grayDiff.allocate(640, 480);

	//変数の初期化
	bLearnBakground = true;
	showCvAnalysis = false;
	threshold = 20;
	videoMode = 0;

	ofEnableAlphaBlending();

	Particle::setup();
	for (int i = 1; i < 2; i++) {
		fs.push_back(FireBall(ofGetWidth() / i, ofGetHeight() / i));
	}

	selectedCorner = -1;
	corners[0].set(0, 0);
	corners[1].set(camera.getWidth(), 0);
	corners[2].set(camera.getWidth(), camera.getHeight());
	corners[3].set(0, camera.getHeight());

}

//--------------------------------------------------------------
void ofApp::update() {

	for (vector<FireBall>::iterator it = fs.begin(); it != fs.end(); ++it) {
		it->update();
	}

	//新規フレームの取り込みをリセット
	bool bNewFrame = false;

	//カメラ使用の場合はカメラから新規フレーム取り込み
	camera.update();
	//新規にフレームが切り替わったか判定
	bNewFrame = camera.isFrameNew();

	//フレームが切り替わった際のみ画像を解析
	if (bNewFrame) {
		//取り込んだフレームを画像としてキャプチャ
		colorImage.setFromPixels(camera.getPixels(), 640, 480);
		//左右反転
		colorImage.mirror(false, true);

		//カラーのイメージをグレースケールに変換
		grayImage = colorImage;

		//まだ背景画像が記録されていなければ、現在のフレームを背景画像とする
		if (bLearnBakground == true) {
			grayBg = grayImage;
			bLearnBakground = false;
		}

		//グレースケールのイメージと取り込んだ背景画像との差分を算出
		grayDiff.absDiff(grayBg, grayImage);
		//画像を2値化(白と黒だけに)する
		grayDiff.threshold(threshold);
	

		for (int i = 0; i < 5; i++) {
			grayDiff.erode();
		}
		
		for (int i = 0; i < 10; i++) {
			grayDiff.dilate();
		}

		for (int i = 0; i < 5; i++) {
			grayDiff.erode();
		}

		/*

		for (int i = 0; i < 30; i++) {
			grayDiff.dilate();
		}
		/*
		for (int i = 0; i < 3; i++) {
			grayDiff.erode();
		}
		*/
		/*
		for (int i = 0; i < 5; i++) {
			grayDiff.erode();
		}
		for (int i = 0; i < 5; i++) {
			grayDiff.dilate();
		}
		*/
	

		//2値化した画像から輪郭を抽出する
		contourFinder.findContours(grayDiff, 25, grayDiff.width * grayDiff.height, 10, false, false);


		unsigned char *diffPixs = grayDiff.getPixels();
		unsigned char *colorPixs = colorImage.getPixels();

		int nPixs = 640 * 480;

		unsigned char* compositeImgPixels = new unsigned char[nPixs * 3];
		
		for (int i =0; i < nPixs; ++i) {
			if( diffPixs[i]==255){
				compositeImgPixels[3*i] = colorPixs[3*i];
				compositeImgPixels[3*i+1] = colorPixs[3*i+1];
				compositeImgPixels[3*i+2] = colorPixs[3*i+2];

				//fs.push_back(FireBall(nPixs % 640, nPixs / 640));

			}else{
				compositeImgPixels[3*i] = 0;
				compositeImgPixels[3*i+1] = 0;
				compositeImgPixels[3*i+2] = 0;


			}
		}
		
		maskImage.setFromPixels(compositeImgPixels, 640, 480);

		delete[] compositeImgPixels;

		for (int i = 0; i < contourFinder.nBlobs; i++) {
			if ( fs.size() < i) break;
			
			fs[i].update(ofPoint(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y));


			//ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
			break;
		}
	}



}

//--------------------------------------------------------------
void ofApp::draw() {



	//現在のモードに応じて、表示する映像を切り替え
	switch (videoMode) {

	case 1:
		//グレースケール映像
		grayImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 2:
		//背景画像
		grayBg.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 3:
		//2値化された差分映像
		grayDiff.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 4:
		//差分のグレースーケル
		maskImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	default:
		//カラー映像
		colorImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;
	}

	for (vector<FireBall>::iterator it = fs.begin(); it != fs.end(); ++it) {
		it->draw();
	}


	//画面に対する映像の比率を計算
	float ratioX = ofGetWidth() / 640;
	float ratioY = ofGetHeight() / 480;

	//解析結果を表示する場合
	if (showCvAnalysis) {
		//検出した解析結果を表示
		for (int i = 0; i < contourFinder.nBlobs; i++) {
			ofPushMatrix();//画面サイズいっぱいに表示されるようリスケール
			glScalef((float)ofGetWidth() / (float)grayDiff.width,
				(float)ofGetHeight() / (float)grayDiff.height,
				1.0f);
			contourFinder.blobs[i].draw(0, 0);
			ofFill();
			ofSetColor(255, 0, 0);
			ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
			ofPopMatrix();
		}
	}

	ofTranslate(30, 30);
	ofxQuadWarp(maskImage, corners[0], corners[1], corners[2], corners[3], 40, 40);

	for (int i = 0; i<4; i++) {
		ofCircle(corners[i], 10);
	}


	//ログと操作説明を表示
	ofSetColor(255, 255, 255);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 20, 20);
	ofDrawBitmapString("Threshold: " + ofToString(threshold), 20, 35);
	ofDrawBitmapString("Number of Blobs: " + ofToString(contourFinder.nBlobs), 20, 50);
	ofDrawBitmapString("[0] Show original video", 20, 65);
	ofDrawBitmapString("[1] Show grayscale video", 20, 80);
	ofDrawBitmapString("[2] Show captured background", 20, 95);
	ofDrawBitmapString("[3] Show difference from background", 20, 110);
	ofDrawBitmapString("[space] Captuer background", 20, 125);
	ofDrawBitmapString("[a] Analysis on / off", 20, 140);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	//キー入力でモード切り替え
	switch (key) {
	case '0':
		//カラー映像表示
		videoMode = 0;
		break;

	case '1':
		//グレースケール映像表示
		videoMode = 1;
		break;

	case '2':
		//背景画像表示
		videoMode = 2;
		break;

	case '3':
		//2値化した差分映像
		videoMode = 3;
		break;

	case '4':
		videoMode = 4;
		break;

	case 'a':
		//解析結果の表示の on / off
		showCvAnalysis ? showCvAnalysis = false : showCvAnalysis = true;
		break;

	case 'f':
		//フルスクリーンに
		ofSetFullscreen(true);
		break;

	case ' ':
		//背景画像を新規に取り込む
		bLearnBakground = true;
		break;

	case '+':
		//2値化の閾値を増加
		threshold++;
		if (threshold > 255) threshold = 255;
		break;

	case '-':
		//2値化の閾値を減少
		threshold--;
		if (threshold < 0) threshold = 0;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	corners[selectedCorner].set(x - 30, y - 30);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	selectedCorner = -1;
	for (int i = 0; i<4; i++) {
		if (ofDist(corners[i].x, corners[i].y, x - 30, y - 30)<10) {
			selectedCorner = i;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	selectedCorner = -1;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

ofPoint ofApp::ofxLerp(ofPoint start, ofPoint end, float amt) {
	return start + amt * (end - start);
}

//--------------------------------------------------------------
int ofApp::ofxIndex(float x, float y, float w) {
	return y*w + x;
}

//--------------------------------------------------------------
void ofApp::ofxQuadWarp(ofBaseHasTexture &tex, ofPoint lt, ofPoint rt, ofPoint rb, ofPoint lb, int rows, int cols) {

	float tw = tex.getTexture().getWidth();//.getTextureReference().getWidth();
	float th = tex.getTexture().getHeight();//getTextureReference().getHeight();

	ofMesh mesh;

	for (int x = 0; x <= cols; x++) {
		float f = float(x) / cols;
		ofPoint vTop(ofxLerp(lt, rt, f));
		ofPoint vBottom(ofxLerp(lb, rb, f));
		ofPoint tTop(ofxLerp(ofPoint(0, 0), ofPoint(tw, 0), f));
		ofPoint tBottom(ofxLerp(ofPoint(0, th), ofPoint(tw, th), f));

		for (int y = 0; y <= rows; y++) {
			float f = float(y) / rows;
			ofPoint v = ofxLerp(vTop, vBottom, f);
			mesh.addVertex(v);
			mesh.addTexCoord(ofxLerp(tTop, tBottom, f));
		}
	}

	for (float y = 0; y<rows; y++) {
		for (float x = 0; x<cols; x++) {
			mesh.addTriangle(ofxIndex(x, y, cols + 1), ofxIndex(x + 1, y, cols + 1), ofxIndex(x, y + 1, cols + 1));
			mesh.addTriangle(ofxIndex(x + 1, y, cols + 1), ofxIndex(x + 1, y + 1, cols + 1), ofxIndex(x, y + 1, cols + 1));
		}
	}

	tex.getTexture().bind();
	mesh.draw();
	tex.getTexture().unbind();
	mesh.drawVertices();
}

ofImage Particle::img;

Particle::Particle(ofPoint _p, float _size) {
	size = _size;
	p = _p;
	v = ofPoint(ofRandom(-1, 1), ofRandom(-1, 1));
	v = 0.3*size*ofRandom(1)*v.normalize(); //初期速度0.3
	c = _p;
}
void Particle::setup() {
	img.loadImage("particle.png");
}

void Particle::update() {
	p += v;
	ofPoint d = p - c;
	v.x += 0.02*(ofRandom(-1, 1) - 0.05*d.x)*size; // だんだんと真ん中に寄るように
	v.y += -0.02*size; // 上に昇るように
	--lt; // 残り生存時間を減らす
};


void Particle::draw() {
	img.draw(p, 5 * size, 5 * size);
};

bool Particle::isDead() {
	return (lt <= 0);
}

FireBall::FireBall(float x, float y) {
	pos = ofPoint(x, y);
	size = 10; // ここで大きさを指定してやる
}

void FireBall::update() {

	// パーティクルを追加
	for (int i = 0; i<10; ++i)
		ps.push_back(Particle(pos, size));

	for (vector<Particle>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->update();
	}
	for (vector<Particle>::iterator it = ps.begin(); it != ps.end(); ++it) {
		if (it->isDead()) {
			it = ps.erase(it);
		}
	}
};

void FireBall::update(ofPoint _pos) {

	pos = _pos;

	// パーティクルを追加
	for (int i = 0; i<10; ++i)
		ps.push_back(Particle(pos, size));

	for (vector<Particle>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->update();
	}
	for (vector<Particle>::iterator it = ps.begin(); it != ps.end(); ++it) {
		if (it->isDead()) {
			it = ps.erase(it);
		}
	}
};


void FireBall::draw() {
	for (vector<Particle>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->draw();
	}
};