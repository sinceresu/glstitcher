#include "stdafx.h"
#include "ParamReader.h"
#include "tinyxml2.h"

using namespace tinyxml2;

int ParamReader::ReadParameters(const char *szParams, FishEyeStitcherParam_t& param)
{
	XMLDocument m_cDocument;
	bool loadOkay = m_cDocument.LoadFile(szParams);

	//m_cDocument.Parse(szParams, NULL, TIXML_ENCODING_UTF8);
	const XMLElement* pRoot = m_cDocument.RootElement();
	if (NULL == pRoot)
		return -1;
	{
		const XMLElement * pChild = pRoot->FirstChildElement("TargetImageSize");
		if (NULL == pChild)
			return -1;
		{
			param.target_width = pChild->IntAttribute("width");
			param.target_height = pChild->IntAttribute("height");
			//	m_previewSetting.interval = atoi(interval);
		}

		pChild = pRoot->FirstChildElement("TableStep");
		if (NULL == pChild)
			return -1;
		{
			param.warp_step_x = pChild->IntAttribute("step_x");
			param.warp_step_y = pChild->IntAttribute("step_y");
			//	m_previewSetting.interval = atoi(interval);
		}

		pChild = pRoot->FirstChildElement("Cameras");
		if (NULL == pChild)
			return -1;
		{
			return ReadCameraParameters(pChild, param);
		}
	}
	return 0;
}

int ParamReader::ReadCameraParameters(const XMLElement * pRoot, FishEyeStitcherParam_t& param)
{
	const XMLElement * pSetting = pRoot->FirstChildElement("Camera");
	if (NULL == pSetting)
		return -1;
	{
		const XMLElement * pRoiSettings = pSetting->FirstChildElement("ROIS");
		if (NULL == pRoiSettings)
			return -1;
		const XMLElement * pRoiSetting = pRoiSettings->FirstChildElement("ROI");
		if (NULL == pRoiSetting)
			return -1;
		{
			param.first_region_left = pRoiSetting->IntAttribute("left");
			param.first_region_width = pRoiSetting->IntAttribute("width");
			const XMLElement * pTableSizeSetting = pRoiSetting->FirstChildElement("TableSize");
			param.first_table_width = pTableSizeSetting->IntAttribute("width");
			param.first_table_height = pTableSizeSetting->IntAttribute("height");
		}
	}
	pSetting = pSetting->NextSiblingElement("Camera");
	if (NULL == pSetting)
		return -1;
	{
		const XMLElement * pRoiSettings = pSetting->FirstChildElement("ROIS");
		if (NULL == pRoiSettings)
			return -1;
		const XMLElement * pRoiSetting = pRoiSettings->FirstChildElement("ROI");
		if (NULL == pRoiSetting)
			return -1;

		param.second_region_left1 = pRoiSetting->IntAttribute("left");
		param.second_region_width1 = pRoiSetting->IntAttribute("width");
		const XMLElement * pTableSizeSetting = pRoiSetting->FirstChildElement("TableSize");
		param.second_table_width1 = pTableSizeSetting->IntAttribute("width");
		param.second_table_height1 = pTableSizeSetting->IntAttribute("height");

		pRoiSetting = pRoiSetting->NextSiblingElement("ROI");
		if (NULL == pRoiSetting)
			return -1;
		param.second_region_left2 = pRoiSetting->IntAttribute("left");
		param.second_region_width2 = pRoiSetting->IntAttribute("width");
		pTableSizeSetting = pRoiSetting->FirstChildElement("TableSize");
		param.second_table_width2 = pTableSizeSetting->IntAttribute("width");
		param.second_table_height2 = pTableSizeSetting->IntAttribute("height");


	}
}

