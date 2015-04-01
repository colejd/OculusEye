//
//  UnityExports.h
//  OculusEye
//
//  Created by Jonathan Cole on 3/31/15.
//
//

#ifndef __OculusEye__UnityExports__
#define __OculusEye__UnityExports__

#include <stdio.h>

extern "C"{
    void InitDriver();
    void Begin();
    void End();
    int GetNumCameras();
}

#endif /* defined(__OculusEye__UnityExports__) */
