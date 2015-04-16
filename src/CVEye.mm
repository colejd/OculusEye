//
//  CVEye.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 9/25/14.
//
//

#include "CVEye.h"
#include "Globals.h"

//Constructor
CVEye::CVEye(const int _index, PS3EyePlugin *plugin, bool isLeft) {
    camIndex = _index;
    eyePlugin = plugin;
    
    finalImage.allocate(CAMERA_WIDTH, CAMERA_HEIGHT, OF_IMAGE_COLOR);
    rawPixelData = (unsigned char*)malloc(CAMERA_WIDTH*CAMERA_HEIGHT*3*sizeof(unsigned char));
    isLeftEye = isLeft;
    
    //Initialize the camera, print results
    cout << (init(CAMERA_WIDTH, CAMERA_HEIGHT) ? "[CVEye] Camera initialized.\n" : "[CVEye] Camera not found.\n");
    
}

//Destructor
CVEye::~CVEye(){
    src_tmp.release();
    src.release();
    src_gray.release();
    canny_output.release();
    contour_output.release();
    
    //Clean up
    //free(rawPixelData); //Having this on crashes the program when cameras aren't plugged in. Why?
    
}

bool CVEye::init(const int _width, const int _height){
    if(isLeftEye) initialized = [eyePlugin LeftEyeInitialized];
    else initialized = [eyePlugin RightEyeInitialized];
    
    //If no cameras are detected, a sample image is loaded.
    if(initialized == false){
        ofLog(OF_LOG_WARNING, "Displaying sample image");
        
        Mat image;
        image = cv::imread("../../../data/images/SampleImage.png", CV_LOAD_IMAGE_COLOR);
        //Imread gives us BGR, convert it to RGB (it might be the other way around)
        cv::cvtColor(image, image, COLOR_BGR2RGB);
        rawPixelData = image.data;
        image.release();
        dummyImage = true;
    }
    
    //Grab some camera data (checks for initialization)
    PullData();
    
    int matType = CV_MAKE_TYPE(CV_8U, 3);
    src_tmp = cv::Mat(cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), matType, rawPixelData);
    src.create(cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), matType);
    src_gray.create(cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), CV_8U);
    src_tmp.copyTo(src);
    cvtColor(src, src_gray, COLOR_RGB2GRAY);
    src.copyTo(dest);
    
    canny_output.create(cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), CV_8U);
    contour_output.create(cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), CV_8UC3);
    
    calibrator = new CameraCalibrator(src_tmp, src_gray, dest);
    
    return initialized;
}

/**
 * Puts RGB888 data into rawPixelData to prepare it for OpenCV processing.
 */
void CVEye::PullData(){
    if(initialized){
        if(isLeftEye){
            //if(leftEyeRef->isNewFrame()){
                [eyePlugin PullData_Left];
                rawPixelData = [eyePlugin GetLeftCameraData];
                src_tmp.data = rawPixelData;
            //}
        }
        else{
            //if(rightEyeRef->isNewFrame()){
                [eyePlugin PullData_Right];
                rawPixelData = [eyePlugin GetRightCameraData];
                src_tmp.data = rawPixelData;
            //}
        }
    }
    /*
    if(initialized)
    {
        //Copy image data into VideoImage when there's a new image from the camera hardware.
        //bool isNewFrame = eyeRef->isNewFrame();
        if(render)
        {
            int whichMethod = 1;
            switch (whichMethod) {
                    
                //BRANCH 1: Straight manual conversion (Preferred)
                case 1:
                    yuvData.Convert(eyeRef->getLastFramePointerVolatile(), eyeRef->getRowBytes(), rawPixelData, eyeRef->getWidth(), eyeRef->getHeight());
                    src_tmp.data = rawPixelData;
                    break;
                
                
                //Branch 2: Parallel manual conversion (????)
                case 2:
                    yuvData.LoadData(eyeRef->getLastFramePointerVolatile(), eyeRef->getRowBytes(), rawPixelData, eyeRef->getWidth(), eyeRef->getHeight());
                    yuvData.ConvertParallel(1); //Number of rows to process at once?
                    src_tmp.data = rawPixelData;
                    break;
                
                //Branch 3: OpenCV native conversion (in case the Assembly types fix it before I do)
                case 3:
                    int matType = CV_MAKE_TYPE(CV_8U, 2);
                    cv::Mat yuv_tmp = cv::Mat(cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), matType);
                    yuv_tmp.data = eyeRef->getLastFramePointerVolatile();
                    cvtColor(yuv_tmp, src_tmp, COLOR_YUV2RGB_YUYV);
                    yuv_tmp.release();
                    break;
            }
            
            
        }
    }
     */
}

