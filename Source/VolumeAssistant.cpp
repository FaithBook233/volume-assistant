#include <iostream>
#include "VolumeManager.h"
#include "WindowsOperator.h"
using namespace std;

int main()
{
    string DocumentPath;
    /* if*/ WindowsOperator::GetDocumentPath(DocumentPath);
        cout << DocumentPath << endl;
    /*else cout << "获取路径失败" << endl;*/
    int NewVolume;
    VolumeManager MyVolumeManager = VolumeManager();
    while (true)
    {
        cout << "当前音量" << MyVolumeManager.GetSystemVolume() << endl << "请输入新音量" << endl;
        cin >> NewVolume;
        MyVolumeManager.SetSystemVolume(NewVolume);
    }
}
