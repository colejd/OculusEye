//
//  mainApp.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 9/25/14.
//
//

#include "mainApp.h"



/**
 * Called once at the start of the application.
 */
void mainApp::setup(){
    
    //Set the minimum type of log that is printed (lower priority logs will be ignored)
    ofSetLogLevel( OF_LOG_NOTICE );
    
    char pathBuf[ PATH_MAX ];
    printf("CWD: %s\n", getcwd(pathBuf, 512));
    
    //string cmd = "ls ../../../";
    //string cmd_output = GetStdoutFromCommand(cmd);
    //printf("%s\n%s", cmd.c_str(), cmd_output.c_str());
    
    ofLog(OF_LOG_VERBOSE, "OpenCV build information:\n");
    ofLog(OF_LOG_VERBOSE, "%s\n", cv::getBuildInformation().c_str());
    
    cv::ipp::setUseIPP(true);
    printf("Using IPP: %s\n", cv::ipp::useIPP() ? "true" : "false");
    
    char name[128];
    dispatch_queue_t queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_GPU, NULL);
    if (queue == NULL) {
        queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_CPU, NULL);
    }
    cl_device_id gpu = gcl_get_device_id_with_dispatch_queue(queue);
    clGetDeviceInfo(gpu, CL_DEVICE_NAME, 128, name, NULL);
    fprintf(stdout, "Created a dispatch queue using the %s\n", name);
    
    ofLog(OF_LOG_NOTICE, " OpenCL acceleration is %s ", cv::ocl::haveOpenCL() ? "available" : "not available");
    ofLog(OF_LOG_NOTICE, " OpenCL acceleration %s ", USE_OPENCL ? "requested" : "not requested");
    cv::ocl::setUseOpenCL(USE_OPENCL);
    if(USE_OPENCL){
        printf("Using OpenCL: %s\n", cv::ocl::useOpenCL() ? "true" : "false");
        std::vector<cv::ocl::PlatformInfo> platforms;
        cv::ocl::getPlatfomsInfo(platforms);
        for(int i=0; i<platforms.size(); i++){
            printf("  Platform %i of %lu\n", i, platforms.size()-1);
            printf("    Name:     %s\n", platforms[i].name().c_str());
            printf("    Vendor:   %s\n", platforms[i].vendor().c_str());
            printf("    Device:   %i\n", platforms[i].deviceNumber());
            printf("    Version:  %s\n", platforms[i].version().c_str());
        }
    }
    
    ofLog(OF_LOG_NOTICE, " CPUs available:    %2.i", cv::getNumberOfCPUs());
    ofLog(OF_LOG_NOTICE, " Threads available: %2.i", cv::getNumThreads());
    
    #if SYSTEM_FULLSCREEN
        //TODO: Why does the camera fps go down when I use this?
        changeScreenRes(TARGET_RES_X, TARGET_RES_Y);
        //Set fullscreen and change resolution.
        ofSetFullscreen(true);
    #endif
    
    ofSetFrameRate(TARGET_FRAMERATE);
    ofLog(OF_LOG_NOTICE, " Target framerate set to %i", TARGET_FRAMERATE);
    
    useVSync = ENABLE_VSYNC;
    ofSetVerticalSync(useVSync);
    ofLog(OF_LOG_NOTICE, " Vertical sync is %s ", ENABLE_VSYNC ? "on" : "off");
    
    oculusRift.init( TARGET_RES_X, TARGET_RES_Y, 0 ); //1280, 800, 4
	oculusRift.setPosition(0, 0, 0);
    
    //Set up Oculus Rift
    oculusRift.interOcularDistance = -0.65f; //IPD of 0.65 is average
    
    eyeFPSGraph = PerformanceGraph("Eye FPS", ofGetWidth() - 70, ofGetHeight() - 15);
    
    //Set up GUI
    CreateGUI();
    
    InitEyes();
    
}