void CVEye::update(){
    //If this is the initialized left eye or right eye, continue
    bool cameraPass = ( ([eyePlugin LeftEyeInitialized] && isLeftEye) || ([eyePlugin RightEyeInitialized] && !isLeftEye) );
    
    //If we have a sample image or a camera frame, update
    if(cameraPass || dummyImage)
    //if(eyeDriver->leftEyeRef || dummyImage)
    {
        bool isNewFrame = false;
        if(isLeftEye && cameraPass) isNewFrame = [eyePlugin LeftEyeHasNewFrame];
        else if(!isLeftEye && cameraPass) isNewFrame = [eyePlugin RightEyeHasNewFrame];
        
        //Do we have camera data or a default image to display?
        bool computeFrame = dummyImage;
        if(cameraPass){
            //Continue if the camera hardware says it has a new frame
            //computeFrame = eyeDriver->leftEyeRef->isNewFrame();
            computeFrame = isNewFrame;
            //Mark this CVEye as updated
            //sync_update = eyeDriver->leftEyeRef->isNewFrame();
            sync_update = isNewFrame;
        }
        
        //Continue if we have new camera data or a default image to display
        if(computeFrame)
        {
            
            PullData();
            //rawPixelData = eyeDriver->rawPixelData_Left;
            //rawPixelData = [eyePlugin GetLeftCameraData];
            
            //-----------------------------------------------
            //Apply operations to src_tmp, which holds the unaltered camera data.
            //Preparation step for all other image operations.
            
            //Remove lens distortion from the camera data using the calibration
            //matrix if it exists
            if(calibrator->calibrated){
                printf("Applying calibration");
                cv::Mat src_tmp_undistorted;
                src_tmp.copyTo(src_tmp_undistorted);
                undistort(src_tmp_undistorted, src_tmp, calibrator->intrinsic, calibrator->distCoeffs);
            }
            
            //Move src_tmp (cv::Mat) into src (cv::UMat). A better way of doing this doesn't exist yet (February 2015)
            src_tmp.copyTo(src);
            cvtColor(src, src_gray, cv::COLOR_RGB2GRAY);
            src.copyTo(dest);
            
            //-----------------------------------------------
            //All image operations after this will use src as the input and dest as the output.
            
            if(calibrator->calibrating){
                
                calibrator->Update();
                
                //Do last
                finalImage.setFromPixels(dest.getMat(NULL).data, CAMERA_WIDTH, CAMERA_HEIGHT, OF_IMAGE_COLOR);
            }
            else if(doCanny){
                
                if(computeFrame){
                    ApplyCanny(src, src_gray, dest);
                }
                else{
                    if(showEdgesOnly) dest = cv::Scalar::all(0);
                    contour_output.copyTo(dest, contour_output);
                }
                //computeFrame = !computeFrame;
                
                //Do last
                finalImage.setFromPixels(dest.getMat(NULL).data, CAMERA_WIDTH, CAMERA_HEIGHT, OF_IMAGE_COLOR);
                
            }
            //If no effects are applied, just put the raw video in the output image.
            else{
                finalImage.setFromPixels(src.getMat(0).data, CAMERA_WIDTH, CAMERA_HEIGHT, OF_IMAGE_COLOR);
            }
            
        }
        
        //FPS counting stuff
        camFrameCount += computeFrame ? 1: 0;
        float timeNow = ofGetElapsedTimeMillis();
        if( timeNow > camFpsLastSampleTime + 1000 ){
            uint32_t framesPassed = camFrameCount - camFpsLastSampleFrame;
            camFps = (float)(framesPassed / ((timeNow - camFpsLastSampleTime)*0.001f));
            camFpsLastSampleTime = timeNow;
            camFpsLastSampleFrame = camFrameCount;
        }
        
    }
    
    //If there is no eye reference, keep trying to make one.
    //else init(width, height);
}

void CVEye::draw(ofVec2f pos, ofVec2f size){
    
}

void CVEye::shutdown(){
    
}

/* Performs Canny edge detection on src and applies the result to dest.
 */
