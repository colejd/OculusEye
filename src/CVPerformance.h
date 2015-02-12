//
//  CVPerformance.h
//  OculusEye
//
//  Created by Jonathan Cole on 12/15/14.
//
//

#ifndef OculusEye_CVPerformance_h
#define OculusEye_CVPerformance_h

#include <opencv2/flann/timer.h>
#include "ofMain.h"
#include <deque>

#define START_TIMER(name) cvflann::StartStopTimer name = cvflann::StartStopTimer(); name.start();

#define STOP_TIMER(name) name.stop();

#define PRINT_TIMER(name, label) printf("%s: %fs\n", label, name.value);

/**
 * Stores a queue of FPS values and draws a representative graph on-screen.
 */
class PerformanceGraph{
public:
    deque <float> fpsqueue;
    
    int sizeLimit = 180;
    std::string label;
    float xpos;
    float ypos;
    float xScale = 0.3f;
    float yScale = 0.5f;
    
    float yMax = 0.0f;
    
    PerformanceGraph(std::string label, const float _xpos, const float _ypos);
    ~PerformanceGraph();
    
    void Enqueue(const float fps);
    void SetPosition(const float x, const float y);
    void Draw();
    bool fillGraph = false;
    
    
private:
    
};

#endif