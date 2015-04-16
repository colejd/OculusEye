//
//  PS3EyePlugin.mm
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//
//  Uses code from http://docs.unity3d.com/Manual/NativePluginInterface.html

#import "PS3EyePlugin.h"
#import "UnityInterop.h"

// ==========================================================================
// C++ (Unity-oriented static bindings for native interface)
// ==========================================================================

void LogNative(const char *message){
    printf("%s\n", message);
}

// --------------------------------------------------------------------------
// Extern functions

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

unsigned char* GetLeftCameraData(){
    return [[PS3EyePlugin sharedInstance] GetLeftCameraData];
}
unsigned char* GetRightCameraData(){
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

/**
 * Called from Unity. Points DebugLog to the print function of Unity.
 */
void SetDebugLogFunction( FuncPtr fp )
{
    DebugLog = fp;
}
    
// --------------------------------------------------------------------------
// Unity functions

void SetUnityTexturePointers(void *leftPtr, void *rightPtr){
    // A script calls this at initialization time; just remember the texture pointer here.
    // Will update texture pixels each frame from the plugin rendering event (texture update
    // needs to happen on the rendering thread).
    left_TexturePointer = leftPtr;
    right_TexturePointer = rightPtr;
    DebugLog("[PS3EyePlugin] Set texture pointers.");
}

/* Altered by Jon */
void UnitySetGraphicsDevice (void* device, int deviceType, int eventType)
{
    // Set device type to -1, i.e. "not recognized by our plugin"
    g_DeviceType = -1;
    
#if SUPPORT_OPENGL
    // If we've got an OpenGL device, remember device type. There's no OpenGL
    // "device pointer" to remember since OpenGL always operates on a currently set
    // global context.
    if (deviceType == kGfxRendererOpenGL)
    {
        g_DeviceType = deviceType;
    }
#endif
}

/* Altered by Jon */
void UnityRenderEvent (int eventID)
{
    // Unknown graphics device type? Do nothing.
    if (g_DeviceType == -1)
        return;
    
    // Actual functions defined below
    SetDefaultGraphicsState ();
    DoRendering (eventID);
}
    
}

/* Altered by Jon */
static void SetDefaultGraphicsState ()
{
    
#if SUPPORT_OPENGL
    // OpenGL case
    if (g_DeviceType == kGfxRendererOpenGL)
    {
        glDisable (GL_CULL_FACE);
        glDisable (GL_LIGHTING);
        glDisable (GL_BLEND);
        glDisable (GL_ALPHA_TEST);
        glDepthFunc (GL_LEQUAL);
        glEnable (GL_DEPTH_TEST);
        glDepthMask (GL_FALSE);
    }
#endif
}

/* Altered by Jon */
/**
 * Copies pixel data from the camera to Unity Texture2D object.
 */
static void DoRendering (int eventID)
{
    
#if SUPPORT_OPENGL
    // OpenGL case
    if (g_DeviceType == kGfxRendererOpenGL)
    {
        if(eventID == 1){
            // update native texture from code
            if (left_TexturePointer && LeftEyeHasNewFrame())
            {
                GLuint gltex = (GLuint)(size_t)(left_TexturePointer);
                glBindTexture (GL_TEXTURE_2D, gltex);
                int texWidth, texHeight;
                glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
                glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
                
                unsigned char* data = new unsigned char[texWidth*texHeight*4];
                FillTextureFromCode (texWidth, texHeight, texHeight*4, data, LEFT_EYE);
                glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
                delete[] data;
            }
        }
        
        if(eventID == 2){
            // update native texture from code
            if (right_TexturePointer && RightEyeHasNewFrame())
            {
                GLuint gltex = (GLuint)(size_t)(right_TexturePointer);
                glBindTexture (GL_TEXTURE_2D, gltex);
                int texWidth, texHeight;
                glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
                glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
                
                unsigned char* data = new unsigned char[texWidth*texHeight*4];
                FillTextureFromCode (texWidth, texHeight, texHeight*4, data, RIGHT_EYE);
                glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
                delete[] data;
            }
        }
    }
    else{
        DebugLog("[PS3EyePlugin] This plugin needs OpenGL!");
    }
#endif
}

/* Altered by Jon */
/**
 * Writes the data from an RGB888 (byte) array to the specified Unity3D Texture2D objects.
 */
