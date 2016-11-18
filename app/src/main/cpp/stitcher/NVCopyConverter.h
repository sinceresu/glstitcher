//
// Created by sujin on 2016/11/18.
//

#ifndef GLSTITCH_NVCOPYCONVERTER_H
#define GLSTITCH_NVCOPYCONVERTER_H

#include "FormatConverter.h"

class NVCopyConverter   : public  FormatConverter {
public:

    /**
* Constructor
*/
    NVCopyConverter();

    /**
     * Deconstructor
     */
    ~NVCopyConverter();

    virtual int ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg);

};


#endif //GLSTITCH_NVCOPYCONVERTER_H
