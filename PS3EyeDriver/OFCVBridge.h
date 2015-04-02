//
//  OFCVBridge.h
//  OculusEye
//
//  Created by Jonathan Cole on 9/22/14.
//
//

/**
 * The cameras give us image data in the YUV422 color space.
 * This module is for color conversion from YUV422 to RGB888.
 */

#include <stdint.h>

#ifndef __ofCV__OFCVBridge__
#define __ofCV__OFCVBridge__

//--------------------------------------------------------------
#define _max(a, b) (((a) > (b)) ? (a) : (b))
#define _saturate(v) static_cast<uint8_t>(static_cast<uint32_t>(v) <= 0xff ? v : v > 0 ? 0xff : 0)

const int ITUR_BT_601_CY = 1220542;
const int ITUR_BT_601_CUB = 2116026;
const int ITUR_BT_601_CUG = -409993;
const int ITUR_BT_601_CVG = -852492;
const int ITUR_BT_601_CVR = 1673527;
const int ITUR_BT_601_SHIFT = 20;

const int SHIFT_THING = (1 << (ITUR_BT_601_SHIFT - 1));

const int bIdx = 2;
const int uIdx = 0;
const int yIdx = 0;

const int uidx = 1 - yIdx + uIdx * 2;
const int vidx = (2 + uidx) % 4;

extern int const LUT_RV[256];
extern int const LUT_GU[256];
extern int const LUT_GV[256];
extern int const LUT_BU[256];
extern int const LUT_SATURATE[256];

void yuv422_to_rgba(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height);
void yuv422_to_rgb(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height);
void rgba_to_rgb(const uint8_t *src, uint8_t *dest, const int srcLength, const int destLength);
void yuv422_to_rgb_optimized(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height);

/*
 * A fast decoder for YUV422 to RGB88 color conversion.
 * Supports multithreading through the ParallelLoopBody class of TBB.
 */
class YUVBuffer {
    private:
        
    public:
    
        uint8_t *src;
        uint8_t *dst;
        int width;
        int height;
        int stride; //Bytes per row
    
        int channels = 3;
    
        YUVBuffer();
        ~YUVBuffer();
    
        //void operator ()(const cv::Range &range) const;
    
        void LoadData(uint8_t *source, const int stride, uint8_t *dest, const int width, const int height);
    
        void ConvertParallel(const int numRows);
    
        void Convert(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height);
    
        void Build_LUTs();
    
    
};


#endif /* defined(__ofCV__OFCVBridge__) */