void mainApp::CreateGUI(){
    
    //GUI page 1
    generalSettingsBar = ofxTweakbars::create("General Settings", "General Settings");
    generalSettingsStorage = new ofxTweakbarSimpleStorage(generalSettingsBar);
    
    generalSettingsBar -> addBool("Canny Edge Detection", doCanny) -> setLabel("Canny Edge Detection");
    generalSettingsBar -> addFloat("Canny Min Threshold", cannyMinThreshold) -> setLabel("Canny Min Threshold") -> setMin("0") -> setMax("100");
    generalSettingsBar -> addFloat("Canny Ratio", cannyRatio) -> setLabel("Canny Ratio") -> setMin("2") -> setMax("3");
    generalSettingsBar -> addColor3f("Edge Color", pickerColor) -> setLabel("Edge Color");
    generalSettingsBar -> addBool("Draw Contours", drawContours) -> setLabel("Draw Contours") -> setGroup("Contour Settings");
    generalSettingsBar -> addInt("Edge Thickness", edgeThickness) -> setLabel("Edge Thickness") -> setGroup("Contour Settings") -> setMin("-1") -> setMax("5");
    generalSettingsBar -> addBool("Erosion/Dilution", doErosionDilution) -> setLabel("Erosion/Dilution") -> setGroup("Erosion/Dilution Settings");
    generalSettingsBar -> addInt("Erosion Iterations", erosionIterations) -> setLabel("Erosion Iterations") -> setGroup("Erosion/Dilution Settings") -> setMin("1") -> setMax("5");
    generalSettingsBar -> addInt("Dilution Iterations", dilutionIterations) -> setLabel("Dilution Iterations") -> setGroup("Erosion/Dilution Settings") -> setMin("1") -> setMax("5");
    generalSettingsBar -> addBool("Adaptive Downsampling", adaptiveDownsampling) -> setLabel("Adaptive Downsampling");
    generalSettingsBar -> addFloat("Downsampling Ratio", downsampleRatio) -> setLabel("Downsampling Ratio") -> setMin("0.01") -> setMax("1.0");
    generalSettingsBar -> addInt("Subdivisions (Cores Used)", imageSubdivisions) -> setLabel("Subdivisions (Cores Used)") -> setMin("1") -> setMax("8");
    generalSettingsBar -> addBool("Show Edges Only", showEdgesOnly) -> setLabel("Show Edges Only");
    generalSettingsBar -> addBool("Rift Distortion", oculusRift.doWarping) -> setLabel("Rift Distortion");
    generalSettingsBar -> addInt("Interpupillary -/+", oculusRift.ipd) -> setLabel("Interpupillary Distance [-/+]") -> setMin("0") -> setMax("100");
    generalSettingsBar -> addBool("Draw Guides", drawGuides) -> setLabel("Draw Guides");
    generalSettingsBar -> addBool("swapEyes", swapEyes) -> setLabel("Swap Eyes");
    generalSettingsBar -> addBool("showPerformanceGraph", showPerformanceGraph) -> setLabel("Show Performace Graph");
    generalSettingsBar -> addBool("useVSync", useVSync) -> setLabel("Use VSync (caps at 60 FPS)");
    generalSettingsBar -> addBool("correctCameraDistortion", correctCameraDistortion) -> setLabel("Correct Camera Distortion");
    
    generalSettingsStorage -> retrieve();
    generalSettingsBar -> load();
    generalSettingsStorage -> store();
    //generalSettingsBar -> close();
    
    //Set settings menu properties
    generalSettingsBar -> setSize(400, 300);
    generalSettingsBar -> setColor(44, 44, 44, 180);
    generalSettingsBar -> setFontSize(2);
    
    //GUI page 2
    ps3EyeSettings = ofxTweakbars::create("PS3 Eye Settings", "PS3 Eye Settings");
    ps3EyeSettingsStorage = new ofxTweakbarSimpleStorage(ps3EyeSettings);
    
    ps3EyeSettings -> addBool("autoWhiteBalance", autoWhiteBalance) -> setLabel("Auto White Balance");
    ps3EyeSettings -> addBool("autoGain", autoGain) -> setLabel("Auto Gain");
    ps3EyeSettings -> addFloat("gain", gain) -> setLabel("Gain") -> setMin("0") -> setMax("63");
    ps3EyeSettings -> addFloat("sharpness", sharpness) -> setLabel("Sharpness") -> setMin("0") -> setMax("63");
    ps3EyeSettings -> addFloat("exposure", exposure) -> setLabel("Exposure") -> setMin("0") -> setMax("255");
    ps3EyeSettings -> addFloat("brightneww", brightness) -> setLabel("Brightness") -> setMin("0") -> setMax("255");
    ps3EyeSettings -> addFloat("contrast", contrast) -> setLabel("Contrast") -> setMin("0") -> setMax("255");
    ps3EyeSettings -> addFloat("hue", hue) -> setLabel("Hue") -> setMin("0") -> setMax("255");
    ps3EyeSettings -> addFloat("blueBalance", blueBalance) -> setLabel("Blue Balance") -> setMin("0") -> setMax("255");
    ps3EyeSettings -> addFloat("redBalance", redBalance) -> setLabel("Red Balance") -> setMin("0") -> setMax("255");
    
    ps3EyeSettingsStorage -> retrieve();
    ps3EyeSettings -> load();
    ps3EyeSettingsStorage -> store();
    ps3EyeSettings -> close();
    
    ps3EyeSettings -> setSize(400, 300);
    ps3EyeSettings -> setColor(44, 44, 44, 180);
    ps3EyeSettings -> setFontSize(2);
    
    otherSettings = ofxTweakbars::create("Other Settings", "Other Settings");
    otherSettingsStorage = new ofxTweakbarSimpleStorage(otherSettings);
    
    otherSettings -> addBool("computeDisparityMap", computeDisparityMap) -> setLabel("Compute Disparity Map");
    otherSettings -> addBool("showDisparityMap", showDisparityMap) -> setLabel("Show Disparity Map");
    otherSettings -> addInt("stereo_lowThreshold", stereoMapper.lowThreshold) -> setLabel("Low Threshold") -> setMin("0") -> setMax("255");
    otherSettings -> addInt("stereo_highThreshold", stereoMapper.highThreshold) -> setLabel("High Threshold") -> setMin("0") -> setMax("255");
    
    otherSettingsStorage -> retrieve();
    otherSettings -> load();
    otherSettingsStorage -> store();
    otherSettings -> close();
    
    otherSettings -> setSize(400, 300);
    otherSettings -> setColor(44, 44, 44, 180);
    otherSettings -> setFontSize(2);
    
    /*
    //GUI page 3
    gui.addPage("Oculus Settings");
    gui.addToggle("Render Left Eye", renderLeftEye);
    gui.addToggle("Swap Eyes", swapEyes);
    //gui.addSlider("Interocular Distance", oculusRift.interOcularDistance, -0.60f, -0.70f);
    gui.addToggle("Render Right Eye", renderRightEye).setNewColumn(true);
    //Finish GUI setup (do last)
    gui.loadFromXML();
    gui.setPage(1);
    gui.show();
     */
    
}

