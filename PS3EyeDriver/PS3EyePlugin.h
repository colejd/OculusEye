//
//  PS3EyePlugin.h
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//
//  Uses code from http://docs.unity3d.com/Manual/NativePluginInterface.html

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
-(void) setGain: (Byte) gain;
-(void) setSharpness: (Byte) sharpness;
-(void) setExposure: (Byte) exposure;
-(void) setBrightness: (Byte) brightness;
-(void) setContrast: (Byte) contrast;
-(void) setHue: (Byte) hue; //huehuehuehuehue
-(void) setBlueBalance: (Byte) blueBalance;
-(void) setRedBalance: (Byte) redBalance;

/*------ Getters -----*/
-(bool) getAutoWhiteBalance;
-(bool) getAutoGain;
-(Byte) getGain;
-(Byte) getSharpness;
-(Byte) getExposure;
-(Byte) getBrightness;
-(Byte) getContrast;
-(Byte) getHue;
-(Byte) getBlueBalance;
-(Byte) getRedBalance;

@end