//
//  Globals.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 4/15/15.
//
//

#include "Globals.h"

//Initialize references here.
namespace Globals {
    CannyTypes cannyType;
    bool autoCannyThresholding;
    int minContourLength;
    bool useStereoGUI = false;
    int GUIConvergence;
    
    bool drawNose;
    float noseScale;
    int noseHeight;
    float GUIHeightScale;
    
    bool useRandomLineColors;
}