void CVEye::ApplyCanny(cv::UMat &src, cv::UMat &src_gray, cv::UMat &dest){
    using namespace cv;
    //START_TIMER(cannyTimer);
    UMat input_image = src_gray;
    
    if(Globals::cannyType == Globals::CANNY_GRAYSCALE){
        //input_image = src_gray;
    }
    else if(Globals::cannyType == Globals::CANNY_HUE){
        std::vector<cv::UMat> channels;
        cv::UMat hsv;
        cv::cvtColor( src, hsv, CV_RGB2HSV );
        cv::split(hsv, channels);
        src_gray = channels[0];
        input_image = src_gray;
        hsv.release();
    }
    else if(Globals::cannyType == Globals::CANNY_COLOR){
        input_image = src;
    }
    
    adaptedDownsampleRatio = downsampleRatio;
    
    //Choose a downsampling ratio based on the current camera framerate.
    if(adaptiveDownsampling){
        float minFPS = 30.0f;
        float maxFPS = 60.0f;
        float percent = 1.0f;
        if(camFps > maxFPS) percent = 1.0f;
        else if(camFps < minFPS) percent = 0.0f;
        else{
            percent = ((maxFPS-minFPS) - (camFps-minFPS)) / (maxFPS-minFPS);
        }
        
        //Lerp
        //a + f * (b - a)
        adaptedDownsampleRatio = 1.0f + percent * (downsampleRatio - 1.0f);
    }
    
    //Downsample the image to a smaller size
    if(downsampleRatio < 1.0f){
        resize(input_image, input_image, cv::Size(), adaptedDownsampleRatio, adaptedDownsampleRatio, INTER_AREA); //#1
    }
    resize(canny_output, canny_output, input_image.size());
    resize(contour_output, contour_output, input_image.size());
    
    //Smooth src_gray to reduce noise (Canny also smooths the image, but it is a linear blur; thus this blurring is preserved)
    int blurScale = 3;
    //Box filter blur
    blur( input_image, input_image, cv::Size(blurScale, blurScale) );
    
    //Gaussian blur
    //cv::GaussianBlur(src_gray, src_gray, cv::Size(7,7), 1.5, 1.5);
    
    //Bilateral blur
    //float kernelLength = 3;
    //UMat src_gray_clone = src_gray.clone();
    //bilateralFilter(src_gray_clone, src_gray, kernelLength, kernelLength*2, kernelLength/2);
    //src_gray_clone.release();
    
    //Perform erosion and dilution if requested
    if(doErosionDilution){
        erode(input_image, input_image, Mat(), cv::Point(-1,-1), erosionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
        dilate(input_image, input_image, Mat(), cv::Point(-1,-1), dilutionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
    }
    
    int edgeThresh = 1;
    int lowThreshold = cannyMinThreshold; //Max of 100 by default
    int highThreshold = lowThreshold * cannyRatio;
    int kernel_size = 3;
    
    //Adjust the thresholds automatically if desired
    if(Globals::autoCannyThresholding){
        int thresholdType = THRESH_OTSU | THRESH_BINARY; //THRESH_OTSU | THRESH_BINARY;
        cv::Mat temp;
        if(Globals::cannyType == Globals::CANNY_COLOR){
            cv::Mat input_gray;
            cvtColor(input_image, input_gray, COLOR_RGB2GRAY);
            highThreshold = threshold(input_gray, temp, 0, 255, thresholdType);
            input_gray.release();
        }
        else{
            highThreshold = threshold(input_image, temp, 0, 255, thresholdType);
        }
        lowThreshold = highThreshold * 0.1;
        temp.release();
    }
    
    //cv::ocl::setUseOpenCL(true);
    //contour_output = Scalar::all(0);
    //canny_output = Scalar::all(0);
    if(Globals::cannyType == Globals::CANNY_COLOR){
        cv::UMat temp_gray;
        cvtColor(input_image, temp_gray, COLOR_RGB2GRAY);
        
        std::vector<cv::UMat> channels;
        cv::split(input_image, channels);
        
        //Separate the three color channels and perform Canny on each
        Canny(channels[0], channels[0], lowThreshold, highThreshold, kernel_size);
        Canny(channels[1], channels[1], lowThreshold, highThreshold, kernel_size);
        Canny(channels[2], channels[2], lowThreshold, highThreshold, kernel_size);
        //Also perform Canny on the grayscale version of the image
        Canny(temp_gray, temp_gray, lowThreshold, highThreshold, kernel_size);
        
        //Merge the three color channels into one image
        //merge(channels, canny_output);
        bitwise_and(channels[0], channels[1], channels[1]);
        bitwise_and(channels[1], channels[2], canny_output);
        //cvtColor(canny_output, canny_output, COLOR_RGB2GRAY);
        //Combine the merged image with the grayscale results for the final image
        bitwise_or(canny_output, temp_gray, canny_output);
        
        temp_gray.release();
    }
    else{
        Canny( input_image, canny_output, lowThreshold, highThreshold, kernel_size );
    }
    //cv::ocl::setUseOpenCL(false);
    
    //Black out the final image if we only want to see the edges when they are drawn
    if(showEdgesOnly) dest = Scalar::all(0);
    
    //Detect contours in canny_output and draw it to the final image
    if(drawContours){
        cv::parallel_for_(cv::Range(0, imageSubdivisions), ParallelContourDetector(canny_output, contour_output, imageSubdivisions, *this));
        
        //The C method for finding/drawing contours.
        /*
        CvMemStorage *mem;
        mem = cvCreateMemStorage(0);
        CvSeq *contours = 0;
        //Find contours in canny_output
        IplImage convertedCanny = canny_output.getMat(0).clone();
        cvFindContours(&convertedCanny, mem, &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
        //Draw contours in contour_output
        IplImage convertedContours = *cvCreateImage(cvSize(contour_output.size().width, contour_output.size().height), 8, 3);
        cvZero(&convertedContours);
        cvDrawContours(&convertedContours, contours, CV_RGB(1, 0, 0), CV_RGB(0, 0, 0), -1, CV_FILLED, 8, cvPoint(0,0));
        //printf("%s\n", type2str(finalContours.type()).c_str());
        //printf("%i\n", finalContours.rows * finalContours.cols );
        
        //Convert image back to UMat
        
        Mat finalContours;
        finalContours.create(contour_output.rows, contour_output.cols, CV_8UC3);
        finalContours = cvarrToMat(&convertedContours);
        finalContours.clone().copyTo(contour_output);
        finalContours.release();
         */
        
        
        //Upsample contour_output image to the size of dest
        if(downsampleRatio < 1.0f)
            resize(contour_output, contour_output, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), 0, 0, INTER_NEAREST);
        
        //Overlay the detected contours onto the final image using its
        //own data as a mask (this copies only the contours to the image)
        contour_output.copyTo(dest, contour_output);
    }
    //Just draw the edges from canny_output to the final image
    else{
        resize(canny_output, canny_output, cv::Size(CAMERA_WIDTH, CAMERA_HEIGHT), 0, 0, INTER_NEAREST);
        dest.setTo(guiLineColor, canny_output);
    }
    
    //STOP_TIMER(cannyTimer);
    //PRINT_TIMER(cannyTimer, "Canny time");
}




/*
void ParallelCanny::operator ()(const cv::Range &range) const{
    using namespace cv;
    
    //cv::UMat canny_output, contour_output;
    
    //cv::UMat src_gray, dest;
    
    //canny_output.create(cv::Size(eye.width, eye.height), CV_8U);
    //contour_output.create(cv::Size(eye.width, eye.height), CV_8UC3);
    
    //float adaptedDownsampleRatio = 0.5f;
    //resize(src_gray, src_gray, cv::Size(), adaptedDownsampleRatio, adaptedDownsampleRatio, INTER_AREA); //#1
    
    //int blurScale = 3;
    blur( src_gray, src_gray, cv::Size(3, 3) ); //#1
    
    printf("PreCanny\n");
    
    int edgeThresh = 1;
    //int lowThreshold = eye.cannyMinThreshold; //Max of 100 by default
    //int highThreshold = lowThreshold * eye.cannyRatio;
    int kernel_size = 3; //Aperture size; should be an odd number.
    
    for(int i = range.start; i < range.end; i++){
        //printf("%i\n", );
        cv::UMat in(src_gray, cv::Rect(0, (src_gray.rows/subsections)*i, src_gray.cols, src_gray.rows/subsections));
        
        cv::UMat out(out_gray, cv::Rect(0, (out_gray.rows/subsections)*i, out_gray.cols, out_gray.rows/subsections));
        try{
            Canny(in, out, 50, 150, kernel_size);
        }
        catch(Exception e){
 
        }
        
        in.release();
        out.release();
    }
    
    printf("PostCanny\n");
    
    //resize(src_gray, src_gray, cv::Size(640, 480), 0, 0, INTER_NEAREST);
}
 */


/**
 * Breaks the image into a number of pieces, specified by range's upper limit;
 * it then schedules each piece for canny detection on a different thread.
 */
 
void ParallelContourDetector::operator ()(const cv::Range &range) const{
    using namespace cv;
    
    for(int i = range.start; i < range.end; i++){
        vector< vector<cv::Point> > contourData;
        vector<Vec4i> contourHierarchy;
        cv::UMat in(src_gray, cv::Rect(0, (src_gray.rows/subsections)*i, src_gray.cols, src_gray.rows/subsections));
        cv::UMat out(out_gray, cv::Rect(0, (out_gray.rows/subsections)*i, out_gray.cols, out_gray.rows/subsections) );
        try{
            findContours( in, contourData, contourHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
            out = Scalar::all(0);
            Scalar color = eye.guiLineColor;
            //if(eye.rave) color = Scalar(rand()&255, rand()&255, rand()&255);
            
            //Prune contours smaller than a minimum length if desired
            if(Globals::minContourLength > 0){
                for (vector<vector<cv::Point> >::iterator it = contourData.begin(); it!=contourData.end(); )
                {
                    if (it->size()<Globals::minContourLength)
                        it=contourData.erase(it);
                    else
                        ++it;
                }
            }
            
            drawContours(out, contourData, -1, color, eye.edgeThickness, 8, contourHierarchy);
        }
        catch(Exception e){
            printf("[CVEye] ParallelContourDetector error.\n");
        }
        
        in.release();
        out.release();
    }
    
}

ParallelContourDetector::~ParallelContourDetector(){ }