/**
 * Called once at the end of the application.
 */
void mainApp::exit(){
    
    //Save settings here
    generalSettingsStorage -> store();
    //ps3EyeSettingsStorage -> store(); //Don't do this unless you know the defaults won't be overwritten
    otherSettingsStorage -> store();
    
    //Clean up
    threadUpdate.stop(); //Stop USB update thread. Do this before deleting USB objects
    delete leftEye;  //Stop USB connections; very important
    delete rightEye; //Stop USB connections; very important
    printf("USB Connections stopped.\n");
    
    //Restore screen resolution
    #if SYSTEM_FULLSCREEN
        changeScreenRes(DEFAULT_RES_X, DEFAULT_RES_Y);
    #endif
    printf("Main app cleaned up and exited.\n");
}


/**
 * Called once per frame. Calculations should go here.
 */
void mainApp::update()
{
    //Update each CVEye (no synchronization)
    leftEye->update();
    rightEye->update();
    
    //Check for synchronization
    if(leftEye->sync_update == rightEye->sync_update){
        SynchronizedUpdate();
        
        //Reset for the next loop
        leftEye->sync_update = false;
        rightEye->sync_update = false;
    }
    
    //End camera calibration if it is running and both cameras have been successfully calibrated
    if(calibrating && (leftEye->calibrator->calibrated)){
        EndCameraCalibration();
    }
    
}

/**
 * Called when both CVEye objects have a new frame ready.
 */
