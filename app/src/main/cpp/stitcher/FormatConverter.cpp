//
// Created by sujin on 2016/11/17.
//

#include "FormatConverter.h"

FormatConverter::FormatConverter() :
        m_bInitialized(false)
{

}

FormatConverter::~FormatConverter()
{

}

int FormatConverter::SetImageSize(int width, int height)
{
    _width = width;
    _height = height;
    m_bInitialized = false;

    return 0;
}