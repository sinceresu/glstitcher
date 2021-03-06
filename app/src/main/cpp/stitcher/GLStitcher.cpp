#include "stdafx.h"

#include "GLStitcher.h"
#include <string>
#include "ParamReader.h"
#include "VertexBuilder.h"
#include "MapPointsBuilder.h"
#include "MaskMaker.h"
#include "TexMapBuilder.h"
#include "ColorCorrector.h"
#include "MemTransfer.h"
#include "MemTransferFactory.h"

#include "../platform/android/egl.h"

//#define TEST_DATA
using namespace ogles_gpgpu;
#define BUFFER_OFFSET(x)  ((const void*) (x))


GLStitcher::GLStitcher() :
	m_pImageParameter(std::make_shared<ImageParameters>()),
	m_pVertexBuilder(std::make_shared<VertexBuilder>()),
	m_pFrontMapPointsBuilder(std::make_shared<MapPointsBuilder>()),
	m_pBackMapPointsBuilder(std::make_shared<MapPointsBuilder>()),
	m_pMaskMaker(std::make_shared<MaskMaker>()),
	m_pColorCorrector(std::make_shared<ColorCorrector>()),
	m_pFrontTexMapBuilder(std::make_shared<TexMapBuilder>()),
	m_pBackTexMapBuilder(std::make_shared<TexMapBuilder>()),
    m_strWorkDirectory(".")

{
	m_pMemTransfer = MemTransferFactory::createInstance();
}


GLStitcher::~GLStitcher()
{
	Release();
}

int GLStitcher::SetWorkDirectory(const char* szDir)
{
	m_strWorkDirectory = szDir;
    return 0;
}




