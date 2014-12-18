//
//  programSettings.h
//  OculusEye
//
//  Created by Jonathan Cole on 12/17/14.
//
//

/**
 * Globals for program setup.
 */

#ifndef OculusEye_programSettings_h
#define OculusEye_programSettings_h



// Do stretched fullscreen? (Screen resolution changes from DEFAULT_RES to TARGET_RES)
#define SYSTEM_FULLSCREEN false
// Native screen resolution
#define DEFAULT_RES_X 1440
#define DEFAULT_RES_Y 900
// Target screen resolution
#define TARGET_RES_X 1280
#define TARGET_RES_Y 800

// Width / Height of the Playstation Eye cameras.
// Can be 320x240@120fps or 640x480@60fps.
#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

// The framerate that will be used (capped to 60 when vsync is enabled).
#define TARGET_FRAMERATE 9999 //9999
//Enable/disable vsync
#define ENABLE_VSYNC true
//Enable/disable OpenCL support through UMats (freezes at Canny at the moment)
#define USE_OPENCL false



#endif
