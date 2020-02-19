#pragma once
#include "../FolderScan/IFolderScan.h"
#include <windows.h>
class CFolderScanRun : public IFolderScanCallBack
{
public:
	CFolderScanRun();
	~CFolderScanRun();

	void BeginScan(const std::string& folderName);
	bool IsFindFinish();
	void StopScan();
	void PrintfFile();
	virtual void ScanError(FOLDERSCANERROR ret);
	virtual void SendAllFolderScanFile(const std::vector<FileInfo>& vecFileInfo);

private:
	IFolderScan* m_pFolderScan;
	HMODULE m_hDll;

	std::vector<FileInfo> m_vecFileInfo;
};

