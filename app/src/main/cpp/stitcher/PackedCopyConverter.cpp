//
// Created by sujin on 2016/11/17.
//

#include "PackedCopyConverter.h"
#include <memory>

PackedCopyConverter::PackedCopyConverter(size_t nPixelBytes) :
        _nPixelBytes(nPixelBytes)
{

}

PackedCopyConverter::~PackedCopyConverter()
{

}

int PackedCopyConverter::ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg)
{
    if (pSrcImg->strides[0] == pDstImg->strides[0]) {
        memcpy(pDstImg->planes[0], pSrcImg->planes[0], pSrcImg->strides[0] * _height);
        return 0;
    }
    unsigned char * writePtr = pDstImg->planes[0];
    const unsigned char * readPtr  = pSrcImg->planes[0];

    for (int row = 0; row < _height; row++) {
        memcpy(writePtr, readPtr, _width * _nPixelBytes);
        readPtr += pSrcImg->strides[0];
        writePtr += pDstImg->strides[0];
    }

    return 0;
}