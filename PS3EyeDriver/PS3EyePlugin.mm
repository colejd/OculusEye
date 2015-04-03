//
//  PS3EyePlugin.mm
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#import "PS3EyePlugin.h"

//C++ ---------------------------------------------------------
extern "C"{
void InitDriver(){
    [[PS3EyePlugin sharedInstance] InitDriver];
}

void Begin(){
    [[PS3EyePlugin sharedInstance] Begin];
}

void End(){
    [[PS3EyePlugin sharedInstance] End];
}

void Dealloc(){
    [[PS3EyePlugin sharedInstance] End];
}

int GetCameraCount(){
    return [[PS3EyePlugin sharedInstance] GetCameraCount];
}

uint8_t* GetLeftCameraData(){
    return [[PS3EyePlugin sharedInstance] GetLeftCameraData];
}
uint8_t* GetRightCameraData(){
    return [[PS3EyePlugin sharedInstance] GetRightCameraData];
}
void PullData_Left(){
    [[PS3EyePlugin sharedInstance] PullData_Left];
}
void PullData_Right(){
    [[PS3EyePlugin sharedInstance] PullData_Right];
}

bool LeftEyeHasNewFrame(){
    return [[PS3EyePlugin sharedInstance] LeftEyeHasNewFrame];
}
bool RightEyeHasNewFrame(){
    return [[PS3EyePlugin sharedInstance] RightEyeHasNewFrame];
}

bool LeftEyeInitialized(){
    return [[PS3EyePlugin sharedInstance] LeftEyeInitialized];
}
bool RightEyeInitialized(){
    return [[PS3EyePlugin sharedInstance] RightEyeInitialized];
}

bool ThreadIsRunning(){
    return [[PS3EyePlugin sharedInstance] ThreadIsRunning];
}

void setAutoWhiteBalance (bool autoWhiteBalance){
    return [[PS3EyePlugin sharedInstance] setAutoWhiteBalance:autoWhiteBalance];
}
void setAutoGain (bool autoGain){
    return [[PS3EyePlugin sharedInstance] setAutoGain:autoGain];
}
void setGain (float gain){
    return [[PS3EyePlugin sharedInstance] setGain:gain];
}
void setSharpness (float sharpness){
    return [[PS3EyePlugin sharedInstance] setSharpness:sharpness];
}
void setExposure (float exposure){
    return [[PS3EyePlugin sharedInstance] setExposure:exposure];
}
void setBrightness (float brightness){
    return [[PS3EyePlugin sharedInstance] setBrightness:brightness];
}
void setContrast (float contrast){
    return [[PS3EyePlugin sharedInstance] setContrast:contrast];
}
void setHue (float hue){ //huehuehuehuehue
    return [[PS3EyePlugin sharedInstance] setHue:hue];
}
void setBlueBalance (float blueBalance){
    return [[PS3EyePlugin sharedInstance] setBlueBalance:blueBalance];
}
void setRedBalance (float redBalance){
    return [[PS3EyePlugin sharedInstance] setRedBalance:redBalance];
}
}





//Objective-C --------------------------------------------------

@implementation PS3EyePlugin{
    //Private instance variables
}

-(id)init {
    if (self = [super init]) {
        if(self){
            driver = new PS3EyeDriver();
            if (!driver) self = nil;
        }
    }
    return self;
}

- (void)dealloc {
    delete driver;
    driver = NULL;
    [super dealloc];
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
    printf("[PS3EyePlugin] starting...\n");
    //driver = new PS3EyeDriver();
    
    //Last
    printf("[PS3EyePlugin] PS3EyePlugin started.\n");
}

-(void) End {
    printf("[PS3EyePlugin] PS3EyePlugin shutting down...\n");
    //delete driver;
    
    //Last
    printf("[PS3EyePlugin] PS3EyePlugin shut down.\n");
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
    //return (driver->leftEyeRef != NULL);
    return driver->leftEyeInitialized;
}

-(bool) RightEyeInitialized {
    //return (driver->rightEyeRef != NULL);
    return driver->rightEyeInitialized;
}

-(bool) ThreadIsRunning {
    return driver->cameraThreadStarted;
}

-(void) setAutoWhiteBalance: (bool) autoWhiteBalance{
    driver->setAutoWhiteBalance(autoWhiteBalance, BOTH_EYES);
}

-(void) setAutoGain: (bool) autoGain{
    driver->setAutoGain(autoGain, BOTH_EYES);
}

-(void) setGain: (float) gain{
    driver->setGain(gain, BOTH_EYES);
}

-(void) setSharpness: (float) sharpness{
    driver->setSharpness(sharpness, BOTH_EYES);
}

-(void) setExposure: (float) exposure{
    driver->setExposure(exposure, BOTH_EYES);
}

-(void) setBrightness: (float) brightness{
    driver->setBrightness(brightness, BOTH_EYES);
}

-(void) setContrast: (float) contrast{
    driver->setContrast(contrast, BOTH_EYES);
}

-(void) setHue: (float) hue{ //huehuehuehuehue
    driver->setHue(hue, BOTH_EYES);
}

-(void) setBlueBalance: (float) blueBalance{
    driver->setBlueBalance(blueBalance, BOTH_EYES);
}

-(void) setRedBalance: (float) redBalance{
    driver->setRedBalance(redBalance, BOTH_EYES);
}

@end //End implementation