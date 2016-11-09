#pragma once
#include <memory>
#include <vector>
#include <string>
#include "types.h"


class ImageStitcher
{
public:
	ImageStitcher();
	virtual ~ImageStitcher();
	void SetSrcImageFormat(const ImageFormat_t& imageFormat);
	void SetDstImageFormat(const ImageFormat_t& imageFormat);
	virtual void SetMapFilePath(const char * szMapFilePath);
	virtual int StitchImage(VideoFrame_t *pSrcImg, VideoFrame_t *pDstImg) = 0;

protected:
	ImageFormat_t	m_srcImageFormat;
	ImageFormat_t	m_dstImageFormat;
	std::string		m_strMapFilePath;
	bool			m_bInitialized;
};
