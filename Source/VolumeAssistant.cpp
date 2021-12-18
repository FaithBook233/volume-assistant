#include <iostream>

#include "VolumeManager.h"
#include "WindowsOperator.h"
#include "DeviceManager.h"
using namespace std;

//int main()
//{
//    string DocumentPath;
//    /* if*/ WindowsOperator::GetDocumentPath(DocumentPath);
//        cout << DocumentPath << endl;
//    /*else cout << "获取路径失败" << endl;*/
//    int NewVolume;
//    VolumeManager MyVolumeManager = VolumeManager();
//    while (true)
//    {
//        cout << "当前音量" << MyVolumeManager.GetSystemVolume() << endl << "请输入新音量" << endl;
//        cin >> NewVolume;
//        MyVolumeManager.SetSystemVolume(NewVolume);
//    }
//}

int main()
{
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