void mainApp::SynchronizedUpdate(){
    if(computeDisparityMap){
        stereoMapper.CalculateStereoMap(leftEye->src_gray.getMat(NULL), rightEye->src_gray.getMat(NULL), swapEyes);
    }
}


/**
 * Called after Update() once per frame. Just draw here.
 */
void mainApp::draw()
{
    //mainFBO.begin();
    
    // STEP 1: DRAW CAMERA OUTPUT --------------------------------------------
    //Perform OpenCV operations on each eye and queue the results
    //to be drawn on their respective sides
    if((leftEye->initialized || leftEye->dummyImage) && renderLeftEye){
        
        //Queue the left Eye's image into the oculusRift FBO.
        oculusRift.leftBackground = &getImageForSide(true);
        //Render into the left FBO.
        oculusRift.beginRenderSceneLeftEye();
        //Draw geometry here if you want
        oculusRift.endRenderSceneLeftEye();
    }
    if((rightEye->initialized || leftEye->dummyImage) && renderRightEye){
        
        //Queue the right Eye's image into the oculusRift FBO.
        oculusRift.rightBackground = &getImageForSide(false);
        //Render into the right FBO.
        oculusRift.beginRenderSceneRightEye();
        //Draw geometry here if you want
        oculusRift.endRenderSceneRightEye();
    }
    
    //Set opacity to full and draw both eyes at once.
    ofSetColor( 255 );
    oculusRift.draw( ofVec2f(0,0), ofVec2f( ofGetWidth(), ofGetHeight() ) );
    
    // STEP 2: DRAW USER GRAPHICS --------------------------------------------

    if(showDisparityMap){
        DrawCVMat(stereoMapper.stereoMap, OF_IMAGE_GRAYSCALE, 0, 0, "Disparity Map");
    }
    
    // STEP 3: DRAW DEBUG (topmost layer) ------------------------------------

    //Draw debug text in top-right
    string str = "app fps: ";
	str += ofToString(ofGetFrameRate(), 2) + "\n";
    //if(leftEye->initialized || leftEye->dummyImage) str += "left eye fps: " + ofToString(leftEye->camFps, 2) + "\n";
    if(leftEye->screenMessage != "") str += "left eye: " + leftEye->screenMessage + "\n";
    //if(rightEye->initialized || leftEye->dummyImage) str += "right eye fps: " + ofToString(rightEye->camFps, 2) + "\n";
    if(rightEye->screenMessage != "") str += "right eye: " + rightEye->screenMessage + "\n";
    ofDrawBitmapString(str, 10, 15);
    
    //Draw downsampling information to screen
    if(adaptiveDownsampling){
        string dsLabel = "Downsample Ratio: ";
        dsLabel += ofToString(leftEye->adaptedDownsampleRatio, 2) + "\n";
        ofDrawBitmapString(dsLabel, 10, ofGetHeight() - 10);
    }
    
    //Draw vertical/horizontal guides for checking eye alignment at a glance
    if(drawGuides){
        ofPolyline vLine;
        vLine.addVertex(ofGetWidth()/2.0f, 0.0f);
        vLine.addVertex(ofGetWidth()/2.0f, ofGetHeight());
        vLine.draw();
        
        ofPolyline hLine;
        hLine.addVertex(0.0f, ofGetHeight()/2.0f);
        hLine.addVertex(ofGetWidth(), ofGetHeight()/2.0f);
        hLine.draw();
    }
    
    if(showPerformanceGraph){
        eyeFPSGraph.Enqueue((leftEye->camFps + rightEye->camFps) / 2.0f);
        eyeFPSGraph.Draw();
    }
    
    //mainFBO.end();
    //mainFBO.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    ofxTweakbars::draw();
    
    //If we're calibrating the camera, clear the screen and draw the calibration checkerboard and preview.
    if(calibrating){
        ofClear(0);

        //Load and draw the checkerboard
        cv::Mat checkerboard = cv::imread("../../../data/images/CalibrationCheckerboard.png", cv::IMREAD_ANYCOLOR);
        DrawCVMat(checkerboard, OF_IMAGE_COLOR, 0, 0);
        
        //Make text for the image with information about the capture process
        std::ostringstream stringStream;
        stringStream << " Calibrating...\n";
        stringStream << " Frames Captured: " << leftEye->calibrator->successes << "/" << leftEye->calibrator->numBoards + 1;
        std::string caption = stringStream.str();
        
        //Draw the camera view
        DrawCVMat(leftEye->dest.getMat(NULL), OF_IMAGE_COLOR, checkerboard.cols, 0, 240, 180, caption);
    }
    
}

