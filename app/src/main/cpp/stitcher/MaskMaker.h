#pragma once
#include "types.h"
//#define USE_GL3W
//#include <vermilion.h>
//#include "vapp.h"
#include "esutil.h"

#include <vector>

typedef struct MaskMakerParameters
{

	int image_width;
	int image_height;
	int warp_step_x;
	int warp_step_y;

	TabledImageROI_t first_region;
	TabledImageROI_t second_region[2];
} MaskMakerParameters;

class MaskMaker
{
public:
	MaskMaker();
	~MaskMaker();
	void SetDstImageFormat(const ImageFormat_t& imageFormat);
	void SetParameter(const MaskMakerParameters & param);

	bool BuildMasks();

	const std::vector<GLfloat>& GetFrontMask();
	const std::vector<GLfloat>& GetBackMask();

private:

	ImageFormat_t	m_dstImageFormat;
	MaskMakerParameters m_parameter;
	std::vector<GLfloat> m_frontMask;
	std::vector<GLfloat> m_backMask;
	bool m_bInitialized;

};

