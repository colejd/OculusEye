#pragma once

#include "ofMain.h"
//#include "ofxOpenCv.h"
//#include "ofxSimpleGuiToo.h"
#include "ofxOculusRift.h"

#include "ps3Eye/ps3eye.h"
#include "screenConfig.h"
//#include "OFCVBridge.h"

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/core/core.hpp"
#include <opencv2/core/ocl.hpp>
//#include "opencv2/gpu/gpu.hpp"

#include <OpenCL/opencl.h>

#include "ofxTweak.h"

#include "CVEye.h"

class ps3eyeUpdate : public ofThread{
    
public:
    ps3eyeUpdate(){
    }
    
    void start(){
        startThread(false);   // blocking, verbose
    }
    
    void stop(){
        stopThread();
    }
    
    //--------------------------
    void threadedFunction()
    {
        while( isThreadRunning() != 0 )
        {
            lock();
            bool res = ps3eye::PS3EYECam::updateDevices();
            if(!res)
            {
                ofLog(OF_LOG_WARNING, "Thread has stopped");
                break;
            }
            unlock();
        }
    }
};

class mainApp : public ofBaseApp{
public:
    //OpenFrameworks------------------------------
    void setup();
    void exit();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void CreateGUI();

    ofImage leftVideoImage, leftFinalImage;
    ofImage rightVideoImage, rightFinalImage;
    
    bool drawGuides;
    bool duplicateEyes;

    //PS3 Eye--------------
    void InitEyes();
    ofImage& getImageForSide(const bool isLeft);
    ps3eyeUpdate threadUpdate;
    int cam_width, cam_height;
    //FPS variables
    int camFrameCount = 0;
    int camFpsLastSampleFrame = 0;
    float camFpsLastSampleTime = 0;
    float camFps = 0;
    //Camera controls
    bool autoWhiteBalance;
    bool autoGain;
    float gain;
    float sharpness;
    float exposure;
    float brightness;
    float contrast;
    float hue;
    float blueBalance;
    float redBalance;

    //Oculus Rift--------------
    ofxOculusRift oculusRift;
    bool doRiftDistortion;
    bool swapEyes;

    //OpenCV--------------------
    float cannyRatio = 3.0f;
    //Raise/lower this number to change Canny sensitivity
    float cannyMinThreshold = 50.0f;
    bool doCanny;
    bool showEdgesOnly;
    //The color specified by the GUI color picker
    float pickerColor[4];
    //Thickness of edges. Set to -1 to have OpenCV try to fill closed contours.
    int edgeThickness = 2;
    bool doErosionDilution;
    int erosionIterations = 1;
    int dilutionIterations = 1;
    float downsampleRatio = 0.5f;
    //Auto-adjustment of downsampleRatio based on framerate
    bool adaptiveDownsampling = false;
    int imageSubdivisions = 1;
    //If false, only canny output is drawn
    bool drawContours = true;

    bool renderLeftEye = true;
    bool renderRightEye = true;

    CVEye *leftEye;
    CVEye *rightEye;

    void UpdateEyeValues(CVEye *eye);
    void UpdateEyeCamera(CVEye *eye);
    
    ofxTweakbar *generalSettingsBar;
    ofxTweakbarSimpleStorage *generalSettingsStorage;
    
    ofxTweakbar *ps3EyeSettings;
    ofxTweakbarSimpleStorage *ps3EyeSettingsStorage;
    
private:
    string GetStdoutFromCommand(string cmd);
};
