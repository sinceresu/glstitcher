//
// Created by sujin on 2016/11/17.
//

#ifndef GLSTITCH_YUV444TONVCONVERTER_H
#define GLSTITCH_YUV444TONVCONVERTER_H
#include "FormatConverter.h"


class YUV444ToNVConverter  : public  FormatConverter{
public:

    /**
* Constructor
*/
    YUV444ToNVConverter();

    /**
     * Deconstructor
     */
    ~YUV444ToNVConverter();

    virtual int ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg);

};


#endif //GLSTITCH_YUV444TONVCONVERTER_H
