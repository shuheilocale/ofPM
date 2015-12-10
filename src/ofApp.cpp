#include "ofApp.h"
#include "ofAppGlutWindow.h"
#include "fireBall.h"
#include "aura.h"
#include "afterimage.h"
#include "autoBuilder.h"
#include "cosmo.h"

using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetBackgroundAuto(false);

	// for aura
	ofSetVerticalSync(true);
	ofDisableBlendMode();
	ofEnableBlendMode(OF_BLENDMODE_ADD);

	for (int i = 0; i < 25000; i++) {
		Aura auraCore;
		auraCore.setInitCondition(ofRandom(WINDOW_W), ofRandom(WINDOW_H), 0, 0);
		auraCore.damping = ofRandom(0.01, 0.05);
		aura.push_back(auraCore);
	}

#ifdef TARGET_OPENGLES
	shaderBlurX.load("shadersES2/shaderBlurX");
	shaderBlurY.load("shadersES2/shaderBlurY");
#else
	if (ofIsGLProgrammableRenderer()) {
		printf("using GL3\n");
		shaderBlurX.load("shadersGL3/shaderBlurX");
		shaderBlurY.load("shadersGL3/shaderBlurY");
	}
	else {
		printf("using GL2\n");
		shaderBlurX.load("shadersGL2/shaderBlurX");
		shaderBlurY.load("shadersGL2/shaderBlurY");
	}
