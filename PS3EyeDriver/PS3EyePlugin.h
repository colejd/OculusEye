//
//  PS3EyePlugin.h
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//
//  Uses code from http://docs.unity3d.com/Manual/NativePluginInterface.html

#ifndef __OculusEye__PS3EyePlugin__
#define __OculusEye__PS3EyePlugin__

/**
 * Interface into PS3 Eye code.
 */

#import <Foundation/Foundation.h>

#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "PS3EyeDriver.h"
#include "PS3EyeMisc.h"

#include <string>
#include <sstream>
#include <iostream>

//C++ ---------------------------------------------------------
//Unity3D hooks
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>

static int g_DeviceType = -1;
static void* left_TexturePointer;
static void* right_TexturePointer;

static void SetDefaultGraphicsState ();
static void DoRendering (int eventID);
//static void FillTextureFromCode (int width, int height, int stride, unsigned char* dst);
static void FillTextureFromCode(int width, int height, int stride, unsigned char* dst, EyeType side);

extern "C" {
    void InitDriver();
    void Begin();
    void End();
    int GetCameraCount();
    
    void Dealloc();

    unsigned char* GetLeftCameraData();
    unsigned char* GetRightCameraData();
    void PullData_Left();
    void PullData_Right();

    bool LeftEyeHasNewFrame();
    bool RightEyeHasNewFrame();

    bool LeftEyeInitialized();
    bool RightEyeInitialized();

    bool ThreadIsRunning();
    
    /*------ Setters -----*/
    void setAutoWhiteBalance (bool autoWhiteBalance);
    void setAutoGain (bool autoGain);
    void setGain (float gain);
    void setSharpness (float sharpness);
    void setExposure (float exposure);
    void setBrightness (float brightness);
    void setContrast (float contrast);
    void setHue (float hue); //huehuehuehuehue
    void setBlueBalance (float blueBalance);
    void setRedBalance (float redBalance);
    
    /*------ Getters -----*/
    bool getAutoWhiteBalance();
    bool getAutoGain();
    uint8_t getGain();
    uint8_t getSharpness();
    uint8_t getExposure();
    uint8_t getBrightness();
    uint8_t getContrast();
    uint8_t getHue();
    uint8_t getBlueBalance();
    uint8_t getRedBalance();
    
    void SetUnityTexturePointers(void *leftPtr, void *rightPtr);
    
}


//Objective-C --------------------------------------------------

@interface PS3EyePlugin : NSObject{
    PS3EyeDriver *driver;
}

+(PS3EyePlugin*) sharedInstance;
-(void)InitDriver;
-(void)Begin;
-(void)End;
-(int)GetCameraCount;
-(unsigned char *)GetLeftCameraData;
-(unsigned char *)GetRightCameraData;
-(void)PullData_Left;
-(void)PullData_Right;

-(bool) LeftEyeHasNewFrame;
-(bool) RightEyeHasNewFrame;

-(bool) LeftEyeInitialized;
-(bool) RightEyeInitialized;

-(bool) ThreadIsRunning;

/*------ Setters -----*/
-(void) setAutoWhiteBalance: (bool) autoWhiteBalance;
-(void) setAutoGain: (bool) autoGain;
-(void) setGain: (uint8_t) gain;
-(void) setSharpness: (uint8_t) sharpness;
-(void) setExposure: (uint8_t) exposure;
-(void) setBrightness: (uint8_t) brightness;
-(void) setContrast: (uint8_t) contrast;
-(void) setHue: (uint8_t) hue; //huehuehuehuehue
-(void) setBlueBalance: (uint8_t) blueBalance;
-(void) setRedBalance: (uint8_t) redBalance;

/*------ Getters -----*/
-(bool) getAutoWhiteBalance;
-(bool) getAutoGain;
-(uint8_t) getGain;
-(uint8_t) getSharpness;
-(uint8_t) getExposure;
-(uint8_t) getBrightness;
-(uint8_t) getContrast;
-(uint8_t) getHue;
-(uint8_t) getBlueBalance;
-(uint8_t) getRedBalance;

@end

#endif