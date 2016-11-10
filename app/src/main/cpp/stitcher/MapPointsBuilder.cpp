#include "stdafx.h"
#include "MapPointsBuilder.h"


MapPointsBuilder::MapPointsBuilder() :
	m_bInitialized(false)
{
}


MapPointsBuilder::~MapPointsBuilder()
{
}

void MapPointsBuilder::SetSrcImageFormat(const ImageFormat_t & imageFormat)
{
	m_srcImageFormat = imageFormat;
	m_bInitialized = false;

}

void MapPointsBuilder::SetDstImageFormat(const ImageFormat_t & imageFormat)
{
	m_dstImageFormat = imageFormat;
	m_bInitialized = false;
}

void MapPointsBuilder::SetParameter(const MapPointsBuilderParam_t & param)
{
	m_parameter = param;
	m_bInitialized = false;
}

int MapPointsBuilder::SetMapFile(const char * szMapFile)
{
	m_strMapFile = szMapFile;
	m_bInitialized = false;
	return 0;
}

bool MapPointsBuilder::BuildMatchPoints()
{
	ReadProjectMaps();
	return true;
}

const std::vector<AdjustFloatMapPoint*>& MapPointsBuilder::GetMapPoints()
{
	// TODO: 在此处插入 return 语句
	return m_pSparseMapPtrs;


}	

bool MapPointsBuilder::ReadProjectMaps()
{
	////////////////////////////////
	int table_size = 0;
	for (int i = 0; i < m_parameter.roi_num; i++) {
		table_size += m_parameter.rois[i].table_width * m_parameter.rois[i].table_height;
	}
	//m_pProjection_sphereRotation.resize(m_pImageParameter->proImageW * m_pImageParameter->proImageH);
	std::vector<float> map_x(table_size);
	std::vector<float> map_y(table_size);
	std::vector<float> lum(table_size);

	FILE *fp = NULL;

	//fp = fopen("./maps/projectionTableFront_angle.dat", "r+b");
	fp = fopen(m_strMapFile.c_str(), "r+b");
	if (fp == NULL) {
		return false;
	}
	fread(map_x.data(), 1, sizeof(float)*table_size, fp);
	fread(map_y.data(), 1, sizeof(float)*table_size, fp);
	fread(lum.data(), 1, sizeof(float)*table_size, fp);

	fclose(fp);
	fp = NULL;

	m_pSparseMap.resize(table_size);
	for (int i = 0; i < table_size; i++) {
		m_pSparseMap[i].x = map_x[i];
		m_pSparseMap[i].y = map_y[i];
		m_pSparseMap[i].l = lum[i];
	}

	int offset = 0;
	m_pSparseMapPtrs.resize(2);
	for (int i = 0; i < m_parameter.roi_num; i++) {
		m_pSparseMapPtrs[i] = &m_pSparseMap[offset];
		offset += m_parameter.rois[i].table_width * m_parameter.rois[i].table_height;
	}

	//note: for test
	//m_parameter.roi_num = 1;
	return true;
}


