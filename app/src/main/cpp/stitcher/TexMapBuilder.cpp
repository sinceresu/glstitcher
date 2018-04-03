#include "stdafx.h"
#include "TexMapBuilder.h"
#include <assert.h>

TexMapBuilder::TexMapBuilder() :
	m_bInitialized(false)
{
}


TexMapBuilder::~TexMapBuilder()
{
}

void TexMapBuilder::SetSrcImageFormat(const ImageFormat_t & imageFormat)
{
	m_srcImageFormat = imageFormat;
	m_bInitialized = false;

}

void TexMapBuilder::SetDstImageFormat(const ImageFormat_t & imageFormat)
{
	m_dstImageFormat = imageFormat;
	m_bInitialized = false;
}

void TexMapBuilder::SetParameter(const TexMapBuilderParam_t & param)
{
	m_parameter = param;
	m_bInitialized = false;
}

bool TexMapBuilder::BuildTexMap(const std::vector<GLfloat> &mask, std::vector<AdjustFloatMapPoint*> pSparseMaps)
{
	//CreateTestMaps();

	int maps_width = m_parameter.image_width / m_parameter.warp_step_x + 1;
	int maps_height = m_parameter.image_height / m_parameter.warp_step_y + 1;
	m_texcoords.resize(4 * maps_width * maps_height);
	//初始化为边界外。
	for (int i = 0; i < maps_width * maps_height; i++) {
		m_texcoords[4 * i] = -2.0;
		m_texcoords[4 * i + 1] = -2.0;
		m_texcoords[4 * i + 2] = 1.0;
		m_texcoords[4 * i + 3] = mask[i];
	}
	for (int i = 0; i < m_parameter.roi_num; i++) {
		AddMapCoords(m_parameter.rois[i], pSparseMaps[i]);
		//AddOrders(m_parameter.rois[i], table_offset);
		//table_offset += m_parameter.rois[i].table_width * m_parameter.rois[i].table_height;

	}
	return true;
}

const std::vector<GLfloat>& TexMapBuilder::GetTexCoords()
{
	// TODO: 在此处插入 return 语句
	return m_texcoords;
}

bool TexMapBuilder::AddMapCoords(TabledImageROI_t roi, AdjustFloatMapPoint* pMaps)
{
	int maps_width = m_parameter.image_width / m_parameter.warp_step_x + 1;
	int maps_height = m_parameter.image_height / m_parameter.warp_step_y + 1;
	int	squares_width = m_parameter.image_width / m_parameter.warp_step_x;
	int squares_height = m_parameter.image_height / m_parameter.warp_step_y;


	//int start_map_x = roi.left / m_parameter.warp_step_x;
	//int start_map_y = roi.top / m_parameter.warp_step_y;

	//int start_map_x = 0;
	//int start_map_y = 0;

	//GLfloat* texcoords = &m_texcoords[3 * (start_map_y * maps_width + start_map_x)];

	//for (int i = 0; i < maps_height; i++) {
	//	for (int j = 0; j < maps_width; j++) {
	//		texcoords[3 * (i * maps_width + j)] = float(start_map_x + j) / squares_width;
	//		texcoords[3 * (i * maps_width + j) + 1] = 1.0 - float(start_map_y + i) / squares_height;
	//		texcoords[3 * (i * maps_width + j) + 2] = 1.0;
	//	}
	//}

	int start_map_x = roi.left / m_parameter.warp_step_x;
	int start_map_y = roi.top / m_parameter.warp_step_y;

	GLfloat* texcoords = &m_texcoords[4 * (start_map_y * maps_width + start_map_x)];

	for (int i = 0; i < roi.table_height; i++) {
		for (int j = 0; j < roi.table_width; j++) {
#if 1
			GLfloat texcood_x = pMaps[i*roi.table_width + j].x;
			GLfloat texcood_y = pMaps[i*roi.table_width + j].y;
			GLfloat lum_coeff = pMaps[i*roi.table_width + j].l;
#else
			GLfloat texcood_x = (float)(start_map_x + j) / squares_width;
			GLfloat texcood_y = (float)(start_map_y + i) / squares_height;
			GLfloat lum_coeff = 1.0;
#endif
			assert(texcood_x >= 0.0 && texcood_x <= 1.0);
			assert(texcood_y >= 0.0 && texcood_y <= 1.0);

			texcoords[4 * (i * maps_width + j)] = texcood_x;
			texcoords[4 * (i * maps_width + j) + 1] = texcood_y;

			//texcoords[3 * (i * maps_width + j) + 2] = 1.0;
			texcoords[4 * (i * maps_width + j) + 2] = lum_coeff;
		}
	}
	return true;
}



void TexMapBuilder::CreateTestMaps()
{
	m_parameter.roi_num = 2;
	m_parameter.warp_step_x = m_parameter.image_width / 4;
	m_parameter.warp_step_y = m_parameter.image_height;

	m_parameter.rois[0].left = 0;
	m_parameter.rois[0].top = 0;
	m_parameter.rois[0].width = m_parameter.image_width / 4;
	m_parameter.rois[0].height = m_parameter.image_height;
	m_parameter.rois[0].table_width = 2;
	m_parameter.rois[0].table_height = 2;

	m_parameter.rois[1].left = m_parameter.image_width * 3 / 4;
	m_parameter.rois[1].top = 0;
	m_parameter.rois[1].width = m_parameter.image_width / 4;
	m_parameter.rois[1].height = m_parameter.image_height;
	m_parameter.rois[1].table_width = 2;
	m_parameter.rois[1].table_height = 2;

	m_parameter.warp_step_x = m_parameter.rois[0].width / (m_parameter.rois[0].table_width - 1);
	m_parameter.warp_step_y = m_parameter.rois[0].height / (m_parameter.rois[0].table_height - 1);

	int table_size = 0;
	for (int i = 0; i < m_parameter.roi_num; i++) {
		table_size += m_parameter.rois[i].table_width * m_parameter.rois[i].table_height;
	}

}
