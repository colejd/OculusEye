//
//  CVEye.cpp
//  ofCV
//
//  Created by Jonathan Cole on 9/25/14.
//
//

#include "CVEye.h"

//Constructor
CVEye::CVEye(const int _width, const int _height, const int _index){
    camIndex = _index;
    width = _width;
    height = _height;
    
    finalImage.allocate(_width, _height, OF_IMAGE_COLOR);
    //finalImage.loadImage("NoCameraFound.jpg");
    //finalImage.setImageType(OF_IMAGE_COLOR);
    rawPixelData = (unsigned char*)malloc(_width*_height*3*sizeof(unsigned char));
    
    //Initialize the camera, print results
    cout << (init(_width, _height) ? "Camera created.\n" : "Camera not found.\n");
    //thread.start();
    
    yuvData = YUVBuffer();
    
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
    
    //Kill USB connection
    if(eyeRef) eyeRef->stop();
    
}

bool CVEye::init(const int _width, const int _height){
    initialized = false;
    
    using namespace ps3eye;
    // list out the devices
    std::vector<PS3EYECam::PS3EYERef> devices( PS3EYECam::getDevices(true) );
    //printf("Eye cameras detected: %lu\n", devices.size());
    if(0 < devices.size() && camIndex < devices.size())
    {
        //threadUpdate.stop();
        eyeRef = devices.at(camIndex);
        bool res = eyeRef->init(width, height, 60);
        eyeRef->start();
        
        width = eyeRef->getWidth();
        height = eyeRef->getHeight();
        
        //Populate GUI fields with default hardware values
        autoWhiteBalance = eyeRef->getAutoWhiteBalance();
        autoGain = eyeRef->getAutogain();
        gain = eyeRef->getGain();
        sharpness = eyeRef->getSharpness();
        exposure = eyeRef->getExposure();
        brightness = eyeRef->getBrightness();
        contrast = eyeRef->getContrast();
        hue = eyeRef->getHue();
        blueBalance = eyeRef->getBlueBalance();
        redBalance = eyeRef->getRedBalance();
        
        initialized = res;
    }
    
    //If no cameras are detected, a sample image is loaded.
    if(initialized == false){
        ofLog(OF_LOG_WARNING, "Using sample image");
        //If we've gotten here, the camera was not found or couldn't be initialized.
        //printf("Image %s\n", ( access( "../../../data/NoCameraFound.png", F_OK ) != -1 ) ? "exists" : "does not exist");
        rawPixelData = cv::imread("../../../data/SampleImage.png", cv::IMREAD_COLOR).data;
        dummyImage = true;
    }
    
    PullData();
    
    using namespace cv;
    
    int matType = CV_MAKE_TYPE(CV_8U, 3);
    src_tmp = Mat(cv::Size(width, height), matType, rawPixelData);
    src.create(cv::Size(width, height), matType);
    src_gray.create(cv::Size(width, height), CV_8U);
    src_tmp.copyTo(src);
    cvtColor(src, src_gray, COLOR_RGB2GRAY);
    src.copyTo(dest);
    
    canny_output.create(cv::Size(width, height), CV_8U);
    contour_output.create(cv::Size(width, height), CV_8UC3);
    
    
    return initialized;
}

/**
 * Puts RGB888 data into rawPixelData to prepare it for OpenCV processing.
 */
void CVEye::PullData(){
    if(initialized)
    {
        //Copy image data into VideoImage when there's a new image from the camera hardware.
        //bool isNewFrame = eyeRef->isNewFrame();
        if(render)
        {
            yuvData.Convert(eyeRef->getLastFramePointerVolatile(), eyeRef->getRowBytes(), rawPixelData, eyeRef->getWidth(), eyeRef->getHeight());
            
            //yuvData.LoadData(eyeRef->getLastFramePointerVolatile(), eyeRef->getRowBytes(), rawPixelData, eyeRef->getWidth(), eyeRef->getHeight());
            //yuvData.ConvertParallel(480);
        }
    }
    else{
        //Pull in a sample video frame
        //rawPixelData = cv::imread("../../../data/Video_Output.png", cv::IMREAD_COLOR).data;
    }
}

