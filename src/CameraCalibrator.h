//
//  CameraCalibrator.h
//  OculusEye
//
//  Created by Jonathan Cole on 2/10/15.
//
//

#ifndef __OculusEye__CameraCalibrator__
#define __OculusEye__CameraCalibrator__

#include <stdio.h>

#include "ofMain.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class CameraCalibrator {
public:
    
    //CameraCalibrator();
    ~CameraCalibrator();
    CameraCalibrator(Mat &src_tmp, UMat &src_gray, UMat &dest);
    
    void BeginCalibration();
    void EndCalibration(bool stopEarly = false);
    bool calibrating = false;
    bool calibrated = false;
    
    int numBoards = 10; //The number of samples (frames with detected chessboards) taken to determine camera intrinsics
    int numCornersHor = 9; //Actual number of squares minus one
    int numCornersVer = 6; //Actual number of squares minus one
    int numSquares;
    cv::Size board_sz;
    
    vector< vector<cv::Point3f> > object_points;
    vector< vector<cv::Point2f> > image_points;
    vector<cv::Point2f> corners;
    int successes=0;
    vector<cv::Point3f> obj;
    float delayBetweenSuccesses = 1.0f;
    
    float lastFrameTime;
    
    cv::Mat intrinsic = cv::Mat(3, 3, CV_32FC1);
    cv::Mat distCoeffs;
    vector<cv::Mat> rvecs;
    vector<cv::Mat> tvecs;
    
    void Update();
    
    float reprojectionError;
    
    
private:
    cv::Mat *src_tmp;
    cv::UMat *src_gray;
    cv::UMat *dest;
    
};


#endif /* defined(__OculusEye__CameraCalibrator__) */
