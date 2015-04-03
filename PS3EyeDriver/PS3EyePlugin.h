//
//  PS3EyePlugin.h
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

/**
 * Interface into PS3 Eye code.
 */

#import <Foundation/Foundation.h>

#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "PS3EyeDriver.h"
#include "PS3EyeMisc.h"

//C++ ---------------------------------------------------------
//Unity3D hooks

extern "C" {
    void InitDriver();
    void Begin();
    void End();
    int GetCameraCount();
    
    void Dealloc();

    uint8_t* GetLeftCameraData();
    uint8_t* GetRightCameraData();
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
-(uint8_t *)GetLeftCameraData;
-(uint8_t *)GetRightCameraData;
-(void)PullData_Left;
-(void)PullData_Right;

-(bool) LeftEyeHasNewFrame;
-(bool) RightEyeHasNewFrame;

-(bool) LeftEyeInitialized;
-(bool) RightEyeInitialized;

-(bool) ThreadIsRunning;

-(void) setAutoWhiteBalance: (bool) autoWhiteBalance;
-(void) setAutoGain: (bool) autoGain;
-(void) setGain: (float) gain;
-(void) setSharpness: (float) sharpness;
-(void) setExposure: (float) exposure;
-(void) setBrightness: (float) brightness;
-(void) setContrast: (float) contrast;
-(void) setHue: (float) hue; //huehuehuehuehue
-(void) setBlueBalance: (float) blueBalance;
-(void) setRedBalance: (float) redBalance;

@end