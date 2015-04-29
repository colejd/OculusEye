//
//  CVEye.h
//  OculusEye
//
//  Created by Jonathan Cole on 9/25/14.
//
//

#ifndef __ofCV__CVEye__
#define __ofCV__CVEye__

#include "ofMain.h"
#include <iostream>

//#include "ps3Eye/ps3eye.h"
//#include "OFCVBridge.h"
#include "programSettings.h"
#include "CameraCalibrator.h"

#include <PS3EyePlugin.h>
//#include "../PS3EyeDriver/PS3EyePlugin.h"
//#include "../PS3EyeDriver/PS3EyeDriver.h"

#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/flann/timer.h>
#include <opencv2/core/mat.hpp>

class CVEye {
public:
	
    CVEye(const int _index, PS3EyePlugin *driver, bool isLeft);
    ~CVEye();
	
    bool init(const int _width, const int _height);
    void draw( ofVec2f pos, ofVec2f size );
    void update();
    void shutdown();
    
    //PS3 Eye stuff
    //ps3eye::PS3EYECam::PS3EYERef eyeRef;
    PS3EyePlugin* eyePlugin;
    
    //FPS variables
    int camFrameCount = 0;
    int camFpsLastSampleFrame = 0;
    float camFpsLastSampleTime = 0;
    float camFps = 0;
    
    //Raw video data array from PS Eye (RGBA888 color)
    unsigned char * rawPixelData;
    
    //OF stuff
    ofImage videoImage, finalImage;
    
    //OpenCV--------------------
    float cannyRatio = 3.0f;
    float cannyMinThreshold = 50.0f;
    bool doCanny = false;
    void ApplyCanny(cv::UMat &src, cv::UMat &src_gray, cv::UMat &dest);
    void ApplyCannyNoSampling(const cv::UMat &src, const cv::UMat &src_gray, cv::UMat &dest);
    bool showEdgesOnly;
    float pickerColor[4];
    cv::Scalar guiLineColor = cv::Scalar(255, 0, 0);
    void UpdateLineColor();
    int edgeThickness = 2;
    bool GPUEnabled = false;
    
    //Is the camera initialized?
    bool initialized = false;
    
    bool render = true;
    
    //Pull camera data for processing using OpenCV.
    void PullData();
    
    //Perform erosion/dilution?
    bool doErosionDilution;
    int erosionIterations = 1;
    int dilutionIterations = 1;
    //Downsampling ratio of the original image. 0.5 means resolution is cut in half.
    float downsampleRatio = 0.5f;
    //Adaptive downsampling changes the downsample ratio depending on framerate.
    bool adaptiveDownsampling = false;
    //Current state of adaptive downsampling.
    float adaptedDownsampleRatio = 0.5f;
    
    //Number of pieces to break an image up for multithreaded contour detection.
    int imageSubdivisions = 1;
    bool drawContours = true;
    
    //True if a sample image is loaded in lieu of camera data
    bool dummyImage = false;
    
    cv::Mat src_tmp;
    cv::UMat src, src_gray, dest;
    cv::UMat canny_output, contour_output;
    
    string screenMessage = "";
    
    //Used for synchronization between multiple CVEye instances
    bool sync_update = false;
    
    CameraCalibrator *calibrator;
    
    bool isLeftEye;
    
    bool outputFrameSteps = false;
    
private:
    int camIndex;
    
};

/**
 * Takes an image and processes it in segments, which are distributed across the CPU cores by TBB.
 */
class ParallelContourDetector : public cv::ParallelLoopBody {
private:
    cv::UMat src_gray;
    cv::UMat &out_gray;
    int subsections;
    CVEye &eye;
    
public:
    
    ParallelContourDetector(cv::UMat _src_gray, cv::UMat &_out_gray, int _subsections, CVEye &_eye) : src_gray(_src_gray), out_gray(_out_gray), subsections(_subsections), eye(_eye) {};
    ~ParallelContourDetector();
    
    virtual void operator ()(const cv::Range &range) const;
    
    
};



#endif /* defined(__ofCV__CVEye__) */
