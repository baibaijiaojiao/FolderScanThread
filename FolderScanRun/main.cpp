#include "FolderScanRun.h"
#include <iostream>
#include <time.h>

int main()
{
	CFolderScanRun m_scan;
	std::cout << clock() << std::endl;
	m_scan.BeginScan("D:");

	while (m_scan.IsFindFinish()) {
	};
	std::cout << clock() << std::endl;
	system("pause");
	m_scan.PrintfFile();
	return 0;
}