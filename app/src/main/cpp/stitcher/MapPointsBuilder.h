#pragma once
#include "types.h"
#include <vector>
#include <string>

typedef struct MapPointsBuilderParam_t {
	int image_width;
	int image_height;
	int warp_step_x;
	int warp_step_y;
	//int image_start;
	int		roi_num;
	TabledImageROI_t rois[2];

} MapPointsBuilderParam_t;

class MapPointsBuilder
{
public:
	MapPointsBuilder();
	~MapPointsBuilder();
	void SetSrcImageFormat(const ImageFormat_t& imageFormat);
	void SetDstImageFormat(const ImageFormat_t& imageFormat);
	void SetParameter(const MapPointsBuilderParam_t & param);
	int SetMapFile(const char* szMapFile);

	bool BuildMatchPoints();

	const std::vector<AdjustFloatMapPoint*>& GetMapPoints();
private:
	bool ReadProjectMaps();

	ImageFormat_t	m_srcImageFormat;
	ImageFormat_t	m_dstImageFormat;
	MapPointsBuilderParam_t m_parameter;
	std::string m_strMapFile;
	std::vector<AdjustFloatMapPoint> m_pSparseMap;
	std::vector<AdjustFloatMapPoint*> m_pSparseMapPtrs;
	bool m_bInitialized;
};

