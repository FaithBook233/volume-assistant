#pragma once
class VolumeManager
{
public:
	VolumeManager();

	/*
	* @brief ����ϵͳ����
	* @Param Ҫ���õ�����
	* -1 ����
	* -2 �ָ�����
	* @return �Ƿ�����ɹ�
	*/
	bool SetSystemVolume(const int NewVolume);
	
	/*
	* @brief ��ȡ��ǰϵͳ����
	*/
	int GetSystemVolume();

	/**
	 * @brief ����ϵͳ����
	 */
	void MuteTheSystem(bool NewSet);
private:
	int SystemmVolume;
};

