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


// Do fullscreen on launch? (Screen resolution changes from DEFAULT_RES to TARGET_RES)
#define LAUNCH_FULLSCREEN false
// Native screen resolution
#define DEFAULT_RES_X 1280
#define DEFAULT_RES_Y 800
// Desired screen resolution
#define TARGET_RES_X 1280
#define TARGET_RES_Y 800

// Values to be requested from the Playstation Eye cameras.
// Illegal values for fps will fall back to the next lowest supported value.
// 640x480 supported framerates: 60, 50, 40, 30, 15
// 320x240 supported framerates: 205 (video is corrupted), 187, 150, 137, 125, 100, 75, 60, 50, 37, 30
#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480
#define CAMERA_FPS 60

// The highest framerate allowed (capped to 60 when vsync is enabled).
#define TARGET_FRAMERATE 9999 //9999
//Enable/disable OpenCL support through UMats (freezes at Canny at the moment)
#define USE_OPENCL false



#endif
