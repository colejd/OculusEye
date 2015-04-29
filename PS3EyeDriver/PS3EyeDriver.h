//
//  PS3EyeDriver.h
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//
//  Uses code from http://docs.unity3d.com/Manual/NativePluginInterface.html

/**
 * Code for pulling camera data from the PS3 Eyes and giving them
 * to the outer interface (PS3EyePlugin).
 */

#ifndef __OculusEye__PS3EyeDriver__
#define __OculusEye__PS3EyeDriver__

#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#include "ps3eye.h"
#include "YUVBuffer.h"

#include "PS3EyeMisc.h"

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480
#define CAMERA_FPS 60

class PS3EyeDriver {
public:
    typedef void (*FuncPtr)( const char * );
    FuncPtr Log;
    
    PS3EyeDriver(FuncPtr logPtr);
    ~PS3EyeDriver();
    
    void Init();
    
    void StartCameraUpdateThread();
    void StopCameraUpdateThread();
    pthread_t cameraThreadID;
    pthread_attr_t cameraThreadAttr;
    size_t stackSize;
    
    int cameraThreadRetVal;
    int cameraThreadErr;
    bool cameraThreadStarted = false;
    
    //Raw video data array from PS Eye (RGBA888 color)
    unsigned char * rawPixelData_Left;
    unsigned char * rawPixelData_Right;
    
    ps3eye::PS3EYECam::PS3EYERef leftEyeRef;
    ps3eye::PS3EYECam::PS3EYERef rightEyeRef;
    
    //bool camerasInitialized = false;
    
    void PullData_Left();
    void PullData_Right();
    
    static int GetNumCameras();
    
    YUVBuffer yuvData_left;
    YUVBuffer yuvData_right;
    
    bool leftEyeInitialized = false;
    bool rightEyeInitialized = false;
    
    void setAutoGain(bool autoGain, EyeType whichSide);
    void setAutoWhiteBalance(bool autoWhiteBalance, EyeType whichSide);
    void setGain(float gain, EyeType whichSide);
    void setSharpness(float sharpness, EyeType whichSide);
    void setExposure(float exposure, EyeType whichSide);
    void setBrightness(float brightness, EyeType whichSide);
    void setContrast(float contrast, EyeType whichSide);
    void setHue(float hue, EyeType whichSide);
    void setBlueBalance(float blueBalance, EyeType whichSide);
    void setRedBalance(float redBalance, EyeType whichSide);
    
    void CameraPollThread();
    
private:
    
    
};

//This actually does get used.
static void *CameraUpdateThread(void *arg);

#endif /* defined(__OculusEye__PS3EyeDriver__) */
