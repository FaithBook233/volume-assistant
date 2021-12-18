#include "DeviceManager.h"
#include <iostream>
#include <windows.h> 
#include <endpointvolume.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>

#include <stdio.h>
#include "stdlib.h"
#include <Windows.h>
#include <setupapi.h>

#pragma comment(lib, "setupapi.lib")

DeviceManager::DeviceManager()
{

}

bool DeviceManager::GetDefaultDevice(IMMDevice** ppMMDevice)
{
	bool ret = false;
	IMMDeviceEnumerator* pMMDeviceEnumerator = nullptr;
	wchar_t wszErrMsg[MAX_PATH] = { 0 };

	try
	{
		HRESULT hr = CoCreateInstance(
			__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
			__uuidof(IMMDeviceEnumerator),
			reinterpret_cast<void**>(&pMMDeviceEnumerator)
		);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
			throw std::exception();
		}

		// get the default render endpoint
		hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, ppMMDevice
		);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: hr = 0x%08x", hr);
			throw std::exception();
		}

		ret = true;
	}
	catch (std::exception&)
	{
		PNTERRMSG(wszErrMsg);
	}

	//释放资源
	CUSTOM_FINALLY
	{
		if (pMMDeviceEnumerator != nullptr)
		{
			pMMDeviceEnumerator->Release();
		}
	}

	return ret;
}
bool DeviceManager::GetDefaultDevice(std::wstring& deviceId, std::wstring& deviceName)
{
	IMMDevice* pMMDevice = nullptr;
	if (!GetDefaultDevice(&pMMDevice))
	{
		return false;
	}

	bool ret = true;
	wchar_t wszErrMsg[MAX_PATH] = { 0 };
	HRESULT hr;
	IPropertyStore* pPropertyStore = nullptr;
	PROPVARIANT pv;
	PropVariantInit(&pv);

	try
	{
		LPWSTR pwszDeviceId = nullptr;
		hr = pMMDevice->GetId(&pwszDeviceId);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"IMMDevice::GetId failed: hr = 0x%08x", hr);
			throw;
		}

		// open the property store on that device
		hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"IMMDevice::OpenPropertyStore failed: hr = 0x%08x", hr);
			throw;
		}

		// get the long name property
		hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"IPropertyStore::GetValue failed: hr = 0x%08x", hr);
			throw;
		}

		if (VT_LPWSTR != pv.vt)
		{
			wsprintf(wszErrMsg, L"PKEY_Device_FriendlyName variant type is %u - expected VT_LPWSTR", pv.vt);
			throw;
		}

		deviceId = pwszDeviceId;
		deviceName = pv.pwszVal;
	}
	catch (std::exception&)
	{
		PNTERRMSG(wszErrMsg);
		ret = false;
	}

	//释放资源
	CUSTOM_FINALLY
	{
		if (pMMDevice != nullptr)
		{
			pMMDevice->Release();
		}

		if (pPropertyStore != nullptr)
		{
			pPropertyStore->Release();
		}

		if (pv.vt != VT_EMPTY)
		{
			hr = PropVariantClear(&pv);
			if (FAILED(hr))
			{
				PNTERR(L"PropVariantClear failed: hr = 0x%08x", hr);
				ret = false;
			}
		}
	}

	return ret;
}

bool DeviceManager::GetDevices(std::map<std::wstring, std::wstring>& deviceMap)
{
	deviceMap.clear();

	bool ret = false;
	wchar_t wszErrMsg[MAX_PATH] = { 0 };
	IMMDeviceEnumerator* pMMDeviceEnumerator = nullptr;
	IMMDeviceCollection* pMMDeviceCollection = nullptr;

	try
	{
		HRESULT hr = CoCreateInstance(
			__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
			__uuidof(IMMDeviceEnumerator),
			reinterpret_cast<void**>(&pMMDeviceEnumerator)
		);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
			throw std::exception();
		}

		// get all the active render endpoints
		hr = pMMDeviceEnumerator->EnumAudioEndpoints(
			eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection
		);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"IMMDeviceEnumerator::EnumAudioEndpoints failed: hr = 0x%08x", hr);
			throw std::exception();
		}

		ret = GetDevices(pMMDeviceCollection, deviceMap);
	}
	catch (std::exception&)
	{
		PNTERRMSG(wszErrMsg);
	}

	//释放资源
	CUSTOM_FINALLY
	{
		if (pMMDeviceEnumerator != nullptr)
		{
			pMMDeviceEnumerator->Release();
		}

		if (pMMDeviceCollection != nullptr)
		{
			pMMDeviceCollection->Release();
		}
	}

	return ret;
}

