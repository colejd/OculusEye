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
    StartCameraUpdateThread();
}


void PS3EyeDriver::StartCameraUpdateThread(){
    if(!cameraThreadStarted){
        cameraThreadErr = pthread_create(cameraThreadID, NULL, CameraUpdateThread, NULL);
        cameraThreadStarted = true;
    }
    else{
        printf("Couldn't start the camera update thread as it was already started.\n");
    }
    
}

void PS3EyeDriver::StopCameraUpdateThread(){
    if(cameraThreadStarted){
        pthread_cancel(*cameraThreadID);
        cameraThreadStarted = false;
    }
    else{
        printf("Couldn't stop camera update thread as it was already stopped.\n");
    }
}

int PS3EyeDriver::GetNumCameras(){
    std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices(true) );
    return (int)devices.size();
}

void *CameraUpdateThread(void *arg){
    while(true){
        //lock();
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

