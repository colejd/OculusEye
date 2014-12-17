//
//  CVPerformance.h
//  OculusEye
//
//  Created by VEMILab on 12/15/14.
//
//

#ifndef OculusEye_CVPerformance_h
#define OculusEye_CVPerformance_h

#include <opencv2/flann/timer.h>

#define START_TIMER(name) cvflann::StartStopTimer name = cvflann::StartStopTimer(); name.start();

#define STOP_TIMER(name) name.stop();

#define PRINT_TIMER(name, label) printf("%s: %fs\n", label, name.value);

#endif
