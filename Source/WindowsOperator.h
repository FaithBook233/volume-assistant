#pragma once
#include <iostream>

class WindowsOperator
{
public:
	//获取系统我的文档位置
	//用于寻找保存路径
	bool GetDocumentPath(std::string& SystemDocumentPath)const;
};

