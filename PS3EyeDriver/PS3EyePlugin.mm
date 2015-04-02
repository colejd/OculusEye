//
//  PS3EyePlugin.mm
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#import "PS3EyePlugin.h"

//C++ ---------------------------------------------------------

void _InitDriver(){
    [[PS3EyePlugin sharedInstance] InitDriver];
}

void _Begin(){
    [[PS3EyePlugin sharedInstance] Begin];
}

void _End(){
    [[PS3EyePlugin sharedInstance] End];
}

int _GetCameraCount(){
    return [[PS3EyePlugin sharedInstance] GetCameraCount];
}




//Objective-C --------------------------------------------------

@implementation PS3EyePlugin{
    //Private instance variables
}

-(id)init {
    if (self = [super init]) {
    }
    return self;
}

static PS3EyePlugin* sharedInstance = nil;
+(PS3EyePlugin*)sharedInstance {
    if( !sharedInstance )
        sharedInstance = [[PS3EyePlugin alloc] init];
    
    return sharedInstance;
}

-(void) InitDriver {
    driver->Init();
}

-(void) Begin {
    printf("PS3EyePlugin starting...\n");
    driver = new PS3EyeDriver();
    
    //Last
    printf("PS3EyePlugin started.\n");
}

-(void) End {
    printf("PS3EyePlugin shutting down...\n");
    delete driver;
    
    //Last
    printf("PS3EyePlugin shut down.\n");
}

-(uint8_t *)GetLeftCameraData{
    return driver->rawPixelData_Left;
}
-(uint8_t *)GetRightCameraData{
    return driver->rawPixelData_Right;
}

-(void)PullData_Left{
    driver->PullData_Left();
}

-(void)PullData_Right{
    driver->PullData_Right();
}

-(int) GetCameraCount {
    return driver->GetNumCameras();
}

-(bool) LeftEyeHasNewFrame {
    return driver->leftEyeRef->isNewFrame();
}

-(bool) RightEyeHasNewFrame {
    return driver->rightEyeRef->isNewFrame();
}

-(bool) LeftEyeInitialized {
    return (driver->leftEyeRef != NULL);
}

-(bool) RightEyeInitialized {
    return (driver->rightEyeRef != NULL);
}

-(bool) ThreadIsRunning {
    return driver->cameraThreadStarted;
}

@end //End implementation