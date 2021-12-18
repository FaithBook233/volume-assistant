#pragma once
class VolumeManager
{
public:
	VolumeManager();
	static void SetSystemVolume(const float NewVolume);
	static float GetSystemVolume();
	static void MuteTheSystem(bool NewSet);
private:
	float SystemmVolume;
};

