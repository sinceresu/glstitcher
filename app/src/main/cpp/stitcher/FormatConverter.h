//
// Created by sujin on 2016/11/17.
//

#ifndef GLSTITCH_FORMATCONVERTER_H
#define GLSTITCH_FORMATCONVERTER_H

#include "types.h"

class FormatConverter {

public:

    /**
* Constructor
*/
    FormatConverter();

    /**
     * Deconstructor
     */
    virtual ~FormatConverter();

    int SetImageSize(int width, int height);

    virtual int ConvertImage(const VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg) = 0;

protected:
    int	        _width;
    int	        _height;
    bool m_bInitialized;
};


#endif //GLSTITCH_FORMATCONVERTER_H