void CVEye::update(){
    if(eyeRef || dummyImage)
    {
        bool pass = dummyImage;
        //Copy image data into VideoImage when there's a new image from the camera hardware.
        if(eyeRef) pass = eyeRef->isNewFrame();
        if(pass)
        {
            
            PullData();
            src_tmp.data = rawPixelData;
            
            if(doCanny){
                
                src_tmp.copyTo(src);
                cvtColor(src, src_gray, cv::COLOR_RGB2GRAY);
                src.copyTo(dest);
                
                if(computeFrame){
                    ApplyCanny(src, src_gray, dest);
                }
                else{
                    if(showEdgesOnly) dest = cv::Scalar::all(0);
                    contour_output.copyTo(dest, contour_output);
                }
                //computeFrame = !computeFrame;
                
                //Do last
                finalImage.setFromPixels(dest.getMat(NULL).data, width, height, OF_IMAGE_COLOR);
                
            }
            //If no effects are applied, just put the raw video in the output image.
            else{
                finalImage.setFromPixels(src_tmp.data, width, height, OF_IMAGE_COLOR);
            }
            
        }
        
        //FPS counting stuff
        camFrameCount += pass ? 1: 0;
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

void CVEye::draw( ofVec2f pos, ofVec2f size ){
    
}

void CVEye::shutdown(){
    
}

/* Performs Canny edge detection on src and applies the result to dest.
 */
void CVEye::ApplyCanny(cv::UMat &src, cv::UMat &src_gray, cv::UMat &dest){
    using namespace cv;
    //START_TIMER(cannyTimer);
    
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
        resize(src_gray, src_gray, cv::Size(), adaptedDownsampleRatio, adaptedDownsampleRatio, INTER_AREA); //#1
    }
    resize(canny_output, canny_output, src_gray.size());
    resize(contour_output, contour_output, src_gray.size());
    
    //Smooth src_gray to reduce noise (Canny also smooths the image, but it is a linear blur; thus this blurring is preserved)
    int blurScale = 3;
    //Box filter blur
    blur( src_gray, src_gray, cv::Size(blurScale, blurScale) );
    
    //Gaussian blur
    //cv::GaussianBlur(src_gray, src_gray, cv::Size(7,7), 1.5, 1.5);
    
    //Bilateral blur
    //float kernelLength = 3;
    //UMat src_gray_clone = src_gray.clone();
    //bilateralFilter(src_gray_clone, src_gray, kernelLength, kernelLength*2, kernelLength/2);
    //src_gray_clone.release();
    
    if(doErosionDilution){
        erode(src_gray, src_gray, Mat(), cv::Point(-1,-1), erosionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
        dilate(src_gray, src_gray, Mat(), cv::Point(-1,-1), dilutionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
    }
    
    int edgeThresh = 1;
    int lowThreshold = cannyMinThreshold; //Max of 100 by default
    int highThreshold = lowThreshold * cannyRatio;
    int kernel_size = 3;
    
    /*
    cv::Mat temp;
    int thresholdType = THRESH_OTSU | THRESH_BINARY; //THRESH_OTSU | THRESH_BINARY;
    highThreshold = threshold(src_gray, temp, 0, 255, thresholdType);
    lowThreshold = highThreshold * 0.1;
    temp.release();
     */
    
    //cv::ocl::setUseOpenCL(true);
    //contour_output = Scalar::all(0);
    //canny_output = Scalar::all(0);
    Canny( src_gray, canny_output, lowThreshold, highThreshold, kernel_size );
    //cv::ocl::setUseOpenCL(false);
    
    //Black out the final image if we only want to see the edges
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
            resize(contour_output, contour_output, cv::Size(640, 480), 0, 0, INTER_NEAREST);
        
        //Overlay the detected contours onto the final image
        //cvtColor(contour_output, contour_output, COLOR_RGB2GRAY);
        contour_output.copyTo(dest, contour_output);
        //dest.setTo(guiLineColor, contour_output);
    }
    //Just draw the edges from canny_output to the final image
    else{
        resize(canny_output, canny_output, cv::Size(640, 480), 0, 0, INTER_NEAREST);
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


void ParallelContourDetector::operator ()(const cv::Range &range) const{
    using namespace cv;
    
    /*
    vector< vector<cv::Point> > contourData;
    vector<Vec4i> contourHierarchy;
    findContours( src_gray, contourData, contourHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    contour_output = Scalar::all(0);
    drawContours(contour_output, contourData, -1, guiLineColor, edgeThickness, 8, contourHierarchy);
     */
    
    for(int i = range.start; i < range.end; i++){
        vector< vector<cv::Point> > contourData;
        vector<Vec4i> contourHierarchy;
        //printf("%i\n", );
        cv::UMat in(src_gray, cv::Rect(0, (src_gray.rows/subsections)*i, src_gray.cols, src_gray.rows/subsections));
        
        cv::UMat out(out_gray, cv::Rect(0, (out_gray.rows/subsections)*i, out_gray.cols, out_gray.rows/subsections));
        try{
            findContours( in, contourData, contourHierarchy, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS, cv::Point(0, 0) );
            out = Scalar::all(0);
            Scalar color = eye.guiLineColor;
            //if(eye.rave) color = Scalar(rand()&255, rand()&255, rand()&255);
            drawContours(out, contourData, -1, color, eye.edgeThickness, 8, contourHierarchy);
        }
        catch(Exception e){
            printf("Error\n");
        }
        
        in.release();
        out.release();
    }
    
    //resize(src_gray, src_gray, cv::Size(640, 480), 0, 0, INTER_NEAREST);
}

ParallelContourDetector::~ParallelContourDetector(){ }