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
#include <Cocoa/Cocoa.h>

//========================================================================
int main(){
    
    //ofAppGlutWindow window;
    ////ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	//ofSetupOpenGL(&window, 1280, 800, OF_WINDOW);
    
    
    //ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    ofSetupOpenGL(1280, 800, OF_WINDOW); // Can be OF_WINDOW, OF_FULLSCREEN, or OF_GAME_MODE
    
    //Possible fix for ATI cards
    glewExperimental= GL_TRUE;
    
    mainApp * app = new mainApp();

    
    // Make borderless and (nearly) fullscreen
    NSWindow *window = (NSWindow *)ofGetCocoaWindow();
    [window setStyleMask:NSBorderlessWindowMask];
    [window setLevel:NSFloatingWindowLevel];
    window.level = NSMainMenuWindowLevel + 1;
    //Give the window focus
    [window makeKeyWindow];
    //ofSetWindowPosition(0,0);
    ofSetWindowShape(1280, 800);
    
    
    // Kick off the running of the app
    ofRunApp(app);
    
    
    return 0;

}
