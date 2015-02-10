//
//  StereoDepthMapper.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 2/10/15.
//
//

#include "StereoDepthMapper.h"


StereoDepthMapper::StereoDepthMapper(){
    sbm = StereoBM::create(ndisparities, SADWindowSize);
}

StereoDepthMapper::~StereoDepthMapper(){
    
}

//https://github.com/Itseez/opencv/blob/master/samples/cpp/tutorial_code/calib3d/stereoBM/SBM_Sample.cpp
void StereoDepthMapper::CalculateStereoMap(const Mat &left, const Mat &right, bool swapSides){
    stereoMapRaw = Mat( left.rows, left.cols, CV_16S );
    stereoMap = Mat( left.rows, left.cols, CV_8UC1 );
    //printf("Left size:   %i, %i\n", left.rows, left.cols);
    //printf("Right size:  %i, %i\n", right.rows, right.cols);
    //printf("Stereo size: %i, %i\n", stereoMap.rows, stereoMap.cols);
    
    //Swap image sides to match actual camera alignment if desired
    if(swapSides)
        sbm->compute( right, left, stereoMapRaw );
    else
        sbm->compute( left, right, stereoMapRaw );
    
    double lowThresh = (double)lowThreshold;
    printf("lowThresh %f\n", lowThresh);
    double highThresh = (double)highThreshold;
    minMaxLoc( stereoMapRaw, &lowThresh, &highThresh );
    
    //printf("Min disp: %f Max value: %f \n", minVal, maxVal);
    
    //-- 4. Display it as a CV_8UC1 image
    stereoMapRaw.convertTo( stereoMap, CV_8UC1, 255/(highThresh - lowThresh));
    
}