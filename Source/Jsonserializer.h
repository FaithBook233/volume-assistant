#pragma once
#include <iostream>

/*
* Json��������ݶ���
* 1.�豸��
* 2.�Ƿ���
* 3.��󱣴������
*/
class JsonSerializer
{
public:
	JsonSerializer();

	static bool GetDocumentPath(std::string& SystemDocumentPath);

	bool SaveVolumeOfDevice();
	int GetVolumeOfDevice();
private:
	std::string VolumeAssistantSavedFilePath;
};

