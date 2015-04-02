//
//  PS3EyeDriver.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#include "PS3EyeDriver.h"

PS3EyeDriver::PS3EyeDriver(){
    printf("[PS3EyeDriver] Driver starting...\n");
    
    rawPixelData_Left = (unsigned char*)malloc(CAMERA_WIDTH*CAMERA_HEIGHT*3*sizeof(unsigned char));
    rawPixelData_Right = (unsigned char*)malloc(CAMERA_WIDTH*CAMERA_HEIGHT*3*sizeof(unsigned char));
    
    yuvData_left = YUVBuffer();
    yuvData_right = YUVBuffer();
    
    //Last
    printf("[PS3EyeDriver] Driver started.\n");
    
}

PS3EyeDriver::~PS3EyeDriver(){
    printf("[PS3EyeDriver] Driver stopping...\n");
    StopCameraUpdateThread();
    
    free(rawPixelData_Left);
    free(rawPixelData_Right);
    
    //Last
    printf("[PS3EyeDriver] Driver stopped.\n");
    
}

void PS3EyeDriver::Init(){
    printf("[PS3EyeDriver] Initializing...\n");
    //Start polling the cameras
    StartCameraUpdateThread();
    
    //Initialize the cameras
    //initialized = false;
    
    using namespace ps3eye;
    // list out the devices
    std::vector<PS3EYECam::PS3EYERef> devices( PS3EYECam::getDevices(true) );
    
    if(GetNumCameras() > 0){
        leftEyeRef = devices.at(0);
        bool eyeDidInit = leftEyeRef->init(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_FPS);
        leftEyeRef->start();
        printf("[PS3EyeDriver] Left camera initialized: %s\n", eyeDidInit ? "Yes" : "No");
        leftEyeInitialized = true;
    }
    if(GetNumCameras() > 1){
        rightEyeRef = devices.at(0);
        bool eyeDidInit = rightEyeRef->init(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_FPS);
        rightEyeRef->start();
        printf("[PS3EyeDriver] Right camera initialized: %s\n", eyeDidInit ? "Yes" : "No");
        rightEyeInitialized = true;
    }
    
    printf("[PS3EyeDriver] Driver init over.\n");
    
}

/**
 * Puts RGB888 data into rawPixelData to prepare it for OpenCV processing.
 */
void PS3EyeDriver::PullData_Left(){
    if(leftEyeRef){
        printf("%i\n", leftEyeRef->getLastFramePointer()[0]);
        yuvData_left.Convert(leftEyeRef->getLastFramePointerVolatile(), leftEyeRef->getRowBytes(), rawPixelData_Left, leftEyeRef->getWidth(), leftEyeRef->getHeight());
    }
}

void PS3EyeDriver::PullData_Right(){
    if(rightEyeRef){
        yuvData_right.Convert(rightEyeRef->getLastFramePointerVolatile(), rightEyeRef->getRowBytes(), rawPixelData_Right, rightEyeRef->getWidth(), rightEyeRef->getHeight());
    }
}


//Stack size is 1 MB * 16
#define REQUIRED_STACK_SIZE 1024*1024*16
void PS3EyeDriver::StartCameraUpdateThread(){
    if(!cameraThreadStarted){
        cameraThreadRetVal = pthread_attr_init(&cameraThreadAttr);
        assert(!cameraThreadRetVal);
        
        cameraThreadErr = pthread_attr_getstacksize(&cameraThreadAttr, &stackSize);
        //assert(!cameraThreadErr);
        if(stackSize < REQUIRED_STACK_SIZE){
            cameraThreadErr = pthread_attr_setstacksize(&cameraThreadAttr, REQUIRED_STACK_SIZE);
            printf("[PS3EyeDriver] Resized the stack\n");
        }
        
        cameraThreadRetVal = pthread_attr_setdetachstate(&cameraThreadAttr, PTHREAD_CREATE_DETACHED);
        assert(!cameraThreadRetVal);
        
        cameraThreadErr = pthread_create(&cameraThreadID, &cameraThreadAttr, &CameraUpdateThread, NULL);
        cameraThreadStarted = true;
    }
    else{
        printf("[PS3EyeDriver] Couldn't start the camera update thread as it was already started.\n");
    }
    
}

void PS3EyeDriver::StopCameraUpdateThread(){
    if(cameraThreadStarted){
        leftEyeRef->stop();
        rightEyeRef->stop();
        //pthread_cancel(cameraThreadID);
        cameraThreadRetVal = pthread_attr_destroy(&cameraThreadAttr);
        //cameraThreadRetVal = pthread_join(cameraThreadID, NULL);
        assert(!cameraThreadRetVal);
        if (cameraThreadErr != 0)
        {
            // Report an error.
            printf("[PS3EyeDriver] Couldn't stop the thread...\n");
        }
        else{
            printf("[PS3EyeDriver] Stopped the camera thread.\n");
        }
        cameraThreadStarted = false;
    }
    else{
        printf("[PS3EyeDriver] Couldn't stop camera update thread as it was already stopped.\n");
    }
}

void *CameraUpdateThread(void *arg){
    while(true){
        //lock();
        //printf("Update\n");
        bool res = ps3eye::PS3EYECam::updateDevices();
        if(!res)
        {
            printf("[PS3EyeDriver] Thread has stopped");
            break;
        }
        //unlock();
    }
    return NULL;
}

int PS3EyeDriver::GetNumCameras(){
    std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices(true) );
    return (int)devices.size();
}

