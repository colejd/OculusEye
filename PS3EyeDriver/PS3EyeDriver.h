//
//  PS3EyeDriver.h
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

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
#include "OFCVBridge.h"

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480
#define CAMERA_FPS 60

class PS3EyeDriver {
public:
    PS3EyeDriver();
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
    
private:
    
    
};

void *CameraUpdateThread(void *arg);

#endif /* defined(__OculusEye__PS3EyeDriver__) */
