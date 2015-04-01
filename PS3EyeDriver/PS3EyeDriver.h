//
//  PS3EyeDriver.h
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

/**
 * Code for pulling camera data from the PS3 Eyes and giving them
 * to the outer interface (PS3EyePlugin).
 */

#ifndef __OculusEye__PS3EyeDriver__
#define __OculusEye__PS3EyeDriver__

#include <stdio.h>
#include <pthread.h>

#include "ps3eye.h"

class PS3EyeDriver {
public:
    PS3EyeDriver();
    ~PS3EyeDriver();
    
    void Init();
    
    void StartCameraUpdateThread();
    void StopCameraUpdateThread();
    pthread_t *cameraThreadID;
    int cameraThreadErr;
    bool cameraThreadStarted = false;
    
    
    static int GetNumCameras();
    
private:
    
    
};

void *CameraUpdateThread(void *arg);

#endif /* defined(__OculusEye__PS3EyeDriver__) */
