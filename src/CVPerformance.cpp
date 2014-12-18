//
//  CVPerformance.cpp
//  OculusEye
//
//  Created by VEMILab on 12/17/14.
//
//

#include "CVPerformance.h"

PerformanceGraph::PerformanceGraph(std::string _label, float _xpos, float _ypos){
    fpsqueue = deque<float>(sizeLimit);
    fpsqueue.assign(0.0f, sizeLimit);
    xpos = _xpos;
    ypos = _ypos;
    label = _label;
}

PerformanceGraph::~PerformanceGraph(){
    
}

/**
 * Push an FPS value to the front of the queue to be drawn.
 */
void PerformanceGraph::Enqueue(float fps){
    fpsqueue.push_front(fps);
    if(fps > yMax) yMax = fps;
    fpsqueue.resize(sizeLimit);
}

/**
 * Draws a graph representing fpsqueue to the screen at (xpos, ypos) (the bottom left corner).
 * Lots of magic numbers, but it looks good for now.
 */
void PerformanceGraph::Draw(){
    float displayedFPS;
    float adjustedYScale = (yScale * (55.0f / yMax)); //60.0f
    
    //Draw label
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(label, xpos-1, ypos - (yMax * adjustedYScale) - 7);
    
    //Draw background box
    ofSetColor(0, 0, 0);
    ofRect(xpos, ypos, sizeLimit * xScale, -yMax * adjustedYScale);
    
    //Draw graph
    ofSetColor(0, 255, 0);
    ofPolyline graphLine;
    for(int i=0; i<sizeLimit; ++i){
        float f = fpsqueue.at(i);
        if(i == 1) displayedFPS = f;
        if(fillGraph) graphLine.addVertex(xpos + (i * xScale), ypos);
        graphLine.addVertex(xpos + (i * xScale), ypos - (f * adjustedYScale));
    }
    graphLine.draw();
    
    //Draw FPS string
    ofSetColor(255, 0, 0);
    ofDrawBitmapString(ofToString(displayedFPS, 2), xpos + 3, ypos - 10);
    
    //Draw border box
    ofSetColor(255, 0, 0);
    ofPolyline box;
    box.addVertex(xpos, ypos);
    box.addVertex(xpos + (sizeLimit * xScale), ypos);
    box.addVertex(xpos + (sizeLimit * xScale), ypos - (60.0f * yScale));
    box.addVertex(xpos, ypos - (60.0f * yScale));
    box.addVertex(xpos, ypos);
    box.draw();          
                  
                  
}