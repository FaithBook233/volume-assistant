#include <iostream>
#include "VolumeManager.h"

using namespace std;

int main()
{
    int NewVolume;
    VolumeManager MyVolumeManager = VolumeManager();
    while (true)
    {
        cout << "当前音量" << MyVolumeManager.GetSystemVolume() << endl << "请输入新音量" << endl;
        cin >> NewVolume;
        MyVolumeManager.SetSystemVolume(NewVolume);
    }
}
