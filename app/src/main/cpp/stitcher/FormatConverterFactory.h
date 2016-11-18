//
// Created by sujin on 2016/11/17.
//

#ifndef GLSTITCH_FORMATCONVERTERFACTORY_H
#define GLSTITCH_FORMATCONVERTERFACTORY_H

#include <memory>
#include "types.h"

class FormatConverter;

class FormatConverterFactory {
public:
    static std::shared_ptr<FormatConverter>  CreateFormatConverter(ePixelFormat_t srcPixelFormat, ePixelFormat_t dstPixelFormat);
};


#endif //GLSTITCH_FORMATCONVERTERFACTORY_H