/**
 * Returns the image for the left or right side of the
 * screen based on isSwapped.
 */
ofImage& mainApp::getImageForSide(const bool isLeft){
    bool isSwapped = swapEyes;
    //if(duplicateEyes) return leftEye->finalImage;
    if(isLeft){
        if(isSwapped) return rightEye->finalImage;
        else return leftEye->finalImage;
    }
    else{
        if(isSwapped) return leftEye -> finalImage;
        else return rightEye->finalImage;
    }
}

//--------------------------------------------------------------
//CVEye methods

/**
 * Creates the CVEye instances that manage camera data and starts
 * the thread that polls the USB ports.
 */
void mainApp::InitEyes(){
    
    // list out the devices
    std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices(true) );
    printf("Eye cameras detected: %lu\n", devices.size());
    
    leftEye = new CVEye(0);
    rightEye = new CVEye(1);
    
    if(devices.size() > 0){
        threadUpdate.start();
    }
    
    //Pull default values from the camera hardware and update the GUI / CVEyes present
    if(leftEye->initialized){
        autoWhiteBalance = leftEye->eyeRef->getAutoWhiteBalance();
        autoGain = leftEye->eyeRef->getAutogain();
        gain = leftEye->eyeRef->getGain();
        sharpness = leftEye->eyeRef->getSharpness();
        exposure = leftEye->eyeRef->getExposure();
        brightness = leftEye->eyeRef->getBrightness();
        contrast = leftEye->eyeRef->getContrast();
        hue = leftEye->eyeRef->getHue();
        blueBalance = leftEye->eyeRef->getBlueBalance();
        redBalance = leftEye->eyeRef->getRedBalance();
        
        UpdateEyeCamera(leftEye);
    }
    UpdateEyeValues(leftEye);
    if(rightEye->initialized){
        UpdateEyeCamera(rightEye);
    }
    UpdateEyeValues(rightEye);
    
}

void mainApp::DrawCVMat(const cv::Mat& mat, ofImageType type, int x, int y, string caption){
    DrawCVMat(mat, type, x, y, mat.cols, mat.rows, caption);
}

void mainApp::DrawCVMat(const cv::Mat& mat, ofImageType type, int x, int y, int w, int h, string caption){
    
    //Draw background box
    int borderWidth = 2;
    ofSetColor(255, 255, 255); //Box color
    ofDrawPlane(x-borderWidth, y-borderWidth, w + (borderWidth*2), h + (borderWidth*2) + 10);
    
    //Draw image from mat
    ofImage output;
    output.allocate(mat.cols, mat.rows, type);
    output.setFromPixels(mat.data, mat.cols, mat.rows, type);
    output.draw(x, y, w, h);
    
    //Draw the caption if available
    ofSetColor(255, 255, 255); //Text color
    if(caption != ""){
        ofDrawBitmapString(caption, x, y + h + 10);
    }
}


/**
 * Expensive. Don't call often.
 */
void mainApp::UpdateEyeCamera(CVEye *eye){
    if(eye->eyeRef){
        eye->eyeRef->setAutoWhiteBalance(autoWhiteBalance);
        eye->eyeRef->setAutogain(autoGain);
        eye->eyeRef->setGain(gain);
        eye->eyeRef->setSharpness(sharpness);
        eye->eyeRef->setExposure(exposure);
        eye->eyeRef->setBrightness(brightness);
        eye->eyeRef->setContrast(contrast);
        eye->eyeRef->setHue(hue);
        eye->eyeRef->setBlueBalance(blueBalance);
        eye->eyeRef->setRedBalance(redBalance);
    }
}

/**
 * Sends GUI values to the CVEye instances.
 */
