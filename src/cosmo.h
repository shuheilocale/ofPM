#pragma once

#include "ofMain.h"
#include <deque>

using namespace std;



class Cosmo
{
public:
	class CosmoUnit {

	public:
		static ofImage aramaki;
		static ofImage teshima;
		static ofImage maeda;
		ofPoint pos;
		float radius;
		int  idx;
		//int   alpha;
		ofColor color;

		CosmoUnit() {
		}

		static void loadImage() {
			aramaki.loadImage("faces/aramaki.png");
			teshima.loadImage("faces/teshima.png");
			maeda.loadImage("faces/maeda.png");
		}

		void setup(int x, int y, float _radius, const ofColor& _color) {
			pos.x = x;
			pos.y = y;
			radius = _radius;
			//alpha = _alpha;
			color = _color;
			idx = ofRandom(0, 2.99);
		}

		void updateAbs(float x, float y) {
			pos.x = x;
			pos.y = y;
		}

		void update(float x, float y, float _radius) {
			pos.x += x;
			pos.y += y;
			radius = _radius;
		}

		void draw() {
			ofSetColor(color);

			
			if (idx == 0) { aramaki.draw(pos, radius, radius); }
			else if (idx == 1) { teshima.draw(pos, radius, radius); }
			else { maeda.draw(pos, radius, radius); }
//			ofCircle(pos.x, pos.y, radius);
		}
	};

	deque<CosmoUnit> cosmos;

	Cosmo() {};
	~Cosmo() {};

	void setup(int cosmoUnitNum ){
		CosmoUnit::loadImage();
		for (int i = 0; i < cosmoUnitNum;i++) {
			CosmoUnit cu;
			cu.setup(ofRandom(ofGetWindowWidth()), ofRandom(ofGetWindowHeight()), ofRandom(5.0, 50.0), ofColor(255, 255, 255, ofRandom(10, 255)));
			cosmos.push_back(cu);
		}
	}

	void update(const ofPoint& center) {

		for (int i = 0, len = cosmos.size(); i < len; ++i) {
			ofVec2f directionVec = center - cosmos[i].pos;
			float dist = center.distance(cosmos[i].pos);
			directionVec.normalize();
			cosmos[i].update(directionVec.x / dist *1000, directionVec.y / dist *1000, cosmos[i].radius);
		}

	}

	void draw() {
		for (int i = 0, len = cosmos.size(); i < len; ++i) {
			cosmos[i].draw();
		}
	}
};

