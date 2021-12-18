#pragma once
#include <iostream>

/*
* Json保存的数据对象
* 1.设备名
* 2.是否静音
* 3.最后保存的音量
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