void mainApp::UpdateEyeValues(CVEye *eye){
    eye->doCanny = doCanny;
    eye->cannyMinThreshold = cannyMinThreshold;
    eye->cannyRatio = cannyRatio;
    eye->edgeThickness = edgeThickness;
    eye->showEdgesOnly = showEdgesOnly;
    eye->doWarping = oculusRift.doWarping;
    eye->guiLineColor = cv::Scalar(pickerColor[0] * 255.0f, pickerColor[1] * 255.0f, pickerColor[2] * 255.0f);
    eye->doErosionDilution = doErosionDilution;
    eye->dilutionIterations = dilutionIterations;
    eye->erosionIterations = erosionIterations;
    eye->downsampleRatio = downsampleRatio;
    eye->adaptiveDownsampling = adaptiveDownsampling;
    eye->imageSubdivisions = imageSubdivisions;
    eye->drawContours = drawContours;
    ofSetVerticalSync(useVSync);
}

void mainApp::BeginCameraCalibration(){
    calibrating = true;
    if(leftEye->initialized){
        leftEye->calibrator->BeginCalibration();
        leftEye->screenMessage = "Calibrating...";
    }
    if(rightEye->initialized){
        //rightEye->calibrator.BeginCalibration();
        rightEye->screenMessage = "Calibrating...";
    }
}

void mainApp::EndCameraCalibration(bool stopEarly){
    if(calibrating && stopEarly){
        leftEye->calibrator->EndCalibration(true);
        //rightEye->calibrator->EndCalibration(true);
        
    }
    leftEye->screenMessage = "";
    rightEye->screenMessage = "";
    calibrating = false;
}

//--------------------------------------------------------------
// UI Methods
/**
 * This method fires when a key is pressed.
 */
void mainApp::keyPressed(int key){
    switch(key) {
        case 'f': ofToggleFullscreen(); break;
        case 'c': if(!calibrating) BeginCameraCalibration(); else EndCameraCalibration(true); break;
        case '=': oculusRift.ipd += 1; break;
        case '-': oculusRift.ipd -= 1; if(oculusRift.ipd < 0) oculusRift.ipd = 0; break;

    }
    UpdateEyeValues(leftEye);
    if(leftEye->initialized){
        UpdateEyeCamera(leftEye);
    }
    UpdateEyeValues(rightEye);
    if(rightEye->initialized){
        UpdateEyeCamera(rightEye);
    }
    
}

/**
 * This method fires when a key is released.
 */
void mainApp::keyReleased(int key){

}

/**
 * This method fires when the mouse is moved.
 */
void mainApp::mouseMoved(int x, int y ){

}

/**
 * This method fires when the mouse is dragged.
 */
void mainApp::mouseDragged(int x, int y, int button){
    if(leftEye){ UpdateEyeValues(leftEye); }
    if(rightEye){ UpdateEyeValues(rightEye); }
}

/**
 * This method fires when a mouse button is down.
 */
void mainApp::mousePressed(int x, int y, int button){
    //Update values to reflect GUI state
    //These operations are expensive. Use them only when necessary.
    if(leftEye){
        UpdateEyeCamera(leftEye);
        UpdateEyeValues(leftEye);
    }
    if(rightEye){
        UpdateEyeCamera(rightEye);
        UpdateEyeValues(rightEye);
    }
}

/**
 * This method fires when a mouse button is up.
 */
void mainApp::mouseReleased(int x, int y, int button){
    //Update values to reflect GUI state
    //These operations are expensive. Use them only when necessary.
    if(leftEye){
        UpdateEyeCamera(leftEye);
        UpdateEyeValues(leftEye);
    }
    if(rightEye){
        UpdateEyeCamera(rightEye);
        UpdateEyeValues(rightEye);
    }
}

/**
 * This method fires when the window is resized.
 */
void mainApp::windowResized(int w, int h){

}

/**
 * This method fires when the window is dragged.
 */
void mainApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void mainApp::gotMessage(ofMessage msg){
    
}

/**
 * Prints out the output from a system command.
 * From https://www.jeremymorgan.com/tutorials/c-programming/how-to-capture-the-output-of-a-linux-command-in-c/
 */
string mainApp::GetStdoutFromCommand(string cmd) {
    
    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    
    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
    return data;
}

