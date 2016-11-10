#pragma once
#include "types.h"
//#define USE_GL3W
//#include <vermilion.h>
//#include "vapp.h"
#include "esutil.h"
#include <vector>
typedef struct VertexBuilderParam_t {
	int image_width;
	int image_height;
	int warp_step_x;
	int warp_step_y;

} VertexBuilderParam_t;
class VertexBuilder
{
public:
	VertexBuilder();
	~VertexBuilder();
	void SetSrcImageFormat(const ImageFormat_t& imageFormat);
	void SetDstImageFormat(const ImageFormat_t& imageFormat);
	void SetParameter(const VertexBuilderParam_t & param);

	bool BuildVertices();
	bool AddVertice();
	bool BuildOrders();

	const std::vector<GLfloat>& GetVertices();
	//const std::vector<GLint>&  GetElementCounts();
	//const std::vector<GLushort>& GetElementIndices();
	//std::vector<const GLvoid *>& GetElements();
	const std::vector<GLushort>& GetElements();
	int GetElementNum();

private:

	void CreateTestMaps();

	ImageFormat_t	m_srcImageFormat;
	ImageFormat_t	m_dstImageFormat;
	VertexBuilderParam_t m_parameter;

	std::vector<GLfloat> m_vertices;
	//std::vector<GLint> m_count;
	//std::vector<GLushort> m_indices;
	//std::vector<const GLvoid *> m_first;
	std::vector<GLushort> m_orders;
	int m_elementCount;
	bool m_bInitialized;

};

