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
	ofxCvGrayscaleImage grayBg; //�L���v�`���[�����w�i�摜
	ofxCvGrayscaleImage grayDiff; //���݂̉摜�Ɣw�i�Ƃ̍���
	ofxCvContourFinder contourFinder; //�֊s���o�̂��߂̃N���X

	bool bLearnBakground; //�w�i�摜��o�^�������ۂ�
	bool showCvAnalysis; //��͌��ʂ�\�����邩
	int threshold; //2�l���̍ۂ̕~���l
	int videoMode; //�\������摜��I��

	vector<FireBall> fs;
};


class Particle {
private:

	static ofImage img; // �p�[�e�B�N���̉摜��ǂݍ���
	ofPoint p; // �ʒu
	ofPoint v; // ���x
	ofPoint c; // �����̂̒��S�ʒu
	float size; // ���̑傫��
	int lt = 40; //������܂ł̎���

public:
	Particle() {};
	Particle(ofPoint _p, float _size);

	static void setup();
	void update();
	void draw();
	bool isDead();
};

class FireBall {
	std::vector<Particle> ps; // �p�[�e�B�N���̔z��
	ofPoint pos; // ���̈ʒu
	float size; // ���̑傫��
public:
	FireBall() {};
	FireBall(float x, float y);
	void setup();
	void update();
	void update(ofPoint _pos);
	void draw();
};