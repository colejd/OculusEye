//
//  PS4EyePlugin.mm
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#import "PS4EyePlugin.h"
#import "UnityInterop.h"

//C++ ---------------------------------------------------------

// --------------------------------------------------------------------------
// Helper utilities


// Prints a string
/*
static void DebugLog (const char* str)
{
#if UNITY_WIN
    OutputDebugStringA (str);
#else
    printf ("%s", str);
    //LogUnity(str);
#endif
}
*/

// --------------------------------------------------------------------------

extern "C"{
void InitDriver(){
    [[PS4EyePlugin sharedInstance] InitDriver];
}

void Begin(){
    [[PS4EyePlugin sharedInstance] Begin];
}

void End(){
    [[PS4EyePlugin sharedInstance] End];
}

void Dealloc(){
    [[PS4EyePlugin sharedInstance] End];
}

int GetCameraCount(){
    return [[PS4EyePlugin sharedInstance] GetCameraCount];
}

uint8_t* GetLeftCameraData(){
    return [[PS4EyePlugin sharedInstance] GetLeftCameraData];
}
uint8_t* GetRightCameraData(){
    return [[PS4EyePlugin sharedInstance] GetRightCameraData];
}
void PullData(){
    [[PS4EyePlugin sharedInstance] PullData];
}

bool EyeHasNewFrame(){
    return [[PS4EyePlugin sharedInstance] EyeHasNewFrame];
}

bool EyeInitialized(){
    return [[PS4EyePlugin sharedInstance] EyeInitialized];
}

bool ThreadIsRunning(){
    return [[PS4EyePlugin sharedInstance] ThreadIsRunning];
}

void setAutoWhiteBalance (bool autoWhiteBalance){
    return [[PS4EyePlugin sharedInstance] setAutoWhiteBalance:autoWhiteBalance];
}
void setAutoGain (bool autoGain){
    return [[PS4EyePlugin sharedInstance] setAutoGain:autoGain];
}
void setGain (float gain){
    return [[PS4EyePlugin sharedInstance] setGain:gain];
}
void setSharpness (float sharpness){
    return [[PS4EyePlugin sharedInstance] setSharpness:sharpness];
}
void setExposure (float exposure){
    return [[PS4EyePlugin sharedInstance] setExposure:exposure];
}
void setBrightness (float brightness){
    return [[PS4EyePlugin sharedInstance] setBrightness:brightness];
}
void setContrast (float contrast){
    return [[PS4EyePlugin sharedInstance] setContrast:contrast];
}
void setHue (float hue){ //huehuehuehuehue
    return [[PS4EyePlugin sharedInstance] setHue:hue];
}
void setBlueBalance (float blueBalance){
    return [[PS4EyePlugin sharedInstance] setBlueBalance:blueBalance];
}
void setRedBalance (float redBalance){
    return [[PS4EyePlugin sharedInstance] setRedBalance:redBalance];
}
    
void SetDebugFunction( FuncPtr fp )
{
    LogUnity = fp;
}
    
void SetUnityTexturePointers(void *leftPtr, void *rightPtr){
    // A script calls this at initialization time; just remember the texture pointer here.
    // Will update texture pixels each frame from the plugin rendering event (texture update
    // needs to happen on the rendering thread).
    left_TexturePointer = leftPtr;
    right_TexturePointer = rightPtr;
    LogUnity("Set Texture Pointers");
}
    
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

static void DoRendering (int eventID)
{
    
    if(EyeHasNewFrame()){
    
#if SUPPORT_OPENGL
    // OpenGL case
    if (g_DeviceType == kGfxRendererOpenGL)
    {
        PullData();
        if(eventID == 1){
            // update native texture from code
            if (left_TexturePointer)
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
            if (right_TexturePointer)
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
        LogUnity("This plugin needs OpenGL!");
    }
#endif
    }
}

static void FillTextureFromCode(int width, int height, int stride, unsigned char* dst, EyeType side){
    
    //Fill
    unsigned char *eyeData = NULL;
    if(side == LEFT_EYE){
        eyeData = GetLeftCameraData();
    }
    else if(side == RIGHT_EYE){
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





//Objective-C --------------------------------------------------

@implementation PS4EyePlugin{
    //Private instance variables
}

-(id)init {
    if (self = [super init]) {
        if(self){
            driver = new PS4EyeDriver();
            if (!driver) self = nil;
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

static PS4EyePlugin* sharedInstance = nil;
+(PS4EyePlugin*)sharedInstance {
    if( !sharedInstance )
        sharedInstance = [[PS4EyePlugin alloc] init];
    
    return sharedInstance;
}

-(void) InitDriver {
    driver->Init();
}

-(void) Begin {
    printf("[PS4EyePlugin] Starting...\n");
    //driver = new PS4EyeDriver();
    
    //Last
    printf("[PS4EyePlugin] PS4EyePlugin started.\n");
}

-(void) End {
    printf("[PS4EyePlugin] PS4EyePlugin shutting down...\n");
    //delete driver;
    
    //Last
    printf("[PS4EyePlugin] PS4EyePlugin shut down.\n");
}

-(uint8_t *)GetLeftCameraData{
    return driver->rawPixelData_Left;
}
-(uint8_t *)GetRightCameraData{
    return driver->rawPixelData_Right;
}

-(void)PullData{
    driver->PullData();
}

-(int) GetCameraCount {
    return driver->GetNumCameras();
}

-(bool) EyeHasNewFrame {
    return driver->eyeRef->isNewFrame();
}

-(bool) EyeInitialized {
    //return (driver->leftEyeRef != NULL);
    return driver->eyeInitialized;
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