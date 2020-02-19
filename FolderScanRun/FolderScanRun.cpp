#include "FolderScanRun.h"
#include <windows.h>
#include <iostream>

typedef IFolderScan* ( *CreateFun)(IFolderScanCallBack* callBack);
typedef void( *ReleaseFun)(IFolderScan*);

CFolderScanRun::CFolderScanRun()
: IFolderScanCallBack()
{

	m_hDll = LoadLibrary("FolderScan.dll");

	if (m_hDll != NULL)
	{
		CreateFun func = (CreateFun)GetProcAddress(m_hDll, "CreateIFolderScan");
		m_pFolderScan = func(this);	
	}
}


CFolderScanRun::~CFolderScanRun()
{
	if (m_pFolderScan && m_hDll)
	{
		ReleaseFun func = (ReleaseFun)GetProcAddress(m_hDll, "ReleaseIFolderScan");
		func(m_pFolderScan);
	}
	FreeLibrary(m_hDll);
}

void CFolderScanRun::BeginScan(const std::string& folderName)
{
	if (m_pFolderScan)
	{
		m_pFolderScan->BeginScan(folderName);
	}
}
bool CFolderScanRun::IsFindFinish()
{
	if (m_pFolderScan)
	{
		return m_pFolderScan->IsScaning();
	}
	return false;
}
void CFolderScanRun::StopScan()
{
	if (m_pFolderScan)
	{
		m_pFolderScan->StopScan();
	}
}

void CFolderScanRun::PrintfFile()
{
	for (size_t i = 0; i < m_vecFileInfo.size(); ++i)
	{
		std::cout << " name = " << m_vecFileInfo[i].name << " size = " << m_vecFileInfo[i].size << std::endl;
	}
}

void CFolderScanRun::ScanError(FOLDERSCANERROR ret)
{
	switch (ret)
	{
	case FS_FolderNoExistent:
		std::cout << " 文件夹不存在！！！  " << std::endl;
		break;
	default:
		break;
	}	
		
}
void CFolderScanRun::SendAllFolderScanFile(const std::vector<FileInfo>& vecFileInfo)
{
	m_vecFileInfo = vecFileInfo;

}