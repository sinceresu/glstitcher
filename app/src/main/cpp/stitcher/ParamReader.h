#pragma once
#include "types.h"
namespace tinyxml2 
{
	class XMLElement;
}

class ParamReader
{
public:
	static int ReadParameters(const char *filename, FishEyeStitcherParam_t& param);
	
private:
	static int ReadCameraParameters(const tinyxml2::XMLElement* pRoot, FishEyeStitcherParam_t& param);


};

