#pragma once
class VolumeManager
{
public:
	VolumeManager();
	bool SetSystemVolume(const int NewVolume);
	int GetSystemVolume();
	void MuteTheSystem(bool NewSet);
private:
	int SystemmVolume;
};

