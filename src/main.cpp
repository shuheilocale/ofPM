#pragma warning(disable:4819)
#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
	//ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofAppGLFWWindow win;
	win.setMultiDisplayFullscreen(true);
	ofSetupOpenGL(640, 480, OF_WINDOW);
	//ofSetupOpenGL(&win, 640, 480, OF_FULLSCREEN);
	ofRunApp(new ofApp());

}
