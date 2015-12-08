#pragma once
#include "ofMain.h"
#include <vector>

class AfterImage {
public:
	AfterImage() {}
	~AfterImage() {}

	int					nPts;
	vector <ofPoint>    pts;

	int toHexColor(int r, int g, int b) {
		int color = 0;
		color |= r << 16;
		color |= g << 8;
		color |= b << 0;
		return color;
	}

	int HexColorByFrameNum(int frameNum) {
		int color;
		frameNum *= 10;
		int fn = frameNum % 1530;
		int phase = fn / 255;  // 0-5

		switch (phase) {

		case 0:
			color = toHexColor(255, fn % 255, 0);
			break;
		case 1:
			color = toHexColor(255 - (fn % 255), 255, 0);
			break;
		case 2:
			color = toHexColor(0, 255, fn % 255);
			break;
		case 3:
			color = toHexColor(0, 255 - (fn % 255), 255);
			break;
		case 4:
			color = toHexColor(fn % 255, 0, 255);
			break;
		case 5:
			color = toHexColor(255, 0, 255 - (fn % 255));
			break;
		default:
			color = toHexColor(255, 255, 255);
			break;
		}

		return color;
	}

	void draw(int r, int g, int b) {
		ofFill();
		ofSetColor(r, g, b);
		ofBeginShape();
		for (int i = 0; i < nPts; i++) {
			ofVertex(pts[i].x, pts[i].y);
		}
		ofEndShape(true);
	}

	void draw(int frameNum) {
		ofFill();
		ofSetHexColor(HexColorByFrameNum(frameNum));
		ofBeginShape();
		for (int i = 0; i < nPts; i++) {
			ofVertex(pts[i].x, pts[i].y);
		}
		ofEndShape(true);
	}
};
