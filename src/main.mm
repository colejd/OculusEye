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
int main(){
    
    //ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    ofSetupOpenGL(1280, 800, OF_WINDOW); // Can be OF_WINDOW, OF_FULLSCREEN, or OF_GAME_MODE
    
    //Possible fix for ATI cards
    glewExperimental= GL_TRUE;
    
    mainApp * app = new mainApp();
    
    
    // Make borderless and fullscreen
    // see https://gist.github.com/ofZach/7808368
    /*
    NSWindow *window = (NSWindow *)ofGetCocoaWindow();
    [window setStyleMask:NSBorderlessWindowMask];
    [window setLevel:NSFloatingWindowLevel];
    window.level = NSMainMenuWindowLevel + 1;
    //Give the window focus
    [window makeKeyWindow];
    //ofSetWindowPosition(0,0);
    ofSetWindowShape(1280, 800);
    ofSetWindowPosition(0, 0);
     */
    
    
    // Kick off the running of the app
    ofRunApp(app);
    
    
    return 0;

}