int GLStitcher::StitchImage(VideoFrame_t * pSrcImgs, VideoFrame_t * pDstImg)
{
	if (!m_bInitialized) {
		Initialize();
	}
	//WinLoop(&esContext);
	//return 0;
	//for (;;) 
	//	glutMainLoopEvent();    
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hFBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		float adjust_coeff = m_pColorCorrector->CalcAdjustCoeff(&pSrcImgs[0], &pSrcImgs[1]);
		glClear(GL_COLOR_BUFFER_BIT);
//		glDisable(GL_CULL_FACE);
		glUseProgram(base_prog);

		glUniform1f(adjust_loc, adjust_coeff);

//		glBindVertexArray(vao);

		m_pMemTransfer->toGPU(&pSrcImgs[0], &pSrcImgs[1]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
		glDrawElements(GL_TRIANGLE_STRIP, m_pVertexBuilder->GetElementNum(), GL_UNSIGNED_SHORT, NULL);

		m_pMemTransfer->fromGPU(pDstImg);

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return 0;
}

void GLStitcher::InitGlut(const char * title)
{
		// glutInitContextProfile(GLUT_CORE_PROFILE);
		// glutInitContextVersion(4, 3);

		memset(&esContext, 0, sizeof(ESContext));

//		esCreateWindow(&esContext, "Simple Texture 2D", 320, 160, ES_WINDOW_RGB);
		EGL::setup();
		EGL::createPBufferSurface(m_dstImageFormat.frame_width, m_dstImageFormat.frame_height);
		EGL::activate();

		esContext.userData = this;
}

const static char *szFrontCameraMapFile = "/maps/map_front.dat";
const static char *szBehindCameraMapFile = "/maps/map_behind.dat";

bool GLStitcher::InitMembers()
{
	m_pVertexBuilder->SetSrcImageFormat(m_srcImageFormat);
	m_pVertexBuilder->SetDstImageFormat(m_dstImageFormat);
	VertexBuilderParam_t full_map_param = {
		m_pImageParameter->mapImageWidth , m_pImageParameter->mapImageHeight,
		m_pImageParameter->proStep_X , m_pImageParameter->proStep_Y,
	};
	m_pVertexBuilder->SetParameter(full_map_param);

	//m_pVertexBuilder->SetMapFile(szFrontCameraMapFile);
	m_pVertexBuilder->BuildVertices();

	m_pFrontTexMapBuilder->SetSrcImageFormat(m_srcImageFormat);
	m_pFrontTexMapBuilder->SetDstImageFormat(m_dstImageFormat);
	MapPointsBuilderParam_t front_map_points_builder_param = {
		m_pImageParameter->mapImageWidth , m_pImageParameter->mapImageHeight,
		m_pImageParameter->proStep_X , m_pImageParameter->proStep_Y,
		1,
		{
			m_pImageParameter->first_region,
			NULL
		}
	};

	m_pFrontMapPointsBuilder->SetParameter(front_map_points_builder_param);

    std::string map_file_path = m_strWorkDirectory + std::string(szFrontCameraMapFile);
    m_pFrontMapPointsBuilder->SetMapFile(map_file_path.c_str());
	m_pFrontMapPointsBuilder->BuildMatchPoints();


	m_pBackMapPointsBuilder->SetSrcImageFormat(m_srcImageFormat);
	m_pBackMapPointsBuilder->SetDstImageFormat(m_dstImageFormat);

	MapPointsBuilderParam_t back_map_points_builder_param = {
		m_pImageParameter->mapImageWidth , m_pImageParameter->mapImageHeight,
		m_pImageParameter->proStep_X , m_pImageParameter->proStep_Y,
		2,
		{
			m_pImageParameter->second_region[0],
			m_pImageParameter->second_region[1]
		}
	};
	m_pBackMapPointsBuilder->SetParameter(back_map_points_builder_param);
    map_file_path = m_strWorkDirectory + std::string(szBehindCameraMapFile);
	m_pBackMapPointsBuilder->SetMapFile(map_file_path.c_str());
	m_pBackMapPointsBuilder->BuildMatchPoints();

	m_pMaskMaker->SetDstImageFormat(m_dstImageFormat);

	MaskMakerParameters mask_maker_param = {
		m_pImageParameter->mapImageWidth , m_pImageParameter->mapImageHeight,
		m_pImageParameter->proStep_X , m_pImageParameter->proStep_Y,
		m_pImageParameter->first_region,
		{
			m_pImageParameter->second_region[0],
			m_pImageParameter->second_region[1]
		}
	};

	m_pMaskMaker->SetParameter(mask_maker_param);
	m_pMaskMaker->BuildMasks();


	m_pColorCorrector->SetSrcImageFormat(m_srcImageFormat);
	m_pColorCorrector->SetDstImageFormat(m_dstImageFormat);

	ColorCorrectorParameters color_corrector_param = {
		m_pImageParameter->mapImageWidth , m_pImageParameter->mapImageHeight,
		m_pImageParameter->proStep_X , m_pImageParameter->proStep_Y,
		m_pImageParameter->first_region,
		{
			m_pImageParameter->second_region[0],
			m_pImageParameter->second_region[1]
		}
	};

	m_pColorCorrector->SetParameter(color_corrector_param);
	m_pColorCorrector->BuildMatchPoints(m_pFrontMapPointsBuilder->GetMapPoints(),
		m_pBackMapPointsBuilder->GetMapPoints());


	m_pFrontTexMapBuilder->SetSrcImageFormat(m_srcImageFormat);
	m_pFrontTexMapBuilder->SetDstImageFormat(m_dstImageFormat);
	TexMapBuilderParam_t front_map_param = {
		m_pImageParameter->mapImageWidth , m_pImageParameter->mapImageHeight,
		m_pImageParameter->proStep_X , m_pImageParameter->proStep_Y,
		1,
		{
			m_pImageParameter->first_region,
			NULL
		}
	};


	m_pFrontTexMapBuilder->SetParameter(front_map_param);
	m_pFrontTexMapBuilder->BuildTexMap(m_pMaskMaker->GetFrontMask(),
		m_pFrontMapPointsBuilder->GetMapPoints());


	m_pBackTexMapBuilder->SetSrcImageFormat(m_srcImageFormat);
	m_pBackTexMapBuilder->SetDstImageFormat(m_dstImageFormat);
	TexMapBuilderParam_t back_map_param = {
		m_pImageParameter->mapImageWidth , m_pImageParameter->mapImageHeight,
		m_pImageParameter->proStep_X , m_pImageParameter->proStep_Y,
		2,
		{
			m_pImageParameter->second_region[0],
			m_pImageParameter->second_region[1]
		}
	};
	m_pBackTexMapBuilder->SetParameter(back_map_param);
	m_pBackTexMapBuilder->BuildTexMap(m_pMaskMaker->GetBackMask(),
		m_pBackMapPointsBuilder->GetMapPoints());

	return true;
}

	void GLStitcher::InitShader()
	{
		char vShaderStr[] =
				"#version 300 es                            \n"
						"\n"
						"layout (location = 0) in vec2 in_position;\n"
						"layout (location = 1) in vec4 in_tex_coord1;\n"
						"layout (location = 2) in vec4 in_tex_coord2;\n"
						//"layout (location = 2) in float in_tex_coeff;\n"
						"\n"
						"out vec2 tex_coord1;\n"
						"out float tex_coeff1;\n"
						"out vec2 tex_coord2;\n"
						"out float tex_coeff2;\n"
						"out float tex_alpha;\n"
						"\n"
						"void main(void)\n"
						"{\n"
						"    gl_Position = vec4(in_position, 0.5, 1.0);\n"
						"    tex_coord1 = vec2(in_tex_coord1.x, in_tex_coord1.y);\n"
						"    tex_coeff1 = in_tex_coord1.z;\n"
						"    tex_coord2 = vec2(in_tex_coord2.x, in_tex_coord2.y);\n"
						"    tex_coeff2 = in_tex_coord2.z;\n"
						"    tex_alpha = in_tex_coord2.w;\n"
						"}\n"
		;

		char fShaderStr[] =
				"#version 300 es                                     \n"
						"precision mediump float;                            \n"
						"in vec2 tex_coord1;\n"
						"in float tex_coeff1;\n"
						"in vec2 tex_coord2;\n"
						"in float tex_coeff2;\n"
						"in float tex_alpha;\n"
						"\n"
						"layout (location = 0) out vec4 color;\n"
						"\n"
						"uniform sampler2D tex1;\n"
						"uniform sampler2D tex2;\n"
						"uniform float adjust;\n"
						"\n"
						"void main(void)\n"
						"{\n"
						"    vec4 output1 = texture(tex1, tex_coord1);\n"
						"\n"
						"    vec4 output2 = texture(tex2, tex_coord2);\n"
						"\n"
						"    color = mix(output1 * tex_coeff1 * adjust,	output2 * tex_coeff2,  tex_alpha);\n"
						"}\n"
		;



	//vglAttachShaderSource(base_prog, GL_VERTEX_SHADER, quad_shader_vs);
	//vglAttachShaderSource(base_prog, GL_FRAGMENT_SHADER, quad_shader_fs);
	base_prog = esLoadProgram(vShaderStr, fShaderStr);
	//glLinkProgram(base_prog);

	char buf[1024];
	glGetProgramInfoLog(base_prog, 1024, NULL, buf);

	glUseProgram(base_prog);
#ifdef GL_TEXTURE_EXTERNAL_OES
#endif
	adjust_loc = glGetUniformLocation(base_prog, "adjust");
	glUniform1i(glGetUniformLocation(base_prog, "tex1"), 0);
	glUniform1i(glGetUniformLocation(base_prog, "tex2"), 1);



}

void GLStitcher::InitGLModel()
{
	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

	const std::vector<GLfloat>& vertices = m_pVertexBuilder->GetVertices();
	const std::vector<GLfloat>& front_texcoords = m_pFrontTexMapBuilder->GetTexCoords();
	const std::vector<GLfloat>& back_texcoords = m_pBackTexMapBuilder->GetTexCoords();

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) +
		front_texcoords.size() * sizeof(GLfloat) +
		back_texcoords.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLfloat), vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), front_texcoords.size() * sizeof(GLfloat), front_texcoords.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) + front_texcoords.size() * sizeof(GLfloat),
		back_texcoords.size() * sizeof(GLfloat), back_texcoords.data());
