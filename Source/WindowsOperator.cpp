#include "WindowsOperator.h"
#include <windows.h>
#include <iostream>
#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

bool WindowsOperator::GetDocumentPath(std::string& SystemDocumentPath) const
{
    CHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
    SystemDocumentPath = std::string(my_documents);
    return S_OK;
}
