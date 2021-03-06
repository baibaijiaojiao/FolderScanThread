#include "FolderScan.h"
#include "ThreadPool.h"
#include <io.h>
#include <iostream>
CFolderScan::CFolderScan(IFolderScanCallBack* callBack)
:IFolderScan(callBack)
, m_callBack(callBack)
, m_ThreadPool(NULL)
, m_iTask(0)
, m_finding(false)
{
	unsigned long const hardware_threads = std::thread::hardware_concurrency();
	m_ThreadPool = new ThreadPool(hardware_threads);
}


CFolderScan::~CFolderScan()
{
	StopScan();
	if (m_ThreadPool)
	{
		delete m_ThreadPool;
		m_ThreadPool = NULL;
	}
}

void CFolderScan::BeginScan(const std::string& folderName)
{
	if (!CheckFolderPath(folderName))
	{
		if (m_callBack)
		{
			m_callBack->ScanError(FS_FolderNoExistent);
		}
		return;
	}

	m_folderName = folderName;

	{
		std::unique_lock<std::mutex> lock(m_findMutex);
		m_finding = true;
	}

	if (m_ThreadPool)
	{
		AddTask();
		m_ThreadPool->enqueue(&CFolderScan::GetFiles, this, m_folderName);
		m_result = m_ThreadPool->enqueue(&CFolderScan::CheckFindFinish, this);

	}
}

bool CFolderScan::CheckFolderPath(const std::string& folderName)
{
	std::string fileFolder = folderName + "\\*";
	std::string fileName;
	struct _finddata_t fileInfo;
	long findResult = _findfirst(fileFolder.c_str(), &fileInfo);
	bool isOk = findResult != -1;
	_findclose(findResult);

	return isOk;
}

void CFolderScan::GetFiles(std::string fileFolderPath)
{
	//std::cout << " 正在查找目录："<< fileFolderPath << std::endl;
	std::vector<FileInfo> vecfileTemp;
	std::string fileFolder = fileFolderPath + "\\*";
	FileInfo file;
	struct _finddata_t fileInfo;
	long findResult = _findfirst(fileFolder.c_str(), &fileInfo);
	if (findResult == -1 || !m_finding)
	{
		_findclose(findResult);
		DeleteTask();
		m_condition.notify_one();
		return;
	}
	bool flag = 0;

	do
	{
		if(!m_finding)
			break;

		file.name = fileFolderPath + "\\" + fileInfo.name;
		file.size = fileInfo.size;
		if (fileInfo.attrib & _A_ARCH)
		{				
			vecfileTemp.push_back(file);
		}
		else if (fileInfo.attrib & _A_SUBDIR) //目录
		{
			//判断是否为"."当前目录，".."上一层目录
			if ((m_ThreadPool && strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))
			{
				//std::cout << " 添加查找目录：" << fileFolderPath << " " << fileInfo.name << std::endl;
				AddTask();
				m_ThreadPool->enqueue(&CFolderScan::GetFiles, this, file.name);
			}

		}
	} while (_findnext(findResult, &fileInfo) == 0);

	InsertFileInfo(vecfileTemp);
	_findclose(findResult);
	DeleteTask();
	m_condition.notify_one();
	return;
}


bool CFolderScan::IsScaning()
{
	return m_iTask > 0;
}

void CFolderScan::StopScan()
{
	if (m_finding)
	{
		std::unique_lock<std::mutex> lock(m_findMutex);
		m_finding = false;
	}
 }

void CFolderScan::AddTask()
{
	std::unique_lock<std::mutex> lock(m_TaskMutex);
	++m_iTask;
	//std::cout << "AddTask = " << m_iTask << std::endl;
}
void CFolderScan::DeleteTask()
{
	std::unique_lock<std::mutex> lock(m_TaskMutex);
	--m_iTask;
	//std::cout << "DeleteTask = " << m_iTask << std::endl;
}

void CFolderScan::CheckFindFinish()
{
	while (m_iTask > 0)
	{
		{
			std::unique_lock<std::mutex> lock(this->m_TaskMutex);
			m_condition.wait(lock,
				[this]{ return m_iTask <= 0; }); //防止伪唤醒	
			m_iTask = 0;
		}
		if (m_callBack)
		{
			std::unique_lock<std::mutex> lock(m_fileMutex);
			std::vector< FileInfo > allFile;
			for (auto && vecfile : m_vecFileInfo)
			{
				allFile.insert(allFile.end(), vecfile.begin(), vecfile.end());
			}
			m_callBack->SendAllFolderScanFile(allFile);
		}
	}
}

void CFolderScan::InsertFileInfo(const std::vector< FileInfo >& vecfiles)
{
	std::unique_lock<std::mutex> lock(m_fileMutex);
	m_vecFileInfo.push_back(vecfiles);
}
