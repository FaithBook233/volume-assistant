#pragma once

#include<iostream>
#include <mmdeviceapi.h> 
#include <string>
#include <map>


//定义空宏，处理finally
#ifndef CUSTOM_FINALLY
#define CUSTOM_FINALLY
#endif // CUSTOM_FINALLY

#define PNTLOG(format, ...) wprintf(format L"\n", __VA_ARGS__)
#define PNTERR(format, ...) PNTLOG(L"Error: " format, __VA_ARGS__)
#define PNTERRMSG(msg) wprintf(L"Error: %s \n", msg)

class DeviceManager
{

public:
	DeviceManager();

	/**
	 * 以下函数来源于CSDN：https://blog.csdn.net/pigautumn/article/details/94381624
	 */
	static bool GetDefaultDevice(IMMDevice** ppMMDevice);

	static bool GetDefaultDevice(std::wstring& deviceId, std::wstring& deviceName);

	static bool GetDevices(std::map<std::wstring, std::wstring>& deviceMap);

	static bool GetDevices(IMMDeviceCollection* pMMDeviceCollection, std::map<std::wstring, std::wstring>& deviceMap);

	static bool GetSpecificDevice(LPCWSTR pwszLongName, IMMDevice** ppMMDevice);

	static bool GetSpecificDevice(IMMDeviceCollection* pMMDeviceCollection, LPCWSTR pwszLongName, IMMDevice** ppMMDevice);
 
private:
	//用于记录当前设备列表
	std::map<std::wstring ,std::wstring> DeviceMap;
};