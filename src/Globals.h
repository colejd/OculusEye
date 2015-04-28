//
//  Globals.h
//  OculusEye
//
//  Created by Jonathan Cole on 4/15/15.
//
//

#ifndef __OculusEye__Globals__
#define __OculusEye__Globals__

#include <stdio.h>

// Declare forward references here.
namespace Globals {
    enum CannyTypes{
        CANNY_GRAYSCALE = 0,
        CANNY_HUE = 1,
        CANNY_COLOR = 2
    };
    extern CannyTypes cannyType;
    
    extern bool autoCannyThresholding;
    
    //Minimum length of a contour before it is pruned.
    extern int minContourLength;
    
    extern bool useStereoGUI;
    
    extern int GUIConvergence;
    extern float GUIHeightScale;
    
    extern bool drawNose;
    extern float noseScale;
    extern int noseHeight;
    
    extern bool useRandomLineColors;
    
    
}

#endif /* defined(__OculusEye__Globals__) */
