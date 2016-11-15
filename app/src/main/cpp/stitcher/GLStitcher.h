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
class MemTransfer;

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

	int InitParams();

	GLuint       m_hTexture;  // Texture handles


	GLuint       m_hFBO;      // Handles for Frame Buffer Objects

	// Shader programs and their uniform locations
	GLuint base_prog;
	GLuint vao;
	GLint  adjust_loc;

	GLuint quad_vbo;
	GLuint m_elementBuffer;

	GLuint front_tex;
	GLuint back_tex;


	std::vector<const GLvoid *> first;

	std::shared_ptr<ImageParameters> m_pImageParameter;
	std::shared_ptr<VertexBuilder> m_pVertexBuilder;
	std::shared_ptr<MapPointsBuilder> m_pFrontMapPointsBuilder;
	std::shared_ptr<MapPointsBuilder> m_pBackMapPointsBuilder;
	std::shared_ptr<MaskMaker> m_pMaskMaker;
	std::shared_ptr<TexMapBuilder> m_pFrontTexMapBuilder;
	std::shared_ptr<TexMapBuilder> m_pBackTexMapBuilder;
	std::shared_ptr<ColorCorrector> m_pColorCorrector;
	std::shared_ptr<MemTransfer> m_pMemTransfer;


	VideoFrame_t * m_pSrcImgs;
	ESContext esContext;
	std::string m_strWorkDirectory;


};

