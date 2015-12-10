#pragma warning(disable:4819)
#include "fugitiveDust.h"

ofImage Dust::img;

Dust::Dust(ofPoint _p, float _size) {
	size = _size;
	p = ofPoint(_p.x + ofRandom(-20,20), _p.y);
	v = ofPoint(ofRandom(-10, 10), ofRandom(-1, 1));
	v = 0.3*size*ofRandom(1)*v.normalize(); //初期速度0.3
	c = p;
}

void Dust::setup() {
	if (!img.loadImage("dust.png")) {
		printf("image load error!!\n");
	}
}


void Dust::draw() {
	img.draw(p, 5 * size, 5 * size);
};


bool Dust::isDead() {
	return (lt <= 0);
}


void Dust::update() {
	p += v;
	ofPoint d = p - c;
	v.x += 0.02*(ofRandom(-1, 1) - 0.05*d.x)*size; // だんだんと真ん中に寄るように
	v.y += -0.02*size; // 上に昇るように
	--lt; // 残り生存時間を減らす
};


// 未実装
FugitiveDust::FugitiveDust(float x, float y) {
	//pos = ofPoint(x, y);
	//size = 10; // ここで大きさを指定してやる
}

void FugitiveDust::setup(const ofPoint& _posL, const ofPoint& _posR, bool _RtoL) {
	posR = _posR;
	posL = _posL;
	RtoL = _RtoL;
	size = 2;
	Dust::setup();
}

void FugitiveDust::update() {

	// パーティクルを追加
	const int dustNum = 50;

	float step = fabs(posR.x - posL.x) / dustNum;

	for (int i = 0; i<dustNum; ++i)
		if (RtoL) {
			ps.push_back(Dust(ofPoint( posR.x-step*i, posR.y), size));
		}
		else {
			ps.push_back(Dust(ofPoint(posL.x + step * i, posL.y), size));
		}

	for (vector<Dust>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->update();
	}

	for (vector<Dust>::iterator it = ps.begin(); it != ps.end(); ++it) {
		if (it->isDead()) {
			it = ps.erase(it);
		}
	}
};
/*　未実装
void FugitiveDust::update(ofPoint _pos) {

	pos = _pos;

	// パーティクルを追加
	for (int i = 0; i<50; ++i)
		ps.push_back(Dust(pos, size));

	for (vector<Dust>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->update();
	}
	for (vector<Dust>::iterator it = ps.begin(); it != ps.end(); ++it) {
		if (it->isDead()) {
			it = ps.erase(it);
		}
	}
};

*/
void FugitiveDust::draw() {
	for (vector<Dust>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->draw();
	}
};

void FugitiveDust::refresh(const ofPoint& _posL, const ofPoint& _posR, bool _RtoL) {
	ps.clear();
	if (_RtoL) {
		setup(_posL, _posR, true);
	}
	else {
		setup(_posL, _posR, false);
	}
}