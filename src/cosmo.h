#pragma once

#include "ofMain.h"
#include <deque>

using namespace std;



class Cosmo
{
public:
	class CosmoUnit {

	public:
		ofPoint pos;
		float radius;
		//int   alpha;
		ofColor color;

		CosmoUnit() {
		}

		void setup(int x, int y, float _radius, const ofColor& _color) {
			pos.x = x;
			pos.y = y;
			radius = _radius;
			//alpha = _alpha;
			color = _color;
		}

		void updateAbs(float x, float y) {
			pos.x = x;
			pos.y = y;
		}

		void update(float x, float y) {
			pos.x += x;
			pos.y += y;
		}

		void draw() {
			ofSetColor(color);
			ofCircle(pos.x, pos.y, radius);
		}
	};

	deque<CosmoUnit> cosmos;

	Cosmo() {};
	~Cosmo() {};

	void setup(int cosmoUnitNum ){
		for (int i = 0; i < cosmoUnitNum;i++) {

			CosmoUnit cu;
			cu.setup(ofRandom(ofGetWindowWidth()), ofRandom(ofGetWindowHeight()), ofRandom(0.5, 3.0), ofColor(255, 255, 255, ofRandom(10, 255)));
			cosmos.push_back(cu);
		}
	}

	void update(const ofPoint& center) {

		for (int i = 0, len = cosmos.size(); i < len; ++i) {
			ofVec2f directionVec = center - cosmos[i].pos;
			float dist = center.distance(cosmos[i].pos);
			directionVec.normalize();
			cosmos[i].update(directionVec.x / dist *1000, directionVec.y / dist *1000);
		}

	}

	void draw() {
		for (int i = 0, len = cosmos.size(); i < len; ++i) {
			cosmos[i].draw();
		}
	}
};

