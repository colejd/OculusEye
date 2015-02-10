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
    
    int ndisparities = 16*5;   /**< Range of disparity */
    int SADWindowSize = 21; /**< Size of the block window. Must be odd */
    
    int lowThreshold;
    int highThreshold;
    
    cv::Ptr<StereoBM> sbm;
    
    StereoDepthMapper();
    ~StereoDepthMapper();
    
    StereoDepthMapper(int rows, int cols);
    void CalculateStereoMap(const Mat &left, const Mat &right, bool swapSides = false);
    
private:
    
};




#endif /* defined(__OculusEye__StereoDepthMapper__) */