bool DeviceManager::GetDevices(IMMDeviceCollection* pMMDeviceCollection, std::map<std::wstring, std::wstring>& deviceMap)
{
	bool ret = true;
	wchar_t wszErrMsg[MAX_PATH] = { 0 };

	UINT count;
	HRESULT hr = pMMDeviceCollection->GetCount(&count);
	if (FAILED(hr))
	{
		wsprintf(wszErrMsg, L"IMMDeviceCollection::GetCount failed: hr = 0x%08x", hr);
		PNTERRMSG(wszErrMsg);
		return false;
	}
	//PNTLOG(L"Active render endpoints found: %u", count);

	for (UINT i = 0; i < count; i++)
	{
		IMMDevice* pMMDevice = nullptr;
		IPropertyStore* pPropertyStore = nullptr;
		PROPVARIANT pv;
		PropVariantInit(&pv);

		try
		{
			// get the "n"th device
			hr = pMMDeviceCollection->Item(i, &pMMDevice);
			if (FAILED(hr))
			{
				wsprintf(wszErrMsg, L"IMMDeviceCollection::Item failed: hr = 0x%08x", hr);
				throw;
			}

			LPWSTR pwszDeviceId = nullptr;
			hr = pMMDevice->GetId(&pwszDeviceId);
			if (FAILED(hr))
			{
				wsprintf(wszErrMsg, L"IMMDeviceCollection::GetId failed: hr = 0x%08x", hr);
				throw;
			}

			// open the property store on that device
			hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
			if (FAILED(hr))
			{
				wsprintf(wszErrMsg, L"IMMDevice::OpenPropertyStore failed: hr = 0x%08x", hr);
				throw;
			}

			// get the long name property
			hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
			if (FAILED(hr))
			{
				wsprintf(wszErrMsg, L"IPropertyStore::GetValue failed: hr = 0x%08x", hr);
				throw;
			}

			if (VT_LPWSTR != pv.vt)
			{
				wsprintf(wszErrMsg, L"PKEY_Device_FriendlyName variant type is %u - expected VT_LPWSTR", pv.vt);
				throw;
			}

			deviceMap[pwszDeviceId] = pv.pwszVal;

			//PNTLOG(L"    %ls", pv.pwszVal);
		}
		catch (std::exception&)
		{
			PNTERRMSG(wszErrMsg);
			ret = false;
		}

		//释放资源
		CUSTOM_FINALLY
		{
			if (pMMDevice != nullptr)
			{
				pMMDevice->Release();
			}

			if (pPropertyStore != nullptr)
			{
				pPropertyStore->Release();
			}

			if (pv.vt != VT_EMPTY)
			{
				hr = PropVariantClear(&pv);
				if (FAILED(hr))
				{
					PNTERR(L"PropVariantClear failed: hr = 0x%08x", hr);
					ret = false;
				}
			}
		}

			//出现异常
			if (!ret)
			{
				break;
			}
	}

	return ret;
}

bool DeviceManager::GetSpecificDevice(LPCWSTR pwszLongName, IMMDevice** ppMMDevice)
{
	bool ret = false;
	wchar_t wszErrMsg[MAX_PATH] = { 0 };
	IMMDeviceEnumerator* pMMDeviceEnumerator = nullptr;
	IMMDeviceCollection* pMMDeviceCollection = nullptr;
	*ppMMDevice = nullptr;

	try
	{
		HRESULT hr = CoCreateInstance(
			__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
			__uuidof(IMMDeviceEnumerator),
			reinterpret_cast<void**>(&pMMDeviceEnumerator)
		);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
			throw std::exception();
		}

		// get all the active render endpoints
		hr = pMMDeviceEnumerator->EnumAudioEndpoints(
			eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection
		);
		if (FAILED(hr))
		{
			wsprintf(wszErrMsg, L"IMMDeviceEnumerator::EnumAudioEndpoints failed: hr = 0x%08x", hr);
			throw std::exception();
		}

		ret = GetSpecificDevice(pMMDeviceCollection, pwszLongName, ppMMDevice);
	}
	catch (std::exception&)
	{
		PNTERRMSG(wszErrMsg);
	}

	//释放资源
	CUSTOM_FINALLY
	{
		if (pMMDeviceEnumerator != nullptr)
		{
			pMMDeviceEnumerator->Release();
		}

		if (pMMDeviceCollection != nullptr)
		{
			pMMDeviceCollection->Release();
		}
	}

	return ret;
}

