//
//  CameraCalibrator.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 2/10/15.
//
//

#include "CameraCalibrator.h"

CameraCalibrator::CameraCalibrator(Mat &src_tmp, UMat &src_gray, UMat &dest){
    numSquares = numCornersHor * numCornersVer;
    board_sz = cv::Size(numCornersHor, numCornersVer);
    
    for(int j=0;j<numSquares;j++){
        obj.push_back(cv::Point3f(j/numCornersHor, j%numCornersHor, 0.0f));
    }
    
    intrinsic = cv::Mat(3, 3, CV_32FC1);
    //Assumes the camera has an aspect ratio of 1
    //Elements (0,0) and (1,1) are the focal lengths along the X and Y axis.
    //See http://www.aishack.in/tutorials/calibrating-undistorting-with-opencv-in-c-oh-yeah/
    intrinsic.ptr<float>(0)[0] = 1;
    intrinsic.ptr<float>(1)[1] = 1;

    this->src_tmp = &src_tmp;
    this->src_gray = &src_gray;
    this->dest = &dest;
}

CameraCalibrator::~CameraCalibrator(){
    
}

void CameraCalibrator::BeginCalibration(){
    printf("\n===BEGIN CALIBRATION===\n");
    printf("Boards: %i\n", numBoards);
    printf("Corners (horizontal): %i\n", numCornersHor);
    printf("Corners (vertical): %i\n", numCornersVer);
    printf("Squares: %i\n", numSquares);
    
    calibrating = true;
    calibrated = false;
    lookingForBoard = true;
    
}

void CameraCalibrator::EndCalibration(bool stopEarly){
    calibrating = false;
    if(!stopEarly){
        reprojectionError = calibrateCamera(object_points, image_points, src_tmp->size(), intrinsic, distCoeffs, rvecs, tvecs);
        printf("Calibrated! Reprojection error = %f / 1 (better values are closer to 0)\n", reprojectionError);
    }
    else{
        printf("Calibration canceled.\n");
    }
    printf("===END CALIBRATION===\n");
}

/**
 * Search for a physical chess board and correct for distortion once found.
 */
void CameraCalibrator::Update(){
    if(successes < numBoards){
        //Look for a chess board in the camera frustum.
        bool found = false;
        
        //Use a time delay to wait between searches to reduce slowdown
        if(lookingForBoard){
            float currentTime = ofGetElapsedTimef();
            if( (currentTime - lastFoundBoardTime) > delayBetweenBoardSearches){
                lastFoundBoardTime = currentTime;
                found = findChessboardCorners(*src_tmp, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH+CV_CALIB_CB_NORMALIZE_IMAGE);
            }
        }
        
        //Once the board is found, it will examine the board every frame. However, it will only
        //record the distortion every few frames, so as to give the user time to move the camera,
        //giving better results.
        if(found) {
            lookingForBoard = false;
            cornerSubPix(*src_gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            drawChessboardCorners(*dest, board_sz, corners, found);
            
            //If delayBetweenSuccesses seconds have passed, capture the orientation of the checkerboard.
            float currentTime = ofGetElapsedTimef();
            if( (currentTime - lastFrameTime) > delayBetweenSuccesses){
                lastFrameTime = currentTime;
                
                image_points.push_back(corners);
                object_points.push_back(obj);
                ofLog(OF_LOG_VERBOSE, "Keyframe %i captured\n", successes);
                successes++;
            }
            
        }
        else{
            lookingForBoard = true;
        }
        
    }
    //If we have enough samples, finalize
    else{
        calibrated = true;
        EndCalibration();
    }
}