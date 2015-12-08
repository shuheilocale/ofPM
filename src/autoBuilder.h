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
			char fname[_MAX_FNAME];
			//printf("%s\n", fname);
			int number = ofRandom(1,10);
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

	void setup( int num, const ofPoint& pos, bool R2L ) {
		if (num == 0) return;
		
		startFrame = ofGetFrameNum();
		delay = 10;

		float step = pos.x / num;
		for (int i = 0, len = num; i < len;++i) {
			Building b;
			
			b.setup(ofPoint(pos.x - step*i + ofRandom(-10.0,10.0), pos.y ), ofRandom(0.1,0.5), 0 ); // 仮ぎめ
			buildings.push_back(b);
		}


		if (R2L) {
			fd.setup(pos, ofPoint(0,0, pos.y), true);
		}
		else {
			fd.setup(ofPoint(0, 0, pos.y), pos, false);
		}
	}

	void refresh(const ofPoint& pos, bool R2L) {
		startFrame = ofGetFrameNum();
		int num = buildings.size();
		buildings.clear();

		float step = pos.x / num;
		for (int i = 0, len = num; i < len;++i) {
			Building b;

			b.setup(ofPoint(pos.x - step*i + ofRandom(-10.0, 10.0), pos.y), ofRandom(0.1, 0.5), 0); // 仮ぎめ
			buildings.push_back(b);
		}

		fd.reflesh(pos, R2L);
		
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
		for (int i = 0, len = buildings.size(); i < len; ++i) {
			buildings[i].draw();
		}
		fd.draw();
	}

	deque<Building> buildings;
	int     delay;
	uint64_t startFrame;
	FugitiveDust fd;
};

