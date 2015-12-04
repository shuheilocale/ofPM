// thanks to kimrepo
#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"


class Aura {
public:

	Aura();     // particle constructor
	virtual ~Aura() {};

	ofVec2f pos;    // vector position
	ofVec2f vel;    // vector velocity
	ofVec2f force;    // vector force

	void resetForce();
	void addDamping();                      // damping 
	void setInitCondition(float px, float py, float vx, float vy);  // starting force
	void update();
	void draw();

	void addRepulsion(float px, float py, float radius, float strength);
	void addRepulsion(ofxCvGrayscaleImage* diffImg, ofxCvBlob* blob, float radius, float strength);
	void addAttraction(float px, float py, float radius, float strength);
	void trail(float catchX, float catchY);

	float damping;
	float size;

	int r, g, b;
	float dist;
};