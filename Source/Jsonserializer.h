#pragma once
#include <iostream>
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

