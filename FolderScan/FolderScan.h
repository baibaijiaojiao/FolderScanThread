#pragma once
#include "IFolderScan.h"
#include <vector>
#include <mutex>
#include <future>

class ThreadPool;

class CFolderScan :
	public IFolderScan
{
public:
	CFolderScan(IFolderScanCallBack* callBack);
	~CFolderScan();

	void BeginScan(const std::string& folderName);
	bool IsScaning();
	void StopScan();

private:
	bool CheckFolderPath(const std::string& folderName);
	void GetFiles(std::string fileFolderPath);
	void AddTask();
	void DeleteTask();
	void CheckFindFinish();
	void InsertFileInfo(const std::vector< FileInfo >& vecfiles);
private:
	IFolderScanCallBack* m_callBack;
	ThreadPool* m_ThreadPool;
	std::string m_folderName;
	std::mutex m_TaskMutex;
	std::mutex m_fileMutex;
	std::mutex m_findMutex;
	std::future<void> m_result;
	std::vector< std::vector< FileInfo > > m_vecFileInfo;
	long m_iTask;
	bool m_finding;
	std::condition_variable m_condition;
};


extern "C" __declspec(dllexport) IFolderScan*  CreateIFolderScan(IFolderScanCallBack* callBack)
{
	return dynamic_cast<IFolderScan*> (new CFolderScan(callBack));
}

extern "C" __declspec(dllexport) void  ReleaseIFolderScan(IFolderScan* pFolderScan)
{
	if (pFolderScan)
	{
		pFolderScan->StopScan();
		while (pFolderScan->IsScaning()) {};

		delete pFolderScan;
		pFolderScan = NULL;
	}
}

