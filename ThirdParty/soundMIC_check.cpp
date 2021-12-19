#pragma comment(lib, "winmm.lib")
#include <Mmsystem.h>

#include <stdio.h>
#include <tchar.h>
#include <mciapi.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys.h>

#include "WavRecordDemoDlg.h"
#define SAFE_RELEASE(punk)  \
if ((punk) != NULL)  \
{ (punk)->Release(); (punk) = NULL; } 

#include <mmdeviceapi.h>
#include "iostream"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define BUF_LEN	4096

CWavRecordDemoDlg* g_pthisdlg = NULL;

///
///功能：扫描windows系统音频终端设备信息(比如：耳机、耳麦、CD等信息)
///
int  EnumAudioPoint()
{
	int result = 0;
	USES_CONVERSION;
	CoInitialize(NULL); //初始化COM库
	HRESULT hr;
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;
	//通过COM库创建IMMDeviceEnumerator接口实例，用于下面枚举音频设备信息
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	if (hr != S_OK)
		goto EXIT_PRO;

	//通过IMMDeviceEnumerator接口实例，获取音频设备的集合接口IMMDeviceCollection实例

	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATEMASK_ALL, &deviceCollection);
	if (hr != S_OK)
		goto EXIT_PRO;

	UINT deviceCount = 0;
	//获取当前有多少音频设备
	deviceCollection->GetCount(&deviceCount);
	//打印终端音频设备数量
	wprintf_s(L"EndPointDevices Count: %d\n", deviceCount);
	for (UINT i = 0; i < deviceCount; ++i)
	{
		IMMDevice* Device = NULL;
		//获取每个音频设备
		hr = deviceCollection->Item(i, &Device);
		if (hr != S_OK)
		{
			if (Device != NULL)
				Device->Release();
			continue;

		}

		LPWSTR DeviceID = NULL;
		//获取音频设备的ID
		hr = Device->GetId(&DeviceID);
		if (hr == S_OK)
		{
			wprintf_s(L"EndPointDevice%d DeviceID: %s\n", i + 1, DeviceID);
		}

		IPropertyStore *pProps = NULL;
		//获取音频设备的属性信息

		hr = Device->OpenPropertyStore(STGM_READ, &pProps);
		if (hr == S_OK)
		{
			PROPVARIANT varName;
			PropVariantInit(&varName);
			hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
			if (hr == S_OK)
			{
				//char* pfriendname = W2A(varName.pwszVal);
				//TRACE("EndPointDevice%d DeviceName: %s\n", pfriendname);
				wprintf_s(L"EndPointDevice%d DeviceName: %s\n", i + 1, varName.pwszVal);
			}
		}
		/**********下面通过设备的拓扑结构性质找寻所连接的外接设备比如耳机耳麦等**********/
		IDeviceTopology *pDeviceTopology = NULL;
		IConnector *pConnFrom = NULL;
		IConnector *pConnTo = NULL;
		IPart *pPart = NULL;
		IKsJackDescription *pJackDesc = NULL;
		//拓扑性
		hr = Device->Activate(__uuidof(IDeviceTopology), CLSCTX_ALL, NULL, (void**)&pDeviceTopology);
		if (hr != S_OK)
		{
			goto EXIT_FOR;
		}

		//连接
		hr = pDeviceTopology->GetConnector(0, &pConnFrom);
		if (hr != S_OK)
		{
			goto EXIT_FOR;
		}
		//连接
		hr = pConnFrom->GetConnectedTo(&pConnTo);
		if (HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND) == hr)
		{
			// The adapter device is not currently active.
			hr = E_NOINTERFACE;
		}

		if (hr != S_OK)
			goto EXIT_FOR;

		hr = pConnTo->QueryInterface(__uuidof(IPart), (void**)&pPart);
		if (hr != S_OK)
		{
			goto EXIT_FOR;
		}
		//获取描述外接设备信息
		hr = pPart->Activate(CLSCTX_INPROC_SERVER, __uuidof(IKsJackDescription), (void**)&pJackDesc);
		if (hr != S_OK)
		{
			goto EXIT_FOR;
		}
		UINT num = 0;
		pJackDesc->GetJackCount(&num);
		TRACE(_T("GetJackCount: %d\n"), num);
		for (UINT j = 0; j < num; ++j)
		{
			KSJACK_DESCRIPTION ksjack_desc = { 0 };
			hr = pJackDesc->GetJackDescription(j, &ksjack_desc);
			if (hr == S_OK)
			{
				TRACE(_T("Jack%d PortConnection: %d\n"), j + 1, ksjack_desc.PortConnection);
				TRACE(_T("Jack%d ConnectionType: %d\n"), j + 1, ksjack_desc.ConnectionType);
				TRACE(_T("Jack%d IsConnected: %d\n\n"), j + 1, ksjack_desc.IsConnected);

				if (ksjack_desc.IsConnected)
				{
					if (ksjack_desc.ConnectionType == eConnType3Point5mm)//连接类型
					{
						TRACE(_T("%d KSJACK_DESCRIPTION: %d\n"), j + 1, ksjack_desc.ConnectionType);
						result = 1;
						break;
					}

				}
				else
				{
					if (ksjack_desc.ConnectionType == eConnType3Point5mm)//连接类型
					{
						TRACE(_T("%d KSJACK_DESCRIPTION: %d\n"), j + 1, ksjack_desc.ConnectionType);
						result = 2;
						break;
					}
				}

			}
		}
	EXIT_FOR:
		if (pDeviceTopology)
			pDeviceTopology->Release();
		if (pConnFrom)
			pConnFrom->Release();
		if (pConnTo)
			pConnTo->Release();
		if (pPart)
			pPart->Release();
		if (pJackDesc)
			pJackDesc->Release();
	}

