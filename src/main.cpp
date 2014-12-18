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
    ofSetupOpenGL(1280, 800, OF_WINDOW);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new mainApp());
    
    printf("App has ended.\n");
    
    
    /*
    MSA::ofxCocoa::InitSettings			initSettings;
	
	initSettings.isOpaque				= true;
	initSettings.windowLevel			= NSMainMenuWindowLevel + 1;
	initSettings.hasWindowShadow		= false;
	initSettings.numFSAASamples			= false;
	initSettings.initRect.size.width	= 1280;
	initSettings.initRect.size.height	= 800;
	initSettings.windowMode				= OF_WINDOW;
	
	// to go fullscreen across all windows:
	initSettings.windowStyle			= NSBorderlessWindowMask;
	initSettings.initRect				= MSA::ofxCocoa::rectForMainScreen();
    //	initSettings.initRect				= MSA::ofxCocoa::rectForAllScreens();
	
	MSA::ofxCocoa::setSyncToDisplayLink(true); //This doesn't happen where it needs to
	MSA::ofxCocoa::AppWindow		cocoaWindow(initSettings);
	
	ofSetupOpenGL(&cocoaWindow, 1280, 800, OF_WINDOW);
	
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
	ofRunApp( new mainApp() );
     */
    
    
    return 0;

}
