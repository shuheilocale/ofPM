#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include <vector>

class Dust {
private:

	static ofImage img; // パーティクルの画像を読み込む
	ofPoint p; // 位置
	ofPoint v; // 速度
	ofPoint c; // 炎自体の中心位置
	float size; // 炎の大きさ
	int lt = 40; //消えるまでの時間

public:
	Dust() {};
	Dust(ofPoint _p, float _size);

	static void setup();
	void update();
	void draw();
	bool isDead();
};


class FugitiveDust {
	std::vector<Dust> ps; // パーティクルの配列
	ofPoint posR; // 埃の位置
	ofPoint posL; // 埃の位置
	bool RtoL;
	float size; // 埃の大きさ
public:
	FugitiveDust() {};
	FugitiveDust(float x, float y);
	void setup(const ofPoint& _posR, const ofPoint& _posL, bool _RtoL);
	void update();
	void update(ofPoint _pos);
	void draw();
	void reflesh(const ofPoint& pos, bool R2L);
};