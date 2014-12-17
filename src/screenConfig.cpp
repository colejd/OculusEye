//
//  screenConfig.cpp
//  ofCV
//
//  Created by Jonathan Cole on 9/22/14.
//
//

#include "screenConfig.h"

size_t displayBitsPerPixelForMode(CGDisplayModeRef mode);
CGDisplayModeRef bestMatchForMode(screenMode screenMode);
CGDisplayModeRef bestMatchForParameters(int width, int height, int depth);

size_t displayBitsPerPixelForMode(CGDisplayModeRef mode){
    size_t depth = 0;
	
	CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
	if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
		depth = 32;
    else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        depth = 16;
    else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        depth = 8;
    
    return depth;
}

CGDisplayModeRef bestMatchForMode(screenMode screenMode){
    bool exactMatch = false;
	
    // Get a copy of the current display mode
	CGDisplayModeRef displayMode = CGDisplayCopyDisplayMode(kCGDirectMainDisplay);
	
    // Loop through all display modes to determine the closest match.
    // CGDisplayBestModeForParameters is deprecated on 10.6 so we will emulate it's behavior
    // Try to find a mode with the requested depth and equal or greater dimensions first.
    // If no match is found, try to find a mode with greater depth and same or greater dimensions.
    // If still no match is found, just use the current mode.
    CFArrayRef allModes = CGDisplayCopyAllDisplayModes(kCGDirectMainDisplay, NULL);
    for(int i = 0; i < CFArrayGetCount(allModes); i++)	{
		CGDisplayModeRef mode = (CGDisplayModeRef)CFArrayGetValueAtIndex(allModes, i);
        
		if(displayBitsPerPixelForMode(mode) != screenMode.bitsPerPixel)
			continue;
		if((CGDisplayModeGetWidth(mode) >= screenMode.width) && (CGDisplayModeGetHeight(mode) >= screenMode.height))
		{
			displayMode = mode;
			exactMatch = true;
			break;
		}
	}
	
    // No depth match was found
    if(!exactMatch)
	{
		for(int i = 0; i < CFArrayGetCount(allModes); i++)
		{
			CGDisplayModeRef mode = (CGDisplayModeRef)CFArrayGetValueAtIndex(allModes, i);
			if(displayBitsPerPixelForMode(mode) >= screenMode.bitsPerPixel)
				continue;
			
			if((CGDisplayModeGetWidth(mode) >= screenMode.width) && (CGDisplayModeGetHeight(mode) >= screenMode.height))
			{
				displayMode = mode;
				break;
			}
		}
	}
	return displayMode;
    
}

CGDisplayModeRef bestMatchForParameters(int width, int height, int depth){
    screenMode mode;
    mode.width = width;
    mode.height = height;
    mode.bitsPerPixel = depth;
    
    return bestMatchForMode(mode);
    
}

void changeScreenRes(int h, int v){
    
	CGRect screenFrame = CGDisplayBounds(kCGDirectMainDisplay);
	CGSize screenSize  = screenFrame.size;
    int screenWidth = screenSize.width;
    int screenHeight = screenSize.height;
	printf("Current resolution is %ix%i\n", screenWidth, screenHeight);
    
	if(h != screenSize.width || v != screenSize.height){
        
		CGDirectDisplayID display = CGMainDisplayID(); // ID of main display
		CFDictionaryRef mode = CGDisplayBestModeForParameters(display, 32, h, v, NULL); // mode to switch to
        //CGDisplayModeRef mode = bestMatchForParameters(h, v, 32);
        
		CGDisplayConfigRef config;
        
		if (CGBeginDisplayConfiguration(&config) == kCGErrorSuccess) {
            CGConfigureDisplayMode(config, display, mode);
            //CGConfigureDisplayWithDisplayMode(config, display, mode, NULL);
			CGCompleteDisplayConfiguration(config, kCGConfigureForSession );
			printf("Changed screen resolution to %ix%i\n", h, v);
		}else{
			printf("Error changing resolution to %ix%i\n", h, v);
		}
        //CGDisplayModeRelease(mode);
	} else{
		printf("Screen resolution is already at the target resolution of %ix%i\n", h, v);
	}
    
}