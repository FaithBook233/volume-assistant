#pragma once
class VolumeManager
{
public:
	VolumeManager();

	/*
	* @brief 设置系统音量
	* @Param 要设置的音量
	* -1 静音
	* -2 恢复静音
	* @return 是否操作成功
	*/
	bool SetSystemVolume(const int NewVolume);
	
	/*
	* @brief 获取当前系统音量
	*/
	int GetSystemVolume();

	/**
	 * @brief 设置系统静音
	 */
	void MuteTheSystem(bool NewSet);
private:
	int SystemmVolume;
};

