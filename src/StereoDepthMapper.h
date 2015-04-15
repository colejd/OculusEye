//
//  StereoDepthMapper.h
//  OculusEye
//
//  Created by Jonathan Cole on 2/10/15.
//
//

/**
 * Uses a lot of code from OpenCV's sample disparity mapping code.
 * Link: https://github.com/Itseez/opencv/blob/master/samples/cpp/tutorial_code/calib3d/stereoBM/SBM_Sample.cpp
 */

#ifndef __OculusEye__StereoDepthMapper__
#define __OculusEye__StereoDepthMapper__

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

class StereoDepthMapper {
public:
    cv::Mat stereoMapRaw;
    cv::Mat stereoMap;
    
    int lowThreshold;
    int highThreshold;
    
    cv::Ptr<StereoBM> sbm;
    
    cv::Ptr<StereoSGBM> sgbm;
    
    StereoDepthMapper();
    ~StereoDepthMapper();
    
    void UpdateValues();
    void CalculateStereoMap(const Mat &leftIn, const Mat &rightIn, bool swapSides = false);
    
    bool useSGBM = false;
    
    //numDisparities – Maximum disparity minus minimum disparity. This parameter must be divisible by 16.
    int numDisparities = 112;
    int preFilterSize = 5;
    //preFilterCap – Truncation value for the prefiltered image pixels.
    int preFilterCap = 61;
    //minDisparity – Minimum possible disparity value.
    int minDisparity = -39;
    int textureThreshold = 507;
    //uniquenessRatio – Margin in percentage by which the best (minimum) computed cost function value should “win” the second best value to consider the found match correct. Normally, a value within the 5-15 range is good enough.
    int uniquenessRatio = 0;
    //speckleWindowSize – Maximum size of smooth disparity regions to consider their noise speckles and invalidate.
    int speckleWindowSize = 0;
    //speckleRange – Maximum disparity variation within each connected component.
    int speckleRange = 8;
    //disp12MaxDiff – Maximum allowed difference (in integer pixel units) in the left-right disparity check.
    int disp12MaxDiff = 1;
    //SADWindowSize – Matched block size. It must be an odd number >=1 .
    int SADWindowSize = 5;
    
    int blurScale = 3;
    int erosionIterations = 1;
    int dilutionIterations = 1;
    
private:
    
};




#endif /* defined(__OculusEye__StereoDepthMapper__) */
