#include "ofApp.h"
#include "aura.h"


Aura::Aura() {
	setInitCondition(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0, 0);
	damping = 0.09f;
	size = 5;

	float colChoice = ofRandomf();

	if (colChoice > 0) {

		r = ofRandom(0, 80);
		g = ofRandom(50, 120);
		b = ofRandom(100, 140);

	}
	else {

		r = ofRandom(210, 250);
		g = ofRandom(170, 210);
		b = ofRandom(30, 70);

	}

	r = 255;
	g = 255;
	b = 0;

}

void Aura::resetForce() {
	force.set(0, 0);
}

void Aura::addDamping() {
	force.x = force.x - vel.x * damping;
	force.y = force.y - vel.y * damping;
}

void Aura::addRepulsion(float px, float py, float radius, float strength) {

	ofVec2f posOfForce;             // vector position of force
	posOfForce.set(px, py);          // initialize

	ofVec2f diff = pos - posOfForce;    // difference

	if (diff.length() < radius) {
		float percent = 1 - (diff.length() / radius);
		diff.normalize();
		force.x += diff.x * percent * strength;
		force.y += diff.y * percent * strength;

		//		printf("forcex = %f, forcey = %f\n", force.x, force.y);
	}
}

void Aura::addAttraction(float px, float py, float radius, float strength) {

	ofVec2f posOfForce;
	posOfForce.set(px, py);

	ofVec2f diff = pos - posOfForce;

	if (diff.length() < radius) {
		float percent = 1 - (diff.length() / radius);
		diff.normalize();
		force.x -= diff.x * percent * strength;
		force.y -= diff.y * percent * strength;
	}
}

void Aura::addRepulsion(ofxCvGrayscaleImage* diffImg, ofxCvBlob* blob, float radius, float strength) {

	const unsigned char *diffPixs = diffImg->getPixels();

	ofVec2f posOfForce;

	int px = int(pos.x);
	int py = int(pos.y);

	if (px < 0) px = 0;
	if (py < 0) py = 0;

	if (px > diffImg->getWidth()) px = diffImg->getWidth() -1 ;
	if (py > diffImg->getHeight()) py = diffImg->getHeight() -1;

	unsigned char mask = diffPixs[px + py * int(diffImg->getWidth())];
	//printf("mask = %u\n", mask);

	posOfForce.set(px, py);

	ofVec2f diff = pos - posOfForce;

	if (mask == 255) {

	}
	else {
		return;
	}

	//printf("diff.length()= %f\n", diff.length());

	if (diff.length() < radius) {


		ofVec2f dist = blob->centroid - posOfForce;

		if (px < blob->centroid.x) {
			diff.x = 1;
		}
		else {
			diff.x = -1;
		}

		if (py < blob->centroid.y) {
			diff.y = 1;
		}
		else {
			diff.y = -1;
		}


		float percent = 1 - (diff.length() / radius);
		diff.normalize();
		force.x -= diff.x * percent * strength / dist.length() * 4;
		force.y -= diff.y * percent * strength / dist.length() * 4;
		//printf("forcex = %f, forcey = %f\n", force.x, force.y);
	}

}




void Aura::trail(float catchX, float catchY) {

	float catchUpSpeed = 0.5f;
	pos.x = catchUpSpeed * catchX + (1 - catchUpSpeed) * pos.x;
	pos.y = catchUpSpeed * catchY + (1 - catchUpSpeed) * pos.y;
}

void Aura::setInitCondition(float px, float py, float vx, float vy) {
	pos.set(px, py);
	vel.set(vx, vy);
}

void Aura::update() {
	vel += force;
	pos += vel;
}

void Aura::draw() {
	ofSetColor(r, g, b);
	ofCircle(pos, size);
}