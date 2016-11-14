#pragma once
#include "types.h"
#include "ImageStitcher.h"
//#define USE_GL3W
//#include <vermilion.h>
//#include "vapp.h"
#include "esutil.h"

#include <vector>
#include <memory>

class ParamReader;

class VertexBuilder;
class TexMapBuilder;
class MapPointsBuilder;
class MaskMaker;
class ColorCorrector;

class GLStitcher: public ImageStitcher
{
public:
	GLStitcher();
	~GLStitcher();

	virtual int StitchImage(VideoFrame_t *pSrcImgs, VideoFrame_t *pDstImg);
	int SetWorkDirectory(const char* szDir);

private:
	bool Initialize();
	void InitGlut(const char * title = 0);
	bool InitMembers();
	void InitShader();
	void InitGLModel();
	void InitTexture();
	void Release();
	void InitDrawOrder();
	int InitParams();

	GLuint       m_hVertShader;  // Vertex shader handle
	GLuint       m_hFragShader;  // Fragment shader handle
	GLuint       m_hTexture;  // Texture handles


	GLuint       m_hFBO;      // Handles for Frame Buffer Objects
	unsigned int m_FrameCount;
	unsigned int m_FramesPerSecond;
	unsigned int m_CurrentFBO;

	// Shader programs and their uniform locations
	GLuint base_prog;
	GLuint vao;
	GLint  adjust_loc;

	GLuint quad_vbo;
	GLuint m_elementBuffer;

	GLuint front_tex;
	GLuint back_tex;


	std::vector<GLfloat> m_vertices;
	std::vector<GLfloat> m_texcoords;
	std::vector<GLfloat> m_indices;
	int m_nMapTableWidth;
	int m_nMapTableHeight;
	std::vector<GLint> count;
	std::vector<GLushort> indices;
	std::vector<const GLvoid *> first;
	int m_elementCount;

	std::shared_ptr<ImageParameters> m_pImageParameter;
	std::shared_ptr<VertexBuilder> m_pVertexBuilder;
	std::shared_ptr<MapPointsBuilder> m_pFrontMapPointsBuilder;
	std::shared_ptr<MapPointsBuilder> m_pBackMapPointsBuilder;
	std::shared_ptr<MaskMaker> m_pMaskMaker;
	std::shared_ptr<TexMapBuilder> m_pFrontTexMapBuilder;
	std::shared_ptr<TexMapBuilder> m_pBackTexMapBuilder;
	std::shared_ptr<ColorCorrector> m_pColorCorrector;
	VideoFrame_t * m_pSrcImgs;
	ESContext esContext;
	std::string m_strWorkDirectory;


};

