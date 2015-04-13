//
//  PS4EyeDriver.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#include "PS4EyeDriver.h"

PS4EyeDriver::PS4EyeDriver(){
    printf("[PS4EyeDriver] Driver starting...\n");
    
    rawPixelData_Left = (unsigned char*)malloc(CAMERA_WIDTH*CAMERA_HEIGHT*3*sizeof(unsigned char));
    rawPixelData_Right = (unsigned char*)malloc(CAMERA_WIDTH*CAMERA_HEIGHT*3*sizeof(unsigned char));
    
    //yuvData_left = YUVBuffer();
    //yuvData_right = YUVBuffer();
    Init();
    
    //Last
    printf("[PS4EyeDriver] Driver started.\n");
    
}

PS4EyeDriver::~PS4EyeDriver(){
    printf("[PS4EyeDriver] Driver stopping...\n");
    StopCameraUpdateThread();
    
    free(rawPixelData_Left);
    free(rawPixelData_Right);
    
    //Last
    printf("[PS4EyeDriver] Driver stopped.\n");
    
}

void PS4EyeDriver::Init(){
    printf("[PS4EyeDriver] Initializing...\n");
    //Start polling the cameras
    StartCameraUpdateThread();
    
    //Initialize the cameras
    //initialized = false;
    
    using namespace ps4eye;
    // list out the devices
    std::vector<PS4EYECam::PS4EYERef> devices( PS4EYECam::getDevices(true) );
    printf("Actual devices: %i\n", (int)devices.size());
    
    if(GetNumCameras() > 0){
        eyeRef = devices.at(0);
        //Mode 1 (640x480), 30 FPS
        bool eyeDidInit = eyeRef->init(1, 30);
        eyeRef->start();
        printf("[PS4EyeDriver] Camera initialized: %s\n", eyeDidInit ? "Yes" : "No");
        eyeRef->set_led_on();
        eyeInitialized = true;
    }
    
    printf("[PS4EyeDriver] Driver init over.\n");
    
}

/**
 * Puts RGB888 data into rawPixelData to prepare it for OpenCV processing.
 */
void PS4EyeDriver::PullData(){
    if(eyeRef){
        eyeRef->check_ff71();
        eyeframe *frame = eyeRef->getLastVideoFramePointer();
        //printf("%X\n", (int)(leftEyeRef->getLastFramePointerVolatile()[0]));
        //yuvData_left.Convert(frame->videoLeftFrame, frame->get leftEyeRef->getRowBytes(), rawPixelData_Left, leftEyeRef->getWidth(), leftEyeRef->getHeight());
        yuyvToRgb(frame->videoLeftFrame, rawPixelData_Left, eyeRef->getWidth(), eyeRef->getHeight());
        yuyvToRgb(frame->videoRightFrame, rawPixelData_Right, eyeRef->getWidth(), eyeRef->getHeight());
    }
}
/*
void PS4EyeDriver::PullData_Right(){
    
    if(rightEyeRef){
        yuvData_right.Convert(rightEyeRef->getLastFramePointer(), rightEyeRef->getRowBytes(), rawPixelData_Right, rightEyeRef->getWidth(), rightEyeRef->getHeight());
    }
 
}
 */


//Stack size is 1 MB * 16
#define REQUIRED_STACK_SIZE 1024*1024*16
void PS4EyeDriver::StartCameraUpdateThread(){
    if(!cameraThreadStarted){
        cameraThreadRetVal = pthread_attr_init(&cameraThreadAttr);
        assert(!cameraThreadRetVal);
        
        cameraThreadErr = pthread_attr_getstacksize(&cameraThreadAttr, &stackSize);
        //assert(!cameraThreadErr);
        if(stackSize < REQUIRED_STACK_SIZE){
            cameraThreadErr = pthread_attr_setstacksize(&cameraThreadAttr, REQUIRED_STACK_SIZE);
            printf("[PS4EyeDriver] Resized the stack\n");
        }
        
        cameraThreadRetVal = pthread_attr_setdetachstate(&cameraThreadAttr, PTHREAD_CREATE_DETACHED);
        assert(!cameraThreadRetVal);
        
        cameraThreadErr = pthread_create(&cameraThreadID, &cameraThreadAttr, &CameraUpdateThread, NULL);
        cameraThreadStarted = true;
    }
    else{
        printf("[PS4EyeDriver] Couldn't start the camera update thread as it was already started.\n");
    }
    
}

void PS4EyeDriver::StopCameraUpdateThread(){
    if(cameraThreadStarted){
        if(eyeRef) eyeRef->shutdown();
        printf("[PS4EyeDriver] USB Connections stopped.\n");
        //pthread_cancel(cameraThreadID);
        cameraThreadRetVal = pthread_attr_destroy(&cameraThreadAttr);
        //cameraThreadRetVal = pthread_join(cameraThreadID, NULL);
        assert(!cameraThreadRetVal);
        if (cameraThreadErr != 0)
        {
            // Report an error.
            printf("[PS4EyeDriver] Couldn't stop the thread...\n");
        }
        else{
            printf("[PS4EyeDriver] Stopped the camera thread.\n");
        }
        cameraThreadStarted = false;
    }
    else{
        printf("[PS4EyeDriver] Couldn't stop camera update thread as it was already stopped.\n");
    }
}

