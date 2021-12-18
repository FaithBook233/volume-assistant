#include "WindowsOperator.h"
#include <windows.h>
#include <iostream>
#include <shlobj.h>

//#pragma comment(lib, "shell32.lib")

bool WindowsOperator::GetDocumentPath(std::string& SystemDocumentPath)
{
    CHAR MyDocumentsPath[MAX_PATH];
    HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, MyDocumentsPath);
    SystemDocumentPath = std::string(MyDocumentsPath);
    return result;
}