bool DeviceManager::GetSpecificDevice(IMMDeviceCollection* pMMDeviceCollection, LPCWSTR pwszLongName, IMMDevice** ppMMDevice)
{
	bool ret = true;
	wchar_t wszErrMsg[MAX_PATH] = { 0 };

	UINT count;
	HRESULT hr = pMMDeviceCollection->GetCount(&count);
	if (FAILED(hr))
	{
		wsprintf(wszErrMsg, L"IMMDeviceCollection::GetCount failed: hr = 0x%08x", hr);
		PNTERRMSG(wszErrMsg);
		return false;
	}
	PNTLOG(L"Active render endpoints found: %u", count);

	for (UINT i = 0; i < count; i++)
	{
		IMMDevice* pMMDevice = nullptr;
		IPropertyStore* pPropertyStore = nullptr;
		PROPVARIANT pv;
		PropVariantInit(&pv);

		try
		{
			// get the "n"th device
			hr = pMMDeviceCollection->Item(i, &pMMDevice);
			if (FAILED(hr))
			{
				wsprintf(wszErrMsg, L"IMMDeviceCollection::Item failed: hr = 0x%08x", hr);
				throw;
			}

			// open the property store on that device
			hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
			if (FAILED(hr))
			{
				wsprintf(wszErrMsg, L"IMMDevice::OpenPropertyStore failed: hr = 0x%08x", hr);
				throw;
			}

			// get the long name property
			hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
			if (FAILED(hr))
			{
				wsprintf(wszErrMsg, L"IPropertyStore::GetValue failed: hr = 0x%08x", hr);
				throw;
			}

			if (VT_LPWSTR != pv.vt)
			{
				wsprintf(wszErrMsg, L"PKEY_Device_FriendlyName variant type is %u - expected VT_LPWSTR", pv.vt);
				throw;
			}

			// is it a match?
			if (0 == _wcsicmp(pv.pwszVal, pwszLongName))
			{
				// did we already find it?
				if (nullptr == *ppMMDevice)
				{
					*ppMMDevice = pMMDevice;
					pMMDevice->AddRef();
				}
				else
				{
					wsprintf(wszErrMsg, L"Found (at least) two devices named %ls", pwszLongName);
					throw;
				}
			}
		}
		catch (std::exception&)
		{
			PNTERRMSG(wszErrMsg);
			ret = false;
		}

		//释放资源
		CUSTOM_FINALLY
		{
			if (pMMDevice != nullptr)
			{
				pMMDevice->Release();
			}

			if (pPropertyStore != nullptr)
			{
				pPropertyStore->Release();
			}

			if (pv.vt != VT_EMPTY)
			{
				hr = PropVariantClear(&pv);
				if (FAILED(hr))
				{
					PNTERR(L"PropVariantClear failed: hr = 0x%08x", hr);
					ret = false;
				}
			}
		}

			//出现异常
			if (!ret)
			{
				if (*ppMMDevice != nullptr)
				{
					(*ppMMDevice)->Release();
				}
				break;
			}
	}

	if (nullptr == *ppMMDevice)
	{
		PNTERR(L"Could not find a device named %ls", pwszLongName);
	}

	return ret;
}

void  DeviceManager::PrintDevicesInfo1()
{

	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		printf("SetupDiGetClassDevs Err:%d", GetLastError());
		return;
	};

	SP_CLASSIMAGELIST_DATA _spImageData = { 0 };
	_spImageData.cbSize = sizeof(SP_CLASSIMAGELIST_DATA);
	SetupDiGetClassImageList(&_spImageData);

	short  wIndex = 0;
	SP_DEVINFO_DATA spDevInfoData = { 0 };
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	while (1)
	{
		if (SetupDiEnumDeviceInfo(hDevInfo, wIndex, &spDevInfoData))
		{
			char  szBuf[MAX_PATH] = { 0 };
			int  wImageIdx = 0;
			short  wItem = 0;
			if (!SetupDiGetDeviceRegistryPropertyA(hDevInfo, &spDevInfoData, SPDRP_CLASS, NULL, (PBYTE)szBuf, MAX_PATH, 0))
			{
				wIndex++;
				continue;
			};

			if (SetupDiGetClassImageIndex(&_spImageData, &spDevInfoData.ClassGuid, &wImageIdx))
			{
				char  szName[MAX_PATH] = { 0 };
				DWORD  dwRequireSize;
				//
				if (!SetupDiGetClassDescription(&spDevInfoData.ClassGuid,PWSTR( szBuf), MAX_PATH, &dwRequireSize))
				{
					wIndex++;
					continue;
				};
				printf("Class:%s\r\n", szBuf);

				if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)szName, MAX_PATH - 1, 0))
				{
					printf("Device:%s\r\n\r\n", szName);
				}
				else  if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)szName, MAX_PATH - 1, 0))
				{
					printf("Device:%s\r\n\r\n", szName);
				};
			};
		}
		else
			break;
		wIndex++;
	};

	SetupDiDestroyClassImageList(&_spImageData);

}




int  DeviceManager::main()
{

	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD  i;

	// 得到所有设备 HDEVINFO      
	hDevInfo = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);

	if (hDevInfo == INVALID_HANDLE_VALUE)
		return  0;

	// 循环列举     
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		char  szClassBuf[MAX_PATH] = { 0 };
		char  szDescBuf[MAX_PATH] = { 0 };

		// 获取类名  
		if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_CLASS, NULL, (PBYTE)szClassBuf, MAX_PATH - 1, NULL))
			continue;

		//获取设备描述信息
		if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)szDescBuf, MAX_PATH - 1, NULL))
			continue;

		printf("Class:%s\r\nDesc:%s\r\n\r\n", szClassBuf, szDescBuf);
	}

	//  释放     
	SetupDiDestroyDeviceInfoList(hDevInfo);

	getchar();

	return  0;
}