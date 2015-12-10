#pragma once
#include "ofMain.h"
#include "fugitiveDust.h"
#include <deque>

using namespace std;


class AutoBuilder
{
	class Building {
	public:

		ofImage img;
		ofPoint pos;			 // 画像の左上
		ofPoint posUnderCenter;  // 画像の底辺の中心
		float   scale;
		float   deg;
		int     stopHeight;
		int     moveHeight;
		

		Building* parent;			// 使わない
		deque<Building*> children;	// 使わない

		void setup( const ofPoint& _pos_under_center, float _scale , float _deg) {
			scale = _scale;
			deg = _deg;
			// とりあえずここで対応しとくガチ書きで。
			char fname[255]; // _MAX_PATHはwin専用
			//printf("%s\n", fname);
			int number = ofRandom(1,14); // ★ファイル数分だけ変える
			sprintf(fname, "buildings/%02d.png", number);
			if( !img.loadImage(fname) )
			{
				printf("%02d.png\n", number);
			}
			img.resize(img.getWidth() * scale, img.getHeight() * scale);
			//img.setAnchorPercent(0.5, 0.5);

			// 画像の底辺の中心を、左上に変換する。
			posUnderCenter = _pos_under_center;
			pos = posUnderCenter;
			float shiftX = img.getWidth() / 2.0;
			float shiftY = img.getHeight();
			moveHeight = 0;
			stopHeight = img.getHeight();
			pos.x -= shiftX;
			pos.y -= shiftY;
			pos.y += img.getHeight();

		}

		void update() {
			//deg += 5;
			float swing = sin(pos.y) * scale*20;
			if ( moveHeight < stopHeight) {
				float step = stopHeight / 10.0;
				moveHeight += step;
				pos.x += swing;
				pos.y -= step;
			}


		}

		void draw() {

			img.draw(pos);
			//ofPushMatrix();
			//ofTranslate(-pos.x - img.getWidth() / 2, -pos.y - img.getHeight() / 2);
			//ofTranslate(posUnderCenter.x, posUnderCenter.y);

			//ofRotateZ(10);
			//ofRotate(10);
			
			//static int val = 0;
			//ofTranslate(val++, 0);
			//ofTranslate(pos);
			//ofScale(1, -1);
			//img.draw(0,0);
			//ofPopMatrix();
			//
			
			//ofTranslate(pos);
			//ofPopMatrix();
			//ofTranslate(pos);
			//ofPushMatrix();
			//img.setAnchorPercent(0.5, 0.5);
			//ofTranslate(-img.getWidth() / 2, -img.getHeight() / 2);
			//ofPopMatrix();
			//ofPopMatrix();
			
		}

	};

public:
	
	AutoBuilder();
	~AutoBuilder();

	void setup( int num, const ofPoint& posL, const ofPoint& posR, bool R2L ) {
		drawOk = posL != posR;
		if (num == 0) return;
		buildings.clear();
		startFrame = ofGetFrameNum();
		delay = 10;

		//buildings.resize(num);
		
		float step = fabs(posR.x - posL.x) / num;
		if (R2L) {
			for (int i = 0, len = num; i < len;++i) {
				Building b;
				b.setup(ofPoint(posR.x - step*i + ofRandom(-10.0, 10.0), posR.y), ofRandom(0.1, 0.5), 0); // 仮ぎめ
				buildings.push_back(b);
			}
		}else{
			for (int i = 0, len = num; i < len;++i) {
				Building b;
				b.setup(ofPoint(posL.x + step*i + ofRandom(-10.0, 10.0), posL.y), ofRandom(0.1, 0.5), 0); // 仮ぎめ
				buildings.push_back(b);
			}
		}

		fd.setup(posR, posL, true);
	}

	void refresh(const ofPoint& posL, const ofPoint& posR, bool R2L) {
		drawOk = posL != posR;
		if (buildings.empty()) return;
		int num = buildings.size();

		//buildings.clear();
		startFrame = ofGetFrameNum();
		delay = 10;

		float step = fabs(posR.x - posL.x) / num;
		if (R2L) {
			for (int i = 0, len = num; i < len;++i) {
				Building b;
				b.setup(ofPoint(posR.x - step*i + ofRandom(-10.0, 10.0), posR.y), ofRandom(0.1, 0.5), 0); // 仮ぎめ
				//buildings.push_back(b);
				buildings[i] = b;
			}
		}
		else {
			for (int i = 0, len = num; i < len;++i) {
				Building b;
				b.setup(ofPoint(posL.x + step*i + ofRandom(-10.0, 10.0), posL.y), ofRandom(0.1, 0.5), 0); // 仮ぎめ
				buildings[i] =b;
			}
		}

		fd.refresh(posL, posR, R2L);
		
	}

	void update() {

		for (int i = 0, len = buildings.size(); i < len; ++i) {
			if ( i*10 < ofGetFrameNum() - startFrame ) {
				buildings[i].update();

			}
		}
		// 砂埃は常に立っている
		fd.update();

	}

	void draw() {
		if (!drawOk) return;

		for (int i = 0, len = buildings.size(); i < len; ++i) {
			buildings[i].draw();
		}
		fd.draw();
	}

	deque<Building> buildings;
	int     delay;
	uint64_t startFrame;
	FugitiveDust fd;
	bool drawOk;
};

