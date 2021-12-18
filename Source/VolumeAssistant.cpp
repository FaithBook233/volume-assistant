#include <iostream>
#include <string>

#include "VolumeManager.h" 
#include "DeviceManager.h"
#include "JsonSerializer.h"


using namespace std;

int main()
{
    CoInitialize(0);
    string DocumentPath;
    JsonSerializer::GetDocumentPath(DocumentPath);
        cout << DocumentPath << endl;

    std::wstring deviceId;
    std::wstring deviceName;
    DeviceManager::GetDefaultDevice(deviceId, deviceName);
    std::wcout << "默认设备ID:" << deviceId  <<std::endl;

    std::cout << "下面是设备列表" << std::endl;
    std::map<wstring, wstring>deviceMap;
    DeviceManager::GetDevices(deviceMap);
    for (auto Current : deviceMap)
    {
        std::wcout << "设备ID：" << Current.first << std::endl;// << "设备名" << Current.second << std::endl << std::endl;
    }

    DeviceManager::main();
 
}

int main2()
{
    CoInitialize(0);
    DeviceManager MyDeviceManager = DeviceManager();
    VolumeManager MyVolumeManager = VolumeManager();
//CheckDevice:
   // MyDeviceManager.CheckDevice();

    //while (true)
    //{
    //    if (MyDeviceManager.AudioDeviceChanged())
    //    {
    //        goto CheckDevice;
    //    }
    //    if (MyVolumeManager.SystemVolumeChanged())
    //    {
    //        WindowsOperator::SaveDeviceAndVolume();
    //    }
    //    Sleep(200);
    //}
    return 0;
}
