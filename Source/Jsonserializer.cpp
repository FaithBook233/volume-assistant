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
		//��ȡʧ����ֱ�Ӵ�����
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
