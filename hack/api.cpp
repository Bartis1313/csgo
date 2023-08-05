#include "api.hpp"

#include <ShObjIdl_core.h>

std::filesystem::path api::getDocumentsPath()
{
	if (static CHAR documents[MAX_PATH]; SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents)))
		return std::filesystem::path{ documents };
	
	messageBox("Couldn't find windows documents path", "Documents Error");
	return {};
}

std::filesystem::path api::getHackPath()
{
	std::filesystem::path toReturn;
	toReturn.assign(getDocumentsPath() / localization.path);
	return toReturn;
}

std::string api::getDefaultConfigName()
{
	return localization.defaultConfigName;
}

std::filesystem::path api::getExtraLoadPath()
{
	return localization.utilityPath;
}