//
//  PS3EyePlugin.h
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#import <Foundation/Foundation.h>

#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "PS3EyeDriver.h"

//C++ ---------------------------------------------------------
//Unity3D hooks
void _InitDriver();
void _Begin();
void _End();
int _GetNumCameras();

//Objective-C --------------------------------------------------

@interface PS3EyePlugin : NSObject{
    PS3EyeDriver *driver;
}

+(PS3EyePlugin*) sharedInstance;
-(void)InitDriver;
-(void)Begin;
-(void)End;
-(int)GetNumCameras;

@end