//
//  StereoDepthMapper.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 2/10/15.
//
//

#include "StereoDepthMapper.h"


StereoDepthMapper::StereoDepthMapper(){
    sbm = StereoBM::create(16, 9);
    
    sbm->setNumDisparities(112);
    sbm->setPreFilterSize(5);
    sbm->setPreFilterCap(31);
    sbm->setMinDisparity(0);
    sbm->setTextureThreshold(10);
    sbm->setUniquenessRatio(15);
    sbm->setSpeckleWindowSize(100);
    sbm->setSpeckleRange(32);
    sbm->setDisp12MaxDiff(1);
    sbm->setBlockSize(SADWindowSize > 0 ? SADWindowSize : 3);
    
    sgbm = StereoSGBM::create(-64, 192, 5);
    /*
    sgbm -> SADWindowSize = 5;
    sgbm -> numberOfDisparities = 192;
    sgbm.preFilterCap = 4;
    sgbm.minDisparity = -64;
    sgbm.uniquenessRatio = 1;
    sgbm.speckleWindowSize = 150;
    sgbm.speckleRange = 2;
    sgbm.disp12MaxDiff = 10;
    sgbm.fullDP = false;
    sgbm.P1 = 600;
    sgbm.P2 = 2400;
     */
    
    sgbm->setNumDisparities(192);
    sgbm->setPreFilterCap(4);
    sgbm->setMinDisparity(-64);
    sgbm->setUniquenessRatio(1);
    sgbm->setSpeckleWindowSize(150);
    sgbm->setSpeckleRange(2);
    sgbm->setDisp12MaxDiff(10);
}

StereoDepthMapper::~StereoDepthMapper(){
    
}

/**
 * Update the sbm with GUI values.
 */
void StereoDepthMapper::UpdateValues(){
    sbm->setNumDisparities(numDisparities);
    sbm->setPreFilterSize(preFilterSize);
    sbm->setPreFilterCap(preFilterCap);
    sbm->setMinDisparity(minDisparity);
    sbm->setTextureThreshold(textureThreshold);
    sbm->setUniquenessRatio(uniquenessRatio);
    sbm->setSpeckleWindowSize(speckleWindowSize);
    sbm->setSpeckleRange(speckleRange);
    sbm->setDisp12MaxDiff(disp12MaxDiff);
    sbm->setBlockSize(SADWindowSize > 0 ? SADWindowSize : 3);
}

//https://github.com/Itseez/opencv/blob/master/samples/cpp/tutorial_code/calib3d/stereoBM/SBM_Sample.cpp
void StereoDepthMapper::CalculateStereoMap(const Mat &leftIn, const Mat &rightIn, bool swapSides){
    UpdateValues();
    
    Mat left = leftIn.clone();
    Mat right = rightIn.clone();
    
    stereoMapRaw = Mat( left.rows, left.cols, CV_16S );
    stereoMap = Mat( left.rows, left.cols, CV_8UC1 );
    
    //Box filter blur
    blur( left, left, cv::Size(blurScale, blurScale) );
    blur( right, right, cv::Size(blurScale, blurScale) );
    
    if(erosionIterations > 0){
        erode(left, left, Mat(), cv::Point(-1,-1), erosionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
        erode(right, right, Mat(), cv::Point(-1,-1), erosionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
    }
    if(dilutionIterations > 0){
        dilate(left, left, Mat(), cv::Point(-1,-1), dilutionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
        dilate(right, right, Mat(), cv::Point(-1,-1), dilutionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
    }
    
    /*
    printf("Left size:   %i, %i\n", left.rows, left.cols);
    printf("Right size:  %i, %i\n", right.rows, right.cols);
    printf("Stereo size: %i, %i\n", stereoMap.rows, stereoMap.cols);
     */
    
    //Swap image sides to match actual camera alignment if desired
    if(useSGBM){
        if(swapSides) sgbm->compute(right, left, stereoMapRaw);
        else sgbm->compute(left, right, stereoMapRaw);
    }
    else{
        if(swapSides) sbm->compute( right, left, stereoMapRaw );
        else sbm->compute( left, right, stereoMapRaw );
    }
    
    
    double lowThresh = (double)lowThreshold;
    double highThresh = (double)highThreshold;
    minMaxLoc( stereoMapRaw, &lowThresh, &highThresh );
    //printf("Min disp: %f Max value: %f \n", minVal, maxVal);
    stereoMapRaw.convertTo( stereoMap, CV_8UC1, 255/(highThresh - lowThresh));
    
    //normalize(stereoMapRaw, stereoMap, 0, 255, CV_MINMAX, CV_8U);
    
    left.release();
    right.release();
    
}