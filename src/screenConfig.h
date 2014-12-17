//
//  screenConfig.h
//  ofCV
//
//  Created by Jonathan Cole on 9/22/14.
//
//

/**
 * This class is for adjusting the resolution of the monitor in OSX. This is
 * important for ensuring that the video displays properly on an Oculus Rift.
 * Based on code from http://forum.openframeworks.cc/t/change-screen-resolution-with-objective-c/5175
 */

#include <CoreVideo/CVDisplayLink.h>

#ifndef __ofCV__screenConfig__
#define __ofCV__screenConfig__

//#include <iostream>

struct screenMode {
    size_t width;
    size_t height;
    size_t bitsPerPixel;
};
size_t displayBitsPerPixelForMode(CGDisplayModeRef mode);
CGDisplayModeRef bestMatchForMode(screenMode screenMode);
CGDisplayModeRef bestMatchForParameters(int width, int height, int depth);
void changeScreenRes(int h, int v);

#endif /* defined(__ofCV__screenConfig__) */
