//
//  main.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 9/25/14.
//
//

#include "ofMain.h"
#include "mainApp.h"
#include "ofAppGlutWindow.h"

//#include "ofxCocoa.h"
//#include <Cocoa/Cocoa.h>

//========================================================================
int main( ){

    
    //ofAppGlutWindow window;
    ////ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	//ofSetupOpenGL(&window, 1280, 800, OF_WINDOW);
    ofSetupOpenGL(1280, 800, OF_WINDOW); // Can be OF_WINDOW or OF_FULLSCREEN
    
    //Possible fix for ATI cards
    glewExperimental= GL_TRUE;

	// Kick off the running of the app
	ofRunApp( new mainApp());
    
    printf("App has ended.\n");
    
    
    return 0;

}
