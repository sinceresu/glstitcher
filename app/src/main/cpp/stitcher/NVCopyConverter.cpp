//
// Created by sujin on 2016/11/18.
//

#include "NVCopyConverter.h"
#include <memory>

NVCopyConverter::NVCopyConverter()
{

}

NVCopyConverter::~NVCopyConverter()
{

}

int NVCopyConverter::ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg)
{
    unsigned char * writePtr = pDstImg->planes[0];
    const unsigned char * readPtr  = pSrcImg->planes[0];

    for (int row = 0; row < _height; row++) {
        memcpy(writePtr, readPtr, _width);
        readPtr += pSrcImg->strides[0];
        writePtr += pDstImg->strides[0];
    }
    writePtr = pDstImg->planes[1];
    readPtr  = pSrcImg->planes[1];

    for (int row = 0; row < _height / 2; row++) {
        memcpy(writePtr, readPtr, _width);
        readPtr += pSrcImg->strides[1];
        writePtr += pDstImg->strides[1];
    }

    return 0;
}