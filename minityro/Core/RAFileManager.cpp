//
//  RAiOSBundleDocs.cpp
//  Ossa
//
//  Created by Rinat Abdrashitov on 2015-01-17.
//  Copyright (c) 2015 Rinat Abdrashitov. All rights reserved.
//

#include "RAFileManager.h"
#include "RALogManager.h"

namespace tyro
{
    std::string GetFilePath(const std::string& fileName, const std::string& extension)
    {	
		//@TODO: Remove full path
	    std::string shaderFolder = std::string(MY_RESOURCE_FILE) + std::string("/minityro/Shaders/");
		// std::string shaderFolder="/Users/rinat/Workspace/MuscleGeometrySrc/src/libigl-muscles/minityro/Shaders/";
		std::string fullPath = shaderFolder + fileName + std::string(".") + extension;
		//RA_LOG_INFO(fullPath.data());
		return fullPath;
	}

	std::string GetFilePath(const std::string& extrapath)
    {	
		std::string fullPath = std::string(MY_RESOURCE_FILE) + extrapath;
		return fullPath;
	}

}
