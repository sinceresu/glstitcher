#include "stdafx.h"
#include "MaskMaker.h"


MaskMaker::MaskMaker() :
	m_bInitialized(false)
{
}


MaskMaker::~MaskMaker() 
{
}

void MaskMaker::SetDstImageFormat(const ImageFormat_t & imageFormat)
{
	m_dstImageFormat = imageFormat;
	m_bInitialized = false;
}

void MaskMaker::SetParameter(const MaskMakerParameters & param)
{
	m_parameter = param;
	m_bInitialized = false;
}
static const int MIX_FRACTION = 2;
bool MaskMaker::BuildMasks()
{
	int maps_width = m_parameter.image_width / m_parameter.warp_step_x + 1;
	int maps_height = m_parameter.image_height / m_parameter.warp_step_y + 1;

	m_frontMask.resize(maps_width * maps_height);
	m_backMask.resize(maps_width * maps_height);


	int seam_left = m_parameter.first_region.left / m_parameter.warp_step_x;
	int seam_right = (m_parameter.second_region[0].left + m_parameter.second_region[0].width) / m_parameter.warp_step_x;
	int seam_width = seam_right - seam_left;
	int seam_middle0 = (seam_right + seam_left) / 2;
	int mix_width0 = seam_width / MIX_FRACTION;

	int mix_left0 = seam_middle0 - mix_width0 / 2;
	int mix_right0 = seam_middle0 + mix_width0 / 2;

	seam_left = m_parameter.second_region[1].left / m_parameter.warp_step_x;
	seam_right = (m_parameter.first_region.left + m_parameter.first_region.width) / m_parameter.warp_step_x;
	seam_width = seam_right - seam_left;
	int seam_middle1 = (seam_right + seam_left) / 2;
	int mix_width1 = seam_width / MIX_FRACTION;

	int mix_left1 = seam_middle1 - mix_width1/ 2;
	int mix_right1 = seam_middle1 + mix_width1 / 2;

	for (int i = 0; i < maps_height; i++) {
		for (int j = 0; j < mix_left0; j++) {
			m_frontMask[i * maps_width + j] = 0.0;
			m_backMask[i * maps_width + j] = 1.0;
		}
		for (int j = mix_left0; j < mix_right0; j++) {
			GLfloat mix_factor = 1.0 * (j - mix_left0) / mix_width0;
			m_frontMask[i * maps_width + j] = mix_factor;
			m_backMask[i * maps_width + j] = 1.0 - mix_factor;
		}

		for (int j = mix_right0; j < mix_left1; j++) {
			m_frontMask[i * maps_width + j] = 1.0;
			m_backMask[i * maps_width + j] = 0.0;
		}

		for (int j = mix_left1; j < mix_right1; j++) {
			GLfloat mix_factor = 1.0 * (j - mix_left1) / mix_width1;
			m_frontMask[i * maps_width + j] = 1.0 - mix_factor;
			m_backMask[i * maps_width + j] = mix_factor;
		}
		for (int j = mix_right1; j < maps_width; j++) {
			m_frontMask[i * maps_width + j] = 0.0;
			m_backMask[i * maps_width + j] = 1.0;
		}

	}



	return true;
}

const std::vector<GLfloat>& MaskMaker::GetFrontMask()
{
	// TODO: 在此处插入 return 语句
	return m_frontMask;
}

const std::vector<GLfloat>& MaskMaker::GetBackMask()
{
	// TODO: 在此处插入 return 语句
	return m_backMask;
}