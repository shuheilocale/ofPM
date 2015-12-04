#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include <vector>

class ParticleOrb {
private:

	static ofImage img; // パーティクルの画像を読み込む
	ofPoint p; // 位置
	ofPoint v; // 速度
	ofPoint c; // 炎自体の中心位置
	float size; // 炎の大きさ
	int lt = 40; //消えるまでの時間

public:
	ParticleOrb() {};
	ParticleOrb(ofPoint _p, float _size);

	static void setup();
	void update();
	void draw();
	bool isDead();
};


class FireBall {
	std::vector<ParticleOrb> ps; // パーティクルの配列
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