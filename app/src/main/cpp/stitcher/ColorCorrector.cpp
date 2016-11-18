#include "stdafx.h"
#include "ColorCorrector.h"


ColorCorrector::ColorCorrector() :
	m_bInitialized(false)
{
}


ColorCorrector::~ColorCorrector()
{
}

void ColorCorrector::SetSrcImageFormat(const ImageFormat_t & imageFormat)
{
	m_srcImageFormat = imageFormat;
	m_bInitialized = false;

}

void ColorCorrector::SetDstImageFormat(const ImageFormat_t & imageFormat)
{
	m_dstImageFormat = imageFormat;
	m_bInitialized = false;
}

void ColorCorrector::SetParameter(const ColorCorrectorParameters & param)
{
	m_parameter = param;
	m_bInitialized = false;
}

bool ColorCorrector::BuildMatchPoints(std::vector<AdjustFloatMapPoint*> pFrontMaps,
	std::vector<AdjustFloatMapPoint*> pBackMaps)
{
	int maps_width = m_parameter.image_width / m_parameter.warp_step_x + 1;
	int maps_height = m_parameter.image_height / m_parameter.warp_step_y + 1;

	m_frontMatchPoints.resize(maps_height);
	m_backMatchPoints.resize(maps_height);


	int first_left = m_parameter.first_region.left / m_parameter.warp_step_x;
	int second_right = (m_parameter.second_region[0].left + m_parameter.second_region[0].width) / m_parameter.warp_step_x;
	int seam_middle = (second_right + first_left) / 2;

	int second_left = m_parameter.second_region[0].left / m_parameter.warp_step_x;
	int first_offset = seam_middle - first_left;
	int second_offset = seam_middle - second_left;

	for (int i = 0; i < maps_height; i++) {
		AdjustFloatMapPoint* pFrontMap = &pFrontMaps[0][i * m_parameter.first_region.table_width + first_offset];
		m_frontMatchPoints[i] = { (int)(pFrontMap->x * m_srcImageFormat.frame_width + 0.5),
			(int)(pFrontMap->y * m_srcImageFormat.frame_height + 0.5) };
		AdjustFloatMapPoint* pBackMap = &pBackMaps[0][i * m_parameter.second_region[0].table_width + second_offset];
		m_backMatchPoints[i] = { (int)(pBackMap->x * m_srcImageFormat.frame_width + 0.5),
			(int)(pBackMap->y * m_srcImageFormat.frame_height + 0.5) };

	}



	return true;
}

float ColorCorrector::CalcAdjustCoeff(const VideoFrame_t * pFrontImg, const VideoFrame_t * pBackImg)
{
	int maps_height = m_parameter.image_height / m_parameter.warp_step_y + 1;
	int64_t all_Tamount = 0;
	int64_t all_Bamount = 0;

	//Y channels
	for (int i = 0; i < maps_height; i++) {
		int coord_x_front = m_frontMatchPoints[i].x;
		int coord_y_front = m_frontMatchPoints[i].y;
		int coord_x_back = m_backMatchPoints[i].x;
		int coord_y_back = m_backMatchPoints[i].y;

		uint8_t * ptr_pixel_front = &pFrontImg->planes[0][coord_y_front * pFrontImg->strides[0] + 4 * coord_x_front];
		uint8_t * ptr_pixel_back = &pBackImg->planes[0][coord_y_back * pBackImg->strides[0] + 4 * coord_x_back];

		all_Tamount += ptr_pixel_front[0] + ptr_pixel_front[1] + ptr_pixel_front[2];
		all_Bamount += ptr_pixel_back[0] + ptr_pixel_back[1] + ptr_pixel_back[2];

	}
	float adjustCoeff = (all_Tamount == 0) ? 1.0 : (float)(all_Bamount) / all_Tamount;

	return adjustCoeff;
}

