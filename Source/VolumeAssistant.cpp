#include <iostream>

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
//    /*else cout << "获取路径失败" << endl;*/
//    int NewVolume;
//    VolumeManager MyVolumeManager = VolumeManager();
//    while (true)
//    {
//        cout << "当前音量" << MyVolumeManager.GetSystemVolume() << endl << "请输入新音量" << endl;
//        cin >> NewVolume;
//        MyVolumeManager.SetSystemVolume(NewVolume);
//    }
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
