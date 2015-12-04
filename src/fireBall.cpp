#pragma warning(disable:4819)
#include "fireBall.h"

ofImage ParticleOrb::img;

ParticleOrb::ParticleOrb(ofPoint _p, float _size) {
	size = _size;
	p = _p;
	v = ofPoint(ofRandom(-1, 1), ofRandom(-1, 1));
	v = 0.3*size*ofRandom(1)*v.normalize(); //初期速度0.3
	c = _p;
}
void ParticleOrb::setup() {
	img.loadImage("particle.png");
}

void ParticleOrb::draw() {
	img.draw(p, 5 * size, 5 * size);
};

bool ParticleOrb::isDead() {
	return (lt <= 0);
}


void ParticleOrb::update() {
	p += v;
	ofPoint d = p - c;
	v.x += 0.02*(ofRandom(-1, 1) - 0.05*d.x)*size; // だんだんと真ん中に寄るように
	v.y += -0.02*size; // 上に昇るように
	--lt; // 残り生存時間を減らす
};


FireBall::FireBall(float x, float y) {
	pos = ofPoint(x, y);
	size = 10; // ここで大きさを指定してやる
}

void FireBall::update() {

	// パーティクルを追加
	for (int i = 0; i<10; ++i)
		ps.push_back(ParticleOrb(pos, size));

	for (vector<ParticleOrb>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->update();
	}
	for (vector<ParticleOrb>::iterator it = ps.begin(); it != ps.end(); ++it) {
		if (it->isDead()) {
			it = ps.erase(it);
		}
	}
};

void FireBall::update(ofPoint _pos) {

	pos = _pos;

	// パーティクルを追加
	for (int i = 0; i<10; ++i)
		ps.push_back(ParticleOrb(pos, size));

	for (vector<ParticleOrb>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->update();
	}
	for (vector<ParticleOrb>::iterator it = ps.begin(); it != ps.end(); ++it) {
		if (it->isDead()) {
			it = ps.erase(it);
		}
	}
};


void FireBall::draw() {
	for (vector<ParticleOrb>::iterator it = ps.begin(); it != ps.end(); ++it) {
		it->draw();
	}
};

