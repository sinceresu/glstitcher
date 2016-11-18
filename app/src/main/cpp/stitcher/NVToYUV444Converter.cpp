//
// Created by sujin on 2016/11/17.
//

#include "NVToYUV444Converter.h"

NVToYUV444Converter::NVToYUV444Converter()
{

}

NVToYUV444Converter::~NVToYUV444Converter()
{

}


int NVToYUV444Converter::ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg)
{
    const unsigned  char* pSrcY = pSrcImg->planes[0];
    const unsigned  char* pSrcUV = pSrcImg->planes[1];
    unsigned  char* pDstYUV = pDstImg->planes[0];

    for (int row = 0; row < _height; row+=2) {
        unsigned  char* pDstNextYUV = pDstYUV + pDstImg->strides[0];
        const unsigned  char* pSrcNextY = pSrcY + pSrcImg->strides[0];
        for (int col = 0; col < _width; col +=2 ) {
            unsigned char u = pSrcUV[col];
            unsigned char v = pSrcUV[col + 1];
            pDstYUV[3*col] = pSrcY[col];
            pDstYUV[3*col + 1] = u;
            pDstYUV[3*col + 2] = v;
            pDstNextYUV[3*col] = pSrcNextY[col];
            pDstNextYUV[3*col + 1] = u;
            pDstNextYUV[3*col + 2] = v;

            pDstYUV[3*(col + 1)] = pSrcY[(col + 1)];
            pDstYUV[3*(col + 1) + 1] = u;
            pDstYUV[3*(col + 1) + 2] = v;
            pDstNextYUV[3*(col + 1)] = pSrcNextY[(col + 1)];
            pDstNextYUV[3*(col + 1) + 1] = u;
            pDstNextYUV[3*(col + 1) + 2] = v;

        }
        pSrcY += 2 * pSrcImg->strides[0];
        pSrcUV += pSrcImg->strides[1];
        pDstYUV += 2 * pDstImg->strides[0];

    }
    return  0;
}