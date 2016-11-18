#include "StdAfx.h"
#include "ImageStitcher.h"


ImageStitcher::ImageStitcher() :
	m_bInitialized(false)
{
}


ImageStitcher::~ImageStitcher()
{
}
void ImageStitcher::SetSrcImageFormat(const ImageFormat_t & imageFormat)
{
	m_srcImageFormat = imageFormat;
	m_bInitialized = false;

}
void ImageStitcher::SetDstImageFormat(const ImageFormat_t & imageFormat)
{
	m_dstImageFormat = imageFormat;
	m_bInitialized = false;
}

