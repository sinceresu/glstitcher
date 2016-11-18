//
// Created by sujin on 2016/11/17.
//

#include "YUV444ToNVConverter.h"

YUV444ToNVConverter::YUV444ToNVConverter()
{

}

YUV444ToNVConverter::~YUV444ToNVConverter()
{

}


int YUV444ToNVConverter::ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg) {
    const unsigned char *pSrcYUV = pSrcImg->planes[0];
    unsigned char *pDstY = pDstImg->planes[0];
    unsigned char *pDstUV = pDstImg->planes[1];

    for (int row = 0; row < _height; row += 2) {
        unsigned char *pDstNextY = pDstY + pDstImg->strides[0];
        const unsigned char *pSrcNextYUV = pSrcYUV + pSrcImg->strides[0];
        for (int col = 0; col < _width; col += 2) {
            unsigned char y = pSrcYUV[3 * col];
            unsigned char u = pSrcYUV[3 * col + 1];
            unsigned char v = pSrcYUV[3 * col + 2];

            pDstY[col] = y;
            pDstUV[col] = u;
            pDstUV[col + 1] = v;

            y = pSrcNextYUV[3 * col];

            pDstNextY[col] = y;


            y = pSrcYUV[3 * (col + 1)];

            pDstY[col + 1] = y;


            y = pSrcNextYUV[3 * (col + 1)];
            pDstNextY[col + 1] = y;

        }
        pSrcYUV += 2 * pSrcImg->strides[0];
        pDstY += 2 * pDstImg->strides[0];
        pDstUV += pDstImg->strides[1];

    }
    return 0;

}
