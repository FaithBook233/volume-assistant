#include "JsonSerializer.h"  

#include <windows.h>
#include <iostream>
#include <shlobj.h>
JsonSerializer::JsonSerializer()
{
	std::string FilePathToGet;
	if (GetDocumentPath(FilePathToGet))
	{
		VolumeAssistantSavedFilePath = FilePathToGet + std::string("/");
	}
	else
	{
		//获取失败则直接创建新
		VolumeAssistantSavedFilePath = std::string("D:/VolumeAssistant/");
	}
	
}

bool JsonSerializer::GetDocumentPath(std::string& SystemDocumentPath)
{
	CHAR MyDocumentsPath[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, MyDocumentsPath);
	SystemDocumentPath = std::string(MyDocumentsPath);
	return result;
}
