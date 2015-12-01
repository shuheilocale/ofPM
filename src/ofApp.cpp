#include "ofApp.h"

using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {
	// window
	ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	ofSetFrameRate(60);

	//�J��������f������荞��ŕ\��
	camera.setVerbose(true);
	camera.initGrabber(640, 480);

	//�g�p����摜�̗̈���m��
	colorImage.allocate(640, 480);
	grayImage.allocate(640, 480);
	grayBg.allocate(640, 480);
	grayDiff.allocate(640, 480);

	//�ϐ��̏�����
	bLearnBakground = true;
	showCvAnalysis = false;
	threshold = 20;
	videoMode = 0;

	ofEnableAlphaBlending();

	Particle::setup();
	for (int i = 1; i < 2; i++) {
		fs.push_back(FireBall(ofGetWidth() / i, ofGetHeight() / i));
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	for (vector<FireBall>::iterator it = fs.begin(); it != fs.end(); ++it) {
		it->update();
	}

	//�V�K�t���[���̎�荞�݂����Z�b�g
	bool bNewFrame = false;

	//�J�����g�p�̏ꍇ�̓J��������V�K�t���[����荞��
	camera.update();
	//�V�K�Ƀt���[�����؂�ւ����������
	bNewFrame = camera.isFrameNew();

	//�t���[�����؂�ւ�����ۂ̂݉摜�����
	if (bNewFrame) {
		//��荞�񂾃t���[�����摜�Ƃ��ăL���v�`��
		colorImage.setFromPixels(camera.getPixels(), 640, 480);
		//���E���]
		colorImage.mirror(false, true);

		//�J���[�̃C���[�W���O���[�X�P�[���ɕϊ�
		grayImage = colorImage;

		//�܂��w�i�摜���L�^����Ă��Ȃ���΁A���݂̃t���[����w�i�摜�Ƃ���
		if (bLearnBakground == true) {
			grayBg = grayImage;
			bLearnBakground = false;
		}

		//�O���[�X�P�[���̃C���[�W�Ǝ�荞�񂾔w�i�摜�Ƃ̍������Z�o
		grayDiff.absDiff(grayBg, grayImage);
		//�摜��2�l��(���ƍ�������)����
		grayDiff.threshold(threshold);
		//2�l�������摜����֊s�𒊏o����
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



	//���݂̃��[�h�ɉ����āA�\������f����؂�ւ�
	switch (videoMode) {

	case 1:
		//�O���[�X�P�[���f��
		grayImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 2:
		//�w�i�摜
		grayBg.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 3:
		//2�l�����ꂽ�����f��
		grayDiff.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case 4:
		//�����̃O���[�X�[�P��
		maskImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	default:
		//�J���[�f��
		colorImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;
	}

	for (vector<FireBall>::iterator it = fs.begin(); it != fs.end(); ++it) {
		it->draw();
	}


	//��ʂɑ΂���f���̔䗦���v�Z
	float ratioX = ofGetWidth() / 640;
	float ratioY = ofGetHeight() / 480;

	//��͌��ʂ�\������ꍇ
	if (showCvAnalysis) {
		//���o������͌��ʂ�\��
		for (int i = 0; i < contourFinder.nBlobs; i++) {
			ofPushMatrix();//��ʃT�C�Y�����ς��ɕ\�������悤���X�P�[��
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

	//���O�Ƒ��������\��
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
	//�L�[���͂Ń��[�h�؂�ւ�
	switch (key) {
	case '0':
		//�J���[�f���\��
		videoMode = 0;
		break;

	case '1':
		//�O���[�X�P�[���f���\��
		videoMode = 1;
		break;

	case '2':
		//�w�i�摜�\��
		videoMode = 2;
		break;

	case '3':
		//2�l�����������f��
		videoMode = 3;
		break;

	case '4':
		videoMode = 4;
		break;

	case 'a':
		//��͌��ʂ̕\���� on / off
		showCvAnalysis ? showCvAnalysis = false : showCvAnalysis = true;
		break;

	case 'f':
		//�t���X�N���[����
		ofSetFullscreen(true);
		break;

	case ' ':
		//�w�i�摜��V�K�Ɏ�荞��
		bLearnBakground = true;
		break;

	case '+':
		//2�l����臒l�𑝉�
		threshold++;
		if (threshold > 255) threshold = 255;
		break;

	case '-':
		//2�l����臒l������
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

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

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

ofImage Particle::img;

Particle::Particle(ofPoint _p, float _size) {
	size = _size;
	p = _p;
	v = ofPoint(ofRandom(-1, 1), ofRandom(-1, 1));
	v = 0.3*size*ofRandom(1)*v.normalize(); //�������x0.3
	c = _p;
}
void Particle::setup() {
	img.loadImage("particle.png");
}

void Particle::update() {
	p += v;
	ofPoint d = p - c;
	v.x += 0.02*(ofRandom(-1, 1) - 0.05*d.x)*size; // ���񂾂�Ɛ^�񒆂Ɋ��悤��
	v.y += -0.02*size; // ��ɏ���悤��
	--lt; // �c�萶�����Ԃ����炷
};


void Particle::draw() {
	img.draw(p, 5 * size, 5 * size);
};

bool Particle::isDead() {
	return (lt <= 0);
}

FireBall::FireBall(float x, float y) {
	pos = ofPoint(x, y);
	size = 10; // �����ő傫�����w�肵�Ă��
}

void FireBall::update() {

	// �p�[�e�B�N����ǉ�
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

	// �p�[�e�B�N����ǉ�
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