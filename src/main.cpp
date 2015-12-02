#pragma warning(disable:4819)
#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofSetupOpenGL(new ofAppGlutWindow, 800, 600, OF_WINDOW);
	ofRunApp(new ofApp());

}
