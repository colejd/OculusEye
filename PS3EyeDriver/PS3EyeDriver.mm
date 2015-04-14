//
//  PS3EyeDriver.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#include "PS3EyeDriver.h"

PS3EyeDriver::PS3EyeDriver(FuncPtr logPtr){
    Log = logPtr;
    Log("[PS3EyeDriver] Creating driver...");
    //printf("[PS3EyeDriver] Driver starting...\n");
    
    rawPixelData_Left = (unsigned char*)malloc(CAMERA_WIDTH*CAMERA_HEIGHT*3*sizeof(unsigned char));
    rawPixelData_Right = (unsigned char*)malloc(CAMERA_WIDTH*CAMERA_HEIGHT*3*sizeof(unsigned char));
    
    yuvData_left = YUVBuffer();
    yuvData_right = YUVBuffer();
    
    //Last
    Log("[PS3EyeDriver] Driver created.");
    
}

PS3EyeDriver::~PS3EyeDriver(){
    Log("[PS3EyeDriver] Driver stopping...");
    StopCameraUpdateThread();
    
    free(rawPixelData_Left);
    free(rawPixelData_Right);
    
    //Last
    Log("[PS3EyeDriver] Driver stopped.");
    
}

void PS3EyeDriver::Init(){
    Log("[PS3EyeDriver] Initializing...");
    //Start polling the cameras
    StartCameraUpdateThread();
    
    //Initialize the cameras
    //initialized = false;
    //if(cameraThreadStarted) Log("The thread is started.");
    
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
        rightEyeRef = devices.at(1);
        bool eyeDidInit = rightEyeRef->init(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_FPS);
        rightEyeRef->start();
        printf("[PS3EyeDriver] Right camera initialized: %s\n", eyeDidInit ? "Yes" : "No");
        rightEyeInitialized = true;
    }
    
    Log("[PS3EyeDriver] Driver initialized.");
    
}

/**
 * Puts RGB888 data into rawPixelData to prepare it for OpenCV processing.
 */
void PS3EyeDriver::PullData_Left(){
    if(leftEyeRef){
        //printf("%X\n", (int)(leftEyeRef->getLastFramePointerVolatile()[0]));
        yuvData_left.Convert(leftEyeRef->getLastFramePointer(), leftEyeRef->getRowBytes(), rawPixelData_Left, leftEyeRef->getWidth(), leftEyeRef->getHeight());
    }
}

void PS3EyeDriver::PullData_Right(){
    if(rightEyeRef){
        yuvData_right.Convert(rightEyeRef->getLastFramePointer(), rightEyeRef->getRowBytes(), rawPixelData_Right, rightEyeRef->getWidth(), rightEyeRef->getHeight());
    }
}


//Stack size is 1 MB * 16
#define REQUIRED_STACK_SIZE 1024*1024*16
void PS3EyeDriver::StartCameraUpdateThread(){
    if(!cameraThreadStarted){
        Log("[PS3EyeDriver] Starting the polling thread.");
        /*
        cameraThreadRetVal = pthread_attr_init(&cameraThreadAttr);
        //assert(!cameraThreadRetVal);
        
        cameraThreadErr = pthread_attr_getstacksize(&cameraThreadAttr, &stackSize);
        //assert(!cameraThreadErr);
        if(stackSize < REQUIRED_STACK_SIZE){
            cameraThreadErr = pthread_attr_setstacksize(&cameraThreadAttr, REQUIRED_STACK_SIZE);
            Log("[PS3EyeDriver] Resized the stack");
        }
        
        cameraThreadRetVal = pthread_attr_setdetachstate(&cameraThreadAttr, PTHREAD_CREATE_DETACHED);
        //assert(!cameraThreadRetVal);
        
        cameraThreadErr = pthread_create(&cameraThreadID, &cameraThreadAttr, &CameraUpdateThread, NULL);
         */
        
        //http://www.boost.org/doc/libs/1_54_0/doc/html/thread/thread_management.html#thread.thread_management.tutorial.launching
        boost::thread::attributes attrs;
        // set portable attributes
        // ...
        attrs.set_stack_size(4096*10);
        #if defined(BOOST_THREAD_PLATFORM_WIN32)
                // ... window version
        #elif defined(BOOST_THREAD_PLATFORM_PTHREAD)
                // ... pthread version
                pthread_attr_setschedpolicy(attrs.native_handle(), SCHED_RR);
        #else
        #error "Boost threads unavailable on this platform"
        #endif
        
        cameraPollingThread = boost::thread(attrs, boost::bind(&PS3EyeDriver::CameraPollThread, this));
        cameraPollingThread.detach();
        
        //cameraPollingThread = boost::thread(&PS3EyeDriver::CameraPollThread, this);
        
        Log("[PS3EyeDriver] Finished starting the polling thread.");
    }
    else{
        Log("[PS3EyeDriver] Couldn't start the camera update thread as it was already started.");
    }
    
}