//
//	glGenVertexArrays(1, &vao);
//	glBindVertexArray(vao);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(GLfloat)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(GLfloat) +
		front_texcoords.size() * sizeof(GLfloat)));


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &m_elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_pVertexBuilder->GetElementNum() * sizeof(GLushort),
		(const GLvoid*)m_pVertexBuilder->GetElements().data(), GL_STATIC_DRAW);


}


void GLStitcher::InitTexture()
{
	if (true != m_pMemTransfer->prepareInput(m_srcImageFormat.frame_width, m_srcImageFormat.frame_height))
		return;

	glGenFramebuffers(1, &m_hFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hFBO);

	if (true != m_pMemTransfer->prepareOutput(m_dstImageFormat.frame_width, m_dstImageFormat.frame_height))
		return;


}

bool GLStitcher::Initialize()
{
	InitParams();

	InitMembers();

	InitGlut("GLWarper test");

	InitShader();

	InitGLModel();

	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
	InitTexture();

	glClear(GL_COLOR_BUFFER_BIT);

//	InitDrawOrder();

	glViewport(0, 0, m_dstImageFormat.frame_width, m_dstImageFormat.frame_height);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);

	m_bInitialized = true;

	return false;
}

void GLStitcher::Release()
{
	glDeleteFramebuffers(1, &m_hFBO);

	// Delete the textures
//	glDeleteTextures(1, &m_hTexture);


	// Delete program and shader objects
	glUseProgram(0);
	glDeleteProgram(base_prog);
//	glDeleteTextures(1, &front_tex);
//	glDeleteTextures(1, &back_tex);
//	glDeleteVertexArrays(1, &vao);
	EGL::shutdown();
	m_bInitialized = false;
}


int GLStitcher::InitParams()
{
	m_pImageParameter->scale = 256;
	FishEyeStitcherParam_t config;
	//string configFile = m_stitchedVideoSize == VIDEOSIZE_3840X1920 ? "./config/config_1920p.ini" : "./config/config_960p.ini";
	std::string configFile = m_strWorkDirectory + std::string("config/stitch_param.xml");

	if (0 != ParamReader::ReadParameters(configFile.c_str(), config))
		return -1;

	//if (0 != ReadParameters(configFile.c_str(), config))
	//	return -1;

	m_pImageParameter->mapImageWidth = config.target_width;
	m_pImageParameter->mapImageHeight = config.target_height;

	/////////////////////////////////////////

	m_pImageParameter->proStep_X = config.warp_step_x;
	m_pImageParameter->proStep_Y = config.warp_step_y;

	m_pImageParameter->first_region = { config.first_region_left, 0, config.first_region_width, m_pImageParameter->mapImageHeight,
		config.first_table_width, config.first_table_height };
	m_pImageParameter->second_region[0] = { config.second_region_left1, 0, config.second_region_width1, m_pImageParameter->mapImageHeight,
		config.second_table_width1, config.second_table_height1 };
	m_pImageParameter->second_region[1] = { config.second_region_left2, 0, config.second_region_width2, m_pImageParameter->mapImageHeight,
		config.second_table_width2, config.second_table_height2 };


	return 0;
}
