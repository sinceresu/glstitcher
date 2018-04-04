#pragma once
#include "types.h"
#include <vector>

typedef struct ColorCorrectorParameters
{

	int image_width;
	int image_height;
	int warp_step_x;
	int warp_step_y;

	TabledImageROI_t first_region;
	TabledImageROI_t second_region[2];
} ColorCorrectorParameters;

class ColorCorrector
{
public:
	ColorCorrector();
	~ColorCorrector();
	void SetSrcImageFormat(const ImageFormat_t& imageFormat);
	void SetDstImageFormat(const ImageFormat_t& imageFormat);

	void SetParameter(const ColorCorrectorParameters & param);

	bool BuildMatchPoints(std::vector<AdjustFloatMapPoint*> pFrontMaps, 
		std::vector<AdjustFloatMapPoint*> pBackMaps);
	float CalcAdjustCoeff(const VideoFrame_t *pFrontImg, const VideoFrame_t * pBackImg);

private:
	ImageFormat_t	m_srcImageFormat;
	ImageFormat_t	m_dstImageFormat;
	ColorCorrectorParameters m_parameter;

	std::vector<FixMapPoint> m_frontMatchPoints;
	std::vector<FixMapPoint> m_backMatchPoints;

	bool m_bInitialized;

};

