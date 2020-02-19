#include "FolderScanRun.h"
#include <iostream>
#include <time.h>

int main()
{
	CFolderScanRun m_scan;
	std::cout << clock() << std::endl;
	m_scan.BeginScan("D:");

	while (m_scan.IsFindFinish()) {
		static int i = 0;
		++i;
		Sleep(1);
		if(i > 100)
			m_scan.StopScan();
	};
	std::cout << clock() << std::endl;
	system("pause");
	m_scan.PrintfFile();
	system("pause");
	return 0;
}