#pragma once
#include "types.h"
//#define USE_GL3W
//#include <vermilion.h>
//#include "vapp.h"
#include "esutil.h"

#include <vector>

typedef struct TexMapBuilderParam_t {
	int image_width;
	int image_height;
	int warp_step_x;
	int warp_step_y;
	//int image_start;
	int		roi_num;
	TabledImageROI_t rois[2];

} TexMapBuilderParam_t;
class TexMapBuilder
{
public:
	TexMapBuilder();
	~TexMapBuilder();
	void SetSrcImageFormat(const ImageFormat_t& imageFormat);
	void SetDstImageFormat(const ImageFormat_t& imageFormat);
	void SetParameter(const TexMapBuilderParam_t & param);

	bool BuildTexMap(const std::vector<GLfloat> &mask, std::vector<AdjustFloatMapPoint*> pSparseMaps);

	const std::vector<GLfloat>& GetTexCoords();
private:
	bool AddMapCoords(TabledImageROI_t roi, AdjustFloatMapPoint* pMaps);

	void CreateTestMaps();

	ImageFormat_t	m_srcImageFormat;
	ImageFormat_t	m_dstImageFormat;
	TexMapBuilderParam_t m_parameter;

	std::vector<GLfloat> m_texcoords;
	bool m_bInitialized;

};

