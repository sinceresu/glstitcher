//
// Created by sujin on 2016/11/17.
//

#ifndef GLSTITCH_NV21TOYUV444CONVERTER_H
#define GLSTITCH_NV21TOYUV444CONVERTER_H

#include "FormatConverter.h"

class NVToYUV444Converter  : public  FormatConverter{
public:

    /**
* Constructor
*/
    NVToYUV444Converter();

    /**
     * Deconstructor
     */
    ~NVToYUV444Converter();

    virtual int ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg);

};


#endif //GLSTITCH_NV21TOYUV444CONVERTER_H
