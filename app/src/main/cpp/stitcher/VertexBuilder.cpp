#include "stdafx.h"
#include "VertexBuilder.h"


VertexBuilder::VertexBuilder() :
	m_bInitialized(false)
{
}


VertexBuilder::~VertexBuilder()
{
}

void VertexBuilder::SetSrcImageFormat(const ImageFormat_t & imageFormat)
{
	m_srcImageFormat = imageFormat;
	m_bInitialized = false;

}

void VertexBuilder::SetDstImageFormat(const ImageFormat_t & imageFormat)
{
	m_dstImageFormat = imageFormat;
	m_bInitialized = false;
}

void VertexBuilder::SetParameter(const VertexBuilderParam_t & param)
{
	m_parameter = param;
	m_bInitialized = false;
}


bool VertexBuilder::BuildVertices()
{
	//CreateTestMaps();


	AddVertice();

	BuildOrders();

	//int offset = m_indices.size() / m_elementCount;
	//m_first.resize(m_elementCount);
	//for (int i = 00; i < m_elementCount; i++) {
	//	m_first[i] = &m_indices[i*offset];
	//}



	//int offset = m_parameter.rois[0].table_width * m_parameter.rois[0].table_height;
	//AddVertice(m_parameter.rois[1], m_pSparseMap.data() + offset);
	//AddOrders(m_parameter.rois[1], 0);

	return true;
}

bool VertexBuilder::AddVertice()
{
	//平均分割行(列),行（列）数需要在块数上加1.
	int maps_width = m_parameter.image_width / m_parameter.warp_step_x + 1;
	int maps_height = m_parameter.image_height / m_parameter.warp_step_y + 1;
	int	squares_width = m_parameter.image_width / m_parameter.warp_step_x;
	int squares_height = m_parameter.image_height / m_parameter.warp_step_y;

	m_vertices.resize( 2* maps_width * maps_height);
	for (int i = 0; i < maps_height; i++) {
		for (int j = 0; j < maps_width; j++) {
			m_vertices[2 * (i * maps_width + j)] = j * 2.0 / squares_width - 1.0;
			m_vertices[2 * (i * maps_width + j) + 1] = 1.0 - i * 2.0 / squares_height;
		}
	}

	return true;
}



bool VertexBuilder::BuildOrders()
{
	//平均分割行(列),行（列）数需要在块数上加1.
	int maps_x = m_parameter.image_width / m_parameter.warp_step_x + 1;
	int maps_y = m_parameter.image_height / m_parameter.warp_step_y + 1;
	int	squares_width = m_parameter.image_width / m_parameter.warp_step_x;
	int squares_height = m_parameter.image_height / m_parameter.warp_step_y;

	int index_width = maps_x * 2;
	int index_height = maps_y - 1;
	int index_total_size = index_width*index_height;
	//m_elementCount = index_height;


	//m_indices.resize(index_total_size);
	//m_count.resize(m_elementCount);
	//m_first.resize(m_elementCount);
	m_orders.resize(index_total_size);
	m_elementCount = index_total_size;

	//for (int i = 0; i < index_height; i++) {
	//	m_count[i] = index_width;
	//	m_first[i] = &m_indices[i*index_width];
	//	for (int j = 0; j < maps_x; j++) {
	//		m_indices[i * index_width + j * 2] = maps_x * i + j;
	//		m_indices[i * index_width + j * 2 + 1] = maps_x * i + j + maps_x;
	//	}
	//};

	for (int i = 0; i < index_height; i+=2) {
		for (int j = 0; j < maps_x; j++) {
			m_orders[i * index_width + j * 2] = maps_x * i + j;
			m_orders[i * index_width + j * 2 + 1] = maps_x * i + j + maps_x;
		}
		for (int j = 0; j < maps_x; j++) {
			m_orders[(i + 1) * index_width + j * 2] = maps_x * (i + 1) + maps_x - 1 - j;
			m_orders[(i + 1) * index_width + j * 2 + 1] = maps_x * (i + 1) + maps_x - 1 - j + maps_x;
		}
	};


	return true;
}

const std::vector<GLfloat>& VertexBuilder::GetVertices()
{
	return m_vertices;
}
//
//
//const std::vector<GLint>& VertexBuilder::GetElementCounts()
//{
//	return m_count;
//}
//
//const std::vector<GLushort>& VertexBuilder::GetElementIndices()
//{
//	return m_indices;
//}
//
//std::vector<const GLvoid *>& VertexBuilder::GetElements()
//{
//	return m_first;
//}

const std::vector<GLushort>& VertexBuilder::GetElements()
{
	// TODO: 在此处插入 return 语句
	return m_orders;
}



int VertexBuilder::GetElementNum()
{
	return m_elementCount;
}

void VertexBuilder::CreateTestMaps()
{
	m_parameter.warp_step_x = m_parameter.image_width / 4;
	m_parameter.warp_step_y = m_parameter.image_height;
}