EXIT_PRO:
	if (deviceEnumerator)
		deviceEnumerator->Release();
	if (deviceCollection)
		deviceCollection->Release();
	CoUninitialize();

	return result;

}


class CMMNotificationClient : public IMMNotificationClient
{
public:

	IMMDeviceEnumerator *m_pEnumerator;
	CWavRecordDemoDlg* m_pthisdlg;
	CMMNotificationClient() :
		_cRef(1),
		m_pEnumerator(NULL),
		m_pthisdlg(nullptr)
	{
		//初始化COM
		::CoInitialize(NULL);
		HRESULT hr = S_OK;

		//创建接口
		hr = CoCreateInstance(
			__uuidof(MMDeviceEnumerator), NULL,
			CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
			(void**)&m_pEnumerator);

		if (hr == S_OK)
		{
			//cout << "接口创建成功" << endl;
		}
		else
		{
			cout << "接口创建失败" << endl;
		}
		//注册事件
		hr = m_pEnumerator->RegisterEndpointNotificationCallback((IMMNotificationClient*)this);
		if (hr == S_OK)
		{
			cout << "注册成功" << endl;
		}
		else
		{
			cout << "注册失败" << endl;
		}
	}

	~CMMNotificationClient()
	{
		SAFE_RELEASE(m_pEnumerator)
			::CoUninitialize();
	}
	void SetMainDlg(CWavRecordDemoDlg* _this)
	{
		m_pthisdlg = _this;
	}


	// IUnknown methods -- AddRef, Release, and QueryInterface 
private:
	LONG _cRef;
	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&_cRef);
		if (0 == ulRef)
		{
			delete this;
		}
		return ulRef;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid, VOID **ppvInterface)
	{
		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IMMNotificationClient) == riid)
		{
			AddRef();
			*ppvInterface = (IMMNotificationClient*)this;
		}
		else
		{
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
		EDataFlow flow, ERole role,
		LPCWSTR pwstrDeviceId)
	{
		//cout<<"OnDefaultDeviceChanged"<<endl; 

		TRACE("OnDefaultDeviceChanged \n");
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId)
	{
		TRACE("OnDeviceAdded \n");
		return S_OK;
	};

	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId)
	{
		TRACE("OnDeviceRemoved \n");
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(
		LPCWSTR pwstrDeviceId,
		DWORD dwNewState)
	{
		cout << "OnDeviceStateChanged" << endl;

		TRACE("OnDeviceStateChanged \n");
		return S_OK;
	}
	// Given an endpoint ID string, print the friendly device name.
	HRESULT CMMNotificationClient::_PrintDeviceName(LPCWSTR pwstrId)
	{
		USES_CONVERSION;
		HRESULT hr = S_OK;
		IMMDevice *pDevice = NULL;
		IPropertyStore *pProps = NULL;
		PROPVARIANT varString;

		CoInitialize(NULL);
		PropVariantInit(&varString);

		if (m_pEnumerator == NULL)
		{
			// Get enumerator for audio endpoint devices.
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
				NULL, CLSCTX_INPROC_SERVER,
				__uuidof(IMMDeviceEnumerator),
				(void**)&m_pEnumerator);
		}
		if (hr == S_OK)
		{
			hr = m_pEnumerator->GetDevice(pwstrId, &pDevice);
		}
		if (hr == S_OK)
		{
			hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
		}
		if (hr == S_OK)
		{
			// Get the endpoint device's friendly-name property.
			static PROPERTYKEY key;

			//GUID IDevice_FriendlyName = { 0x2cce9ba5, 0xe3ca, 0x49c4, { 0x9d, 0x45, 0x60, 0x24, 0xff, 0x9f, 0xc4, 0xcc } };
			//key.pid = 4;
			//key.fmtid = IDevice_FriendlyName;
			hr = pProps->GetValue(PKEY_Device_FriendlyName, &varString);
		}
		char* pfriendname = W2A(varString.pwszVal);
		char* pfriendid = W2A(pwstrId);
		TRACE("Device name: %s    Endpoint ID string:%s\n", pfriendname, pfriendid),


			PropVariantClear(&varString);

		SAFE_RELEASE(pProps)
			SAFE_RELEASE(pDevice)
			CoUninitialize();
		return hr;
	}
	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(
		LPCWSTR pwstrDeviceId,
		const PROPERTYKEY key)
	{
		static int id = 0;

		//_PrintDeviceName(pwstrDeviceId);

		int result = EnumAudioPoint();
		m_pthisdlg->ERJIState(result);
		//TRACE("OnPropertyValueChanged %s %d \n",
		//	GetCurrentTimeString().c_str(), id++);
		return S_OK;
	}
};