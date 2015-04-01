//
//  PS3EyeDriver.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#include "PS3EyeDriver.h"

PS3EyeDriver::PS3EyeDriver(){
    printf("Driver starting...\n");
    
    //Last
    printf("Driver started.\n");
    
}

PS3EyeDriver::~PS3EyeDriver(){
    printf("Driver stopping...\n");
    StopCameraUpdateThread();
    
    //Last
    printf("Driver stopped.\n");
    
}

void PS3EyeDriver::Init(){
    printf("Wow guys we did it\n");
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
        printf("Left camera initialized.\n");
        camerasInitialized = true;
    }
    if(GetNumCameras() > 1){
        rightEyeRef = devices.at(0);
        bool eyeDidInit = rightEyeRef->init(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_FPS);
        rightEyeRef->start();
        printf("Right camera initialized.\n");
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
            printf("Resized the stack\n");
        }
        
        cameraThreadRetVal = pthread_attr_setdetachstate(&cameraThreadAttr, PTHREAD_CREATE_JOINABLE);
        assert(!cameraThreadRetVal);
        
        cameraThreadErr = pthread_create(&cameraThreadID, &cameraThreadAttr, &CameraUpdateThread, NULL);
        cameraThreadStarted = true;
    }
    else{
        printf("Couldn't start the camera update thread as it was already started.\n");
    }
    
}

void PS3EyeDriver::StopCameraUpdateThread(){
    if(cameraThreadStarted){
        pthread_cancel(cameraThreadID);
        cameraThreadStarted = false;
    }
    else{
        printf("Couldn't stop camera update thread as it was already stopped.\n");
    }
}

void *CameraUpdateThread(void *arg){
    while(true){
        //lock();
        printf("Update\n");
        bool res = ps3eye::PS3EYECam::updateDevices();
        if(!res)
        {
            printf("Thread has stopped");
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

