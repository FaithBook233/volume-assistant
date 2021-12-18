#include <iostream>
#include "VolumeManager.h"

using namespace std;

int main()
{
    while (true)
    {
        cout << VolumeManager::GetSystemVolume() << endl;
    }
}
