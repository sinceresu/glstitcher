//
// Created by sujin on 2016/11/17.
//


#include "FormatConverterFactory.h"
#include "PackedCopyConverter.h"


std::shared_ptr<FormatConverter> FormatConverterFactory::CreateFormatConverter(ePixelFormat_t srcPixelFormat, ePixelFormat_t dstPixelFormat)
{
    if (dstPixelFormat == PIXELFORMAT_444 && srcPixelFormat == PIXELFORMAT_444) {
        return std::make_shared<PackedCopyConverter>(3);
    }
    if (dstPixelFormat == PIXELFORMAT_ARGB && srcPixelFormat == PIXELFORMAT_ARGB) {
        return std::make_shared<PackedCopyConverter>(4);
    }

    if (dstPixelFormat == PIXELFORMAT_RGB && srcPixelFormat == PIXELFORMAT_RGB) {
        return std::make_shared<PackedCopyConverter>(3);
    }


    return std::shared_ptr<FormatConverter>();
}