#endif



	fboBlurOnePass.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA);
	fboBlurTwoPass.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA);
	drawParticles.allocate(ofGetWindowWidth(), ofGetWindowHeight(), GL_RGBA);


	// basic setting

	// window
	ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	ofSetFrameRate(60);

	//カメラから映像を取り込んで表示
	camera.setVerbose(true);
	camera.initGrabber(WINDOW_W, WINDOW_H);

	//使用する画像の領域を確保
	colorImage.allocate(WINDOW_W, WINDOW_H);
	grayImage.allocate(WINDOW_W, WINDOW_H);
	grayBg.allocate(WINDOW_W, WINDOW_H);
	grayDiff.allocate(WINDOW_W, WINDOW_H);
	// todo ↑ウィンドウ引き伸ばしに対応

	//変数の初期化
	bLearnBakground = true;
	showCvAnalysis = false;
	showFullScreen = false;
	showBenchmark = false;
	threshold = 20;
	videoMode = 0;
	
	ParticleOrb::setup();
	for (int i = 1; i < 2; i++) {
		fs.push_back(FireBall(ofGetWidth() / i, ofGetHeight() / i));
	}

	selectedCorner = -1;
	corners[0].set(0, 0);
	corners[1].set(camera.getWidth(), 0);
	corners[2].set(camera.getWidth(), camera.getHeight());
	corners[3].set(0, camera.getHeight());

	cosmo.setup(20000);

	ab.setup(10, ofPoint(0.0,0.0), ofPoint(0.0, 0.0), true);

	printf("ogww:%f, ogww:%f\n", ofGetWidth(), ofGetWindowWidth());

	int x = (ofGetWidth() - camera.getWidth()) * 0.5;       // center on screen.
	int y = (ofGetHeight() - camera.getHeight()) * 0.5;     // center on screen.
	//int x = ofGetWindowWidth() * 0.5;
	//int y = ofGetWindowHeight() * 0.5;
	//int w = camera.getWidth();
	//int h = camera.getHeight();
	int w = ofGetWindowWidth();
	int h = ofGetWindowHeight();

	displayfbo.allocate(ofGetWindowWidth(), ofGetWindowHeight());

	warper.setSourceRect(ofRectangle(0, 0, w, h));              // this is the source rectangle which is the size of the image and located at ( 0, 0 )
	warper.setTopLeftCornerPosition(ofPoint(x, y));             // this is position of the quad warp corners, centering the image on the screen.
	warper.setTopRightCornerPosition(ofPoint(x + w, y));        // this is position of the quad warp corners, centering the image on the screen.
	warper.setBottomLeftCornerPosition(ofPoint(x, y + h));      // this is position of the quad warp corners, centering the image on the screen.
	warper.setBottomRightCornerPosition(ofPoint(x + w, y + h)); // this is position of the quad warp corners, centering the image on the screen.
	warper.setup();
	warper.load(); // reload last saved changes.


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
	maxid = -1;
	//フレームが切り替わった際のみ画像を解析
	if (bNewFrame) {
		//取り込んだフレームを画像としてキャプチャ
		colorImage.setFromPixels(camera.getPixels(), WINDOW_W, WINDOW_H);
		//左右反転
		colorImage.mirror(false, false);

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
	

		for (int i = 0; i < 3; i++) {
			grayDiff.erode();
		}
		
		for (int i = 0; i < 6; i++) {
			grayDiff.dilate();
		}

		for (int i = 0; i < 3; i++) {
			grayDiff.erode();
		}



		//2値化した画像から輪郭を抽出する
		contourFinder.findContours(grayDiff, 25, grayDiff.width * grayDiff.height, 10, false, false);
		float maxarea = 0.0;
		for (int i = 0; i < contourFinder.nBlobs; i++) {
			if (maxarea < contourFinder.blobs[i].area) {
				maxarea = contourFinder.blobs[i].area;
				maxid = i;
			}
		}

		//if (maxid >= 0) {
			//printf("x:%f,y:%f\n", contourFinder.blobs[maxid].centroid.x, contourFinder.blobs[maxid].centroid.y);
		//}

		unsigned char *diffPixs = grayDiff.getPixels();
		unsigned char *colorPixs = colorImage.getPixels();

		int nPixs = WINDOW_W * WINDOW_H;

		unsigned char* compositeImgPixels = new unsigned char[nPixs * 3];
		
		for (int i =0; i < nPixs; ++i) {
			if( diffPixs[i]==255){
				compositeImgPixels[3*i] = colorPixs[3*i];
				compositeImgPixels[3*i+1] = colorPixs[3*i+1];
				compositeImgPixels[3*i+2] = colorPixs[3*i+2];

				//fs.push_back(FireBall(nPixs % WINDOW_W, nPixs / WINDOW_W));
			}else{
				compositeImgPixels[3*i] = 0;
				compositeImgPixels[3*i+1] = 0;
				compositeImgPixels[3*i+2] = 0;


			}
		}
		
		maskImage.setFromPixels(compositeImgPixels, WINDOW_W, WINDOW_H);

		delete[] compositeImgPixels;

		for (int i = 0; i < contourFinder.nBlobs; i++) {
			if ( fs.size() < i) break;
			
			fs[i].update(ofPoint(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y));


			//ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
			break;
		}

		if (maxid >= 0) {
			cosmo.update(contourFinder.blobs[maxid].centroid);
		}
		else {
			cosmo.update(ofPoint(WINDOW_H,WINDOW_W));
		}

		ab.update();
	}



}

