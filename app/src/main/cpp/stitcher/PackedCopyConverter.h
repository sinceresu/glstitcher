//
// Created by sujin on 2016/11/17.
//

#ifndef GLSTITCH_COPYCONVERTER_H
#define GLSTITCH_COPYCONVERTER_H


#include "FormatConverter.h"

class PackedCopyConverter   : public  FormatConverter{
public:

    /**
* Constructor
*/
    PackedCopyConverter(size_t nPixelBytes);

    /**
     * Deconstructor
     */
    ~PackedCopyConverter();

    virtual int ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg);
private:
    size_t _nPixelBytes;
};


#endif //GLSTITCH_COPYCONVERTER_H
