#pragma once

#include <string>
#include <vector>



enum FOLDERSCANERROR
{
	FS_OK = 0,
	FS_FolderNoExistent,
};

struct FileInfo
{
	unsigned long size;
	std::string name;
};

class IFolderScanCallBack
{
public:
	IFolderScanCallBack() {};
	virtual ~IFolderScanCallBack() {};

	virtual void ScanError(FOLDERSCANERROR ret) = 0;
	virtual void SendAllFolderScanFile(const std::vector<FileInfo>& vecFileInfo) = 0;

};

class IFolderScan
{
public:
	IFolderScan(IFolderScanCallBack* callBack) {};
	virtual ~IFolderScan() {};

	virtual void BeginScan(const std::string& folderName) = 0;
	virtual bool IsScaning() = 0;
	virtual void StopScan() = 0;
};