//--------------------------------------------------------------
void ofApp::draw() {

	ofMatrix4x4 mat = warper.getMatrix();
	AfterImage af;

	displayfbo.begin();

	//現在のモードに応じて、表示する映像を切り替え
	switch (videoMode) {

	case 1:
		//グレースケール映像
		grayImage.draw(0, 0);//(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 2:
		//背景画像
		grayBg.draw(0, 0);//(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 3:
		//2値化された差分映像
		grayDiff.draw(0, 0);//(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 4:
		//差分
		maskImage.draw(0, 0);//(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 5:
		colorImage.draw(0, 0);//(0, 0, ofGetWidth(), ofGetHeight());
		drawAura();
		break;
	case 6:
		ofSetColor(0, 0, 0, 23);
		ofRect(0, 0, ofGetWidth(), ofGetHeight());
//		ofBackground(0, 0, 0);
		drawAura();
		break;
	case 7:
		//残像
		ofSetColor(0, 0, 0, 5);
		ofRect(0, 0, ofGetWidth(), ofGetHeight());
		preAf.draw(ofGetFrameNum());
		af.nPts = contourFinder.blobs[maxid].nPts;
		af.pts = contourFinder.blobs[maxid].pts;
		af.draw(0,0,0);
		preAf.nPts = af.nPts;
		preAf.pts = af.pts;
		break;
	case 8:
		ofBackground(0, 0, 0);
		cosmo.draw();
		break;
	case 9:
	{
		ofBackground(0, 0, 0);
		if (maxid >= 0) {
			const ofRectangle& curBoundingRect = contourFinder.blobs[maxid].boundingRect;
			//printf("x/y = %f\n", (curBoundingRect.width / preBoundingRect.width));
			if ((curBoundingRect.width / preBoundingRect.width) > 1.3) {
				bool R2L = contourFinder.blobs[maxid].centroid.x < preCentroid.x;
				//ab.refresh(contourFinder.blobs[maxid].centroid, R2L);
				if (R2L) {
					ab.refresh(ofPoint(0, ofGetHeight()), ofPoint(contourFinder.blobs[maxid].centroid.x, ofGetHeight()), true);
				}
				else {
					ab.refresh(ofPoint(contourFinder.blobs[maxid].centroid.x, ofGetHeight()), ofPoint(ofGetWindowWidth(), ofGetHeight()), false);
				}
			}
		}
		ab.draw();
		break;
	}
	default:
		//カラー映像
		colorImage.draw(0, 0);//(0, 0, ofGetWidth(), ofGetHeight());

		for (vector<FireBall>::iterator it = fs.begin(); it != fs.end(); ++it) {
			it->draw();
		}
		break;
	}

	if (maxid >= 0) {
		preBoundingRect = contourFinder.blobs[maxid].boundingRect;
		preCentroid = contourFinder.blobs[maxid].centroid;
	}

	//画面に対する映像の比率を計算
	float ratioX = ofGetWidth() / WINDOW_W;
	float ratioY = ofGetHeight() / WINDOW_H;

	//解析結果を表示する場合
	if (showCvAnalysis) {
		if (maxid >= 0) {
			ofPushMatrix();//画面サイズいっぱいに表示されるようリスケール
			glScalef(ratioX, ratioY,1.0f);
			contourFinder.blobs[maxid].draw(0, 0);
			//ofFill();
			ofSetColor(255, 0, 0);
			ofEllipse(contourFinder.blobs[maxid].centroid.x*ratioW, contourFinder.blobs[maxid].centroid.y*ratioW, 4, 4);
			ofPopMatrix();
		}

		//検出した解析結果を表示
		/*for (int i = 0; i < contourFinder.nBlobs; i++) {
			
			
			ofPushMatrix();//画面サイズいっぱいに表示されるようリスケール
			glScalef((float)ofGetWidth() / (float)grayDiff.width,
				(float)ofGetHeight() / (float)grayDiff.height,
				1.0f);
			contourFinder.blobs[i].draw(0, 0);
			//ofFill();
			ofSetColor(255, 0, 0);
			ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
			ofPopMatrix();
		}
		*/
	}

	displayfbo.end();

	ofPushMatrix();
	ofMultMatrix(mat);
	displayfbo.draw(0, 0);
	ofPopMatrix();


	
	ofSetColor(ofColor::magenta);
	warper.drawQuadOutline();

	ofSetColor(ofColor::yellow);
	warper.drawCorners();

	ofSetColor(ofColor::magenta);
	warper.drawHighlightedCorner();

	ofSetColor(ofColor::red);
	warper.drawSelectedCorner();
	
	ofSetColor(255, 255, 255);

	if (showBenchmark) {
		//ログと操作説明を表示
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
}
void ofApp::drawAura() {

	drawParticles.begin();

	ofSetColor(0);
	//ofRect (0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	ofClear(0, 0);

	int i = 0;
	float x = 0.0, y = 0.0;
	for (vector<Aura>::iterator it = aura.begin(); it != aura.end(); it++) {

		it->resetForce();
		//it->addAttraction(thisMouseX, thisMouseY, 1000, 0.05);
		//it->addRepulsion(thisMouseX, thisMouseY, 50, 80);

		if (contourFinder.blobs.size() > 0) {

			x = contourFinder.blobs[maxid].centroid.x * ratioW;
			y = contourFinder.blobs[maxid].centroid.y * ratioH;
			it->addAttraction(x, y, 1000, 0.2);
		//	it->addRepulsion(contourFinder.blobs[maxid].centroid.x, contourFinder.blobs[maxid].centroid.y, 50, 80);
			it->addRepulsion(&grayDiff, &contourFinder.blobs[maxid], 50, 40);
		}
		//it->addRepulsion(thisMouseX, thisMouseY, 50, 80);
		it->addDamping();
		it->update();

		if (i == 0) {
			if (contourFinder.blobs.size() > 0) {
				x = contourFinder.blobs[maxid].centroid.x * ratioW;
				y = contourFinder.blobs[maxid].centroid.y * ratioH;
				it->trail(contourFinder.blobs[maxid].centroid.x, contourFinder.blobs[maxid].centroid.y);
			}
			//it->trail(thisMouseX, thisMouseY);
		}
		else {
			it->trail(aura[i - 1].pos.x * ratioW, aura[i - 1].pos.y * ratioH);
		}

		it->draw();

		i++;
	}

	drawParticles.end();

	fboBlurOnePass.begin();

	ofClear(0, 0);

	shaderBlurX.begin();
	shaderBlurX.setUniform1f("blurAmnt", 1.25);

	ofSetColor(255);
	drawParticles.draw(0, 0);


	shaderBlurX.end();

	fboBlurOnePass.end();

	fboBlurTwoPass.begin();

	ofClear(0, 0);

	shaderBlurY.begin();
	shaderBlurY.setUniform1f("blurAmnt", 1.25);

	ofSetColor(255);
	fboBlurOnePass.draw(0, 0);

	shaderBlurY.end();

	fboBlurTwoPass.end();


	ofSetColor(255);
	fboBlurTwoPass.draw(0, 0);
	//ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 10);

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
	case '5':
		// aura
		aura.clear();
		for (int i = 0; i < 25000; i++) {
			Aura auraCore;
			auraCore.setInitCondition(ofRandom(WINDOW_W), ofRandom(WINDOW_H), 0, 0);
			auraCore.damping = ofRandom(0.01, 0.05);
			aura.push_back(auraCore);
		}
		videoMode = 5;
		break;
	case '6':
		// aura
		aura.clear();
		for (int i = 0; i < 25000; i++) {
			Aura auraCore;
			auraCore.setInitCondition(ofRandom(WINDOW_W), ofRandom(WINDOW_H), 0, 0);
			auraCore.damping = ofRandom(0.01, 0.05);
			aura.push_back(auraCore);
		}
		videoMode = 6;
		break;
	case '7':
		// AfterImage
		videoMode = 7;
		break;
	case '8':
		// Cosmo
		videoMode = 8;
		break;
	case '9':
		// Cosmo
		// ２番めの引数はちゃんとかえるんだよ
		//ab.refresh( ofPoint(ofGetWindowWidth()/2, ofGetWindowHeight()+10.0), true);
		videoMode = 9;
		break;
	case 'a':
		//解析結果の表示の on / off
		showCvAnalysis ? showCvAnalysis = false : showCvAnalysis = true;
		break;

	case 'f':
		//フルスクリーンに
		ofSetFullscreen(showFullScreen);
		showFullScreen ? showFullScreen = false : showFullScreen = true;
		break;
	case 'b':
		//ベンチマーク
		showBenchmark ? showBenchmark = false : showBenchmark = true;
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
	case 's':
		warper.save();
		break;
	case 'l':
		warper.load();
		break;
	case 'h':
		warper.toggleShow();
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
	ratioW = w / WINDOW_W;
	ratioH = h / WINDOW_H;

	/*
	colorImage.resize(w, h);
	grayImage.resize(w, h);
	grayBg.resize(w, h);
	grayDiff.resize(w, h);
	*/
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
void ofApp::ofxQuadWarp_(ofBaseHasTexture &tex, ofPoint lt, ofPoint rt, ofPoint rb, ofPoint lb, int rows, int cols) {

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