void PS3EyeDriver::StopCameraUpdateThread(){
    Log("[PS3EyeDriver] Stopping camera update thread...");
    if(cameraThreadStarted){
        //Stop the cameras first. Very important!
        Log("[PS3EyeDriver] Stopping USB connections...");
        leftEyeRef.reset(); //Calls destructor for leftEyeRef
        rightEyeRef.reset(); //Calls destructor for rightEyeRef
        Log("[PS3EyeDriver] USB connections stopped.");
        /*
        //pthread_cancel(cameraThreadID);
        cameraThreadRetVal = pthread_attr_destroy(&cameraThreadAttr);
        //cameraThreadRetVal = pthread_join(cameraThreadID, NULL);
        //assert(!cameraThreadRetVal);
        if (cameraThreadErr != 0)
        {
            // Report an error.
            Log("[PS3EyeDriver] Couldn't stop the thread...");
        }
        else{
            Log("[PS3EyeDriver] Stopped the camera thread.");
        }
         */
        cameraPollingThread.interrupt();
        cameraPollingThread.join();
        cameraThreadStarted = false;
        Log("[PS3EyeDriver] Camera update thread stopped.");
    }
    else{
        Log("[PS3EyeDriver] Couldn't stop camera update thread as it is not running.");
    }
}

/*
static void *CameraUpdateThread(void *arg){
    while(true){
        //lock();
        //printf("Update\n");
        bool res = ps3eye::PS3EYECam::updateDevices();
        if(!res)
        {
            printf("[PS3EyeDriver] Thread has stopped.");
            break;
        }
        //unlock();
    }
    return NULL;
}
 */

void PS3EyeDriver::CameraPollThread(){
    cameraThreadStarted = true;
    while(true){
        try{
            boost::this_thread::interruption_point();
            if(boost::this_thread::interruption_requested()){
                //Log("Interrupt requested.");
                break;
            }
            //lock();
            //printf("Update\n");
            bool res = ps3eye::PS3EYECam::updateDevices();
            //Log("[PS3EyeDriver] Thread update");
            if(!res)
            {
                break;
            }
            //unlock();
        }
        catch(boost::thread_interrupted&){
            //Log("[PS3EyeDriver] Polling thread interrupted.");
            break;
        }
    }
    //Log("[PS3EyeDriver] Polling thread has stopped.");
}

int PS3EyeDriver::GetNumCameras(){
    std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices(true) );
    return (int)devices.size();
}


void PS3EyeDriver::setAutoGain(bool autoGain, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef->setAutogain(autoGain);
        rightEyeRef->setAutogain(autoGain);
    }
}

void PS3EyeDriver::setAutoWhiteBalance(bool autoWhiteBalance, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef->setAutoWhiteBalance(autoWhiteBalance);
        rightEyeRef->setAutoWhiteBalance(autoWhiteBalance);
    }
}

void PS3EyeDriver::setGain(float gain, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef-> setGain(gain);
        rightEyeRef-> setGain(gain);
    }
}

void PS3EyeDriver::setSharpness(float sharpness, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef-> setSharpness(sharpness);
        rightEyeRef-> setSharpness(sharpness);
    }
}

void PS3EyeDriver::setExposure(float exposure, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef->setExposure(exposure);
        rightEyeRef->setExposure(exposure);
    }
}

void PS3EyeDriver::setBrightness(float brightness, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef->setBrightness(brightness);
        rightEyeRef->setBrightness(brightness);
    }
}

void PS3EyeDriver::setContrast(float contrast, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef->setContrast(contrast);
        rightEyeRef->setContrast(contrast);
    }
}

void PS3EyeDriver::setHue(float hue, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef->setHue(hue);
        rightEyeRef->setHue(hue);
    }
}

void PS3EyeDriver::setBlueBalance(float blueBalance, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef->setBlueBalance(blueBalance);
        rightEyeRef->setBlueBalance(blueBalance);
    }
}

void PS3EyeDriver::setRedBalance(float redBalance, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        leftEyeRef->setRedBalance(redBalance);
        rightEyeRef->setRedBalance(redBalance);
    }
}






