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

-(int) GetCameraCount {
    return driver->GetNumCameras();
}

@end //End implementation