void *CameraUpdateThread(void *arg){
    while(true){
        //lock();
        //printf("Update\n");
        bool res = ps4eye::PS4EYECam::updateDevices();
        if(!res)
        {
            printf("[PS4EyeDriver] Thread has stopped");
            break;
        }
        //unlock();
    }
    return NULL;
}

int PS4EyeDriver::GetNumCameras(){
    std::vector<ps4eye::PS4EYECam::PS4EYERef> devices( ps4eye::PS4EYECam::getDevices(true) );
    return (int)devices.size();
}


void PS4EyeDriver::setAutoGain(bool autoGain, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef->setAutogain(autoGain);
        //rightEyeRef->setAutogain(autoGain);
    }
}

void PS4EyeDriver::setAutoWhiteBalance(bool autoWhiteBalance, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef->setAutoWhiteBalance(autoWhiteBalance);
        //rightEyeRef->setAutoWhiteBalance(autoWhiteBalance);
    }
}

void PS4EyeDriver::setGain(float gain, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef-> setGain(gain);
        //rightEyeRef-> setGain(gain);
    }
}

void PS4EyeDriver::setSharpness(float sharpness, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef-> setSharpness(sharpness);
        //rightEyeRef-> setSharpness(sharpness);
    }
}

void PS4EyeDriver::setExposure(float exposure, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef->setExposure(exposure);
        //rightEyeRef->setExposure(exposure);
    }
}

void PS4EyeDriver::setBrightness(float brightness, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef->setBrightness(brightness);
        //rightEyeRef->setBrightness(brightness);
    }
}

void PS4EyeDriver::setContrast(float contrast, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef->setContrast(contrast);
        //rightEyeRef->setContrast(contrast);
    }
}

void PS4EyeDriver::setHue(float hue, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef->setHue(hue);
        //rightEyeRef->setHue(hue);
    }
}

void PS4EyeDriver::setBlueBalance(float blueBalance, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef->setBlueBalance(blueBalance);
        //rightEyeRef->setBlueBalance(blueBalance);
    }
}

void PS4EyeDriver::setRedBalance(float redBalance, EyeType whichSide){
    if(whichSide == LEFT_EYE){ }
    else if(whichSide == RIGHT_EYE){ }
    else if(whichSide == BOTH_EYES){
        //leftEyeRef->setRedBalance(redBalance);
        //rightEyeRef->setRedBalance(redBalance);
    }
}

//from https://social.msdn.microsoft.com/forums/windowsdesktop/en-us/1071301e-74a2-4de4-be72-81c34604cde9/program-to-translate-yuyv-to-rgbrgb modified yuyv order
/*--------------------------------------------------------*\
 |    yuv2rgb                                               |
 \*--------------------------------------------------------*/
void PS4EyeDriver::yuv2rgb(int y, int u, int v, char *r, char *g, char *b)
{
    int r1, g1, b1;
    int c = y-16, d = u - 128, e = v - 128;
    
    r1 = (298 * c           + 409 * e + 128) >> 8;
    g1 = (298 * c - 100 * d - 208 * e + 128) >> 8;
    b1 = (298 * c + 516 * d           + 128) >> 8;
    
    // Even with proper conversion, some values still need clipping.
    
    if (r1 > 255) r1 = 255;
    if (g1 > 255) g1 = 255;
    if (b1 > 255) b1 = 255;
    if (r1 < 0) r1 = 0;
    if (g1 < 0) g1 = 0;
    if (b1 < 0) b1 = 0;
    
    *r = r1 ;
    *g = g1 ;
    *b = b1 ;
}

/*--------------------------------------------------------*\
 |    yuyvToRgb                                             |
 \*--------------------------------------------------------*/
void PS4EyeDriver::yuyvToRgb(uint8_t *in,uint8_t *out, int size_x,int size_y)
{
    int i;
    unsigned int *pixel_16=(unsigned int*)in;;     // for YUYV
    unsigned char *pixel_24=out;    // for RGB
    int y, u, v, y2;
    char r, g, b;
    
    
    for (i=0; i< (size_x*size_y/2) ; i++)
    {
        // read YuYv from newBuffer (2 pixels) and build RGBRGB in pBuffer
        
        //   v  = ((*pixel_16 & 0x000000ff));
        // y  = ((*pixel_16 & 0x0000ff00)>>8);
        // u  = ((*pixel_16 & 0x00ff0000)>>16);
        // y2 = ((*pixel_16 & 0xff000000)>>24);
        
        y2  = ((*pixel_16 & 0x000000ff));
        u  = ((*pixel_16 & 0x0000ff00)>>8);
        y  = ((*pixel_16 & 0x00ff0000)>>16);
        v = ((*pixel_16 & 0xff000000)>>24);
        
        yuv2rgb(y, u, v, &r, &g, &b);            // 1st pixel
        
        
        *pixel_24++ = r;
        *pixel_24++ = g;
        *pixel_24++ = b;
        
        
        
        yuv2rgb(y2, u, v, &r, &g, &b);            // 2nd pixel
        
        *pixel_24++ = r;
        *pixel_24++ = g;
        *pixel_24++ = b;
        
        
        
        pixel_16++;
    }
}