static void FillTextureFromCode(int width, int height, int stride, unsigned char* dst, EyeType side){
    
    //Fill
    unsigned char *eyeData = NULL;
    if(side == LEFT_EYE){
        PullData_Left();
        eyeData = GetLeftCameraData();
    }
    else if(side == RIGHT_EYE){
        PullData_Right();
        eyeData = GetRightCameraData();
    }
    
    
    for (int y = 0; y < height; ++y)
    {
        unsigned char* ptr = dst;
        unsigned char* eyeDataPtr = eyeData;
        //memcpy(dataPtr, data, width*height*3*sizeof(data[0]));
        for (int x = 0; x < width; ++x)
        {
            // Write the texture pixel
            
            ptr[0] = eyeDataPtr[0];//255;//data[0];
            ptr[1] = eyeDataPtr[1];//0;//data[1];
            ptr[2] = eyeDataPtr[2];//0;//data[2];
            ptr[3] = 255;
            
            
            // Jump to next pixel (each is 4 bytes)
            ptr += 4;
            eyeDataPtr += 3;
        }
        
        // Jump to next row
        dst += (width * 4);
        eyeData += (width * 3);
    }
    
}




// ==========================================================================
// Objective-C (native interface for plugin)
// ==========================================================================

@implementation PS3EyePlugin{
    //Private instance variables
}

-(id)init {
    if (self = [super init]) {
        if(self){
            //Set logging function (Unity should set this before this point is reached)
            if(DebugLog == NULL){
                DebugLog = &LogNative;
            }
            DebugLog("[PS3EyePlugin] super init");
            //Create the driver
            driver = new PS3EyeDriver(DebugLog);
            //if (!driver) self = nil;
        }
    }
    return self;
}

- (void)dealloc {
    delete driver;
    driver = NULL;
    if(sharedInstance != nil){
        [sharedInstance End];
        [sharedInstance dealloc];
    }
    [super dealloc];
}

static PS3EyePlugin* sharedInstance = nil;
+(PS3EyePlugin*)sharedInstance {
    if( !sharedInstance ){
        DebugLog("[PS3EyePlugin] Shared instance created.");
        sharedInstance = [[PS3EyePlugin alloc] init];
    }
    
    return sharedInstance;
}

-(void) InitDriver {
    DebugLog("[PS3EyePlugin] InitDriver started.");
    driver->Init();
    DebugLog("[PS3EyePlugin] InitDriver done.");
}

-(void) Begin {
    DebugLog("[PS3EyePlugin] Begin start");
    //driver = new PS3EyeDriver();
    //InitDriver();
    
    //Last
    DebugLog("[PS3EyePlugin] Begin end");
}

-(void) End {
    DebugLog("[PS3EyePlugin] Shutting down...");
    //delete driver;
    
    //Last
    DebugLog("[PS3EyePlugin] Shut down.");
}

-(unsigned char *)GetLeftCameraData{
    return driver->rawPixelData_Left;
}
-(unsigned char *)GetRightCameraData{
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

/*------ Setters -----*/

-(void) setAutoWhiteBalance: (bool) autoWhiteBalance{
    driver->setAutoWhiteBalance(autoWhiteBalance, BOTH_EYES);
}

-(void) setAutoGain: (bool) autoGain{
    driver->setAutoGain(autoGain, BOTH_EYES);
}

-(void) setGain: (Byte) gain{
    driver->setGain(gain, BOTH_EYES);
}

-(void) setSharpness: (Byte) sharpness{
    driver->setSharpness(sharpness, BOTH_EYES);
}

-(void) setExposure: (Byte) exposure{
    driver->setExposure(exposure, BOTH_EYES);
}

-(void) setBrightness: (Byte) brightness{
    driver->setBrightness(brightness, BOTH_EYES);
}

-(void) setContrast: (Byte) contrast{
    driver->setContrast(contrast, BOTH_EYES);
}

-(void) setHue: (Byte) hue{ //huehuehuehuehue
    driver->setHue(hue, BOTH_EYES);
}

-(void) setBlueBalance: (Byte) blueBalance{
    driver->setBlueBalance(blueBalance, BOTH_EYES);
}

-(void) setRedBalance: (Byte) redBalance{
    driver->setRedBalance(redBalance, BOTH_EYES);
}

/*------ Getters -----*/

-(bool) getAutoWhiteBalance {
    return driver->leftEyeRef->getAutoWhiteBalance();
}

-(bool) getAutoGain{
    return driver->leftEyeRef->getAutogain();
}

-(Byte) getGain{
    return driver->leftEyeRef->getGain();
}

-(Byte) getSharpness{
    return driver->leftEyeRef->getSharpness();
}

-(Byte) getExposure{
    return driver->leftEyeRef->getExposure();
}

-(Byte) getBrightness{
    return driver->leftEyeRef->getBrightness();
}

-(Byte) getContrast{
    return driver->leftEyeRef->getContrast();
}

-(Byte) getHue{ //hhhehehe
    return driver->leftEyeRef->getHue();
}

-(Byte) getBlueBalance{
    return driver->leftEyeRef->getBlueBalance();
}

-(Byte) getRedBalance{
    return driver->leftEyeRef->getRedBalance();
}

@end //End implementation