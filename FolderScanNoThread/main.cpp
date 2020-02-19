#include<iostream>
#include<io.h>
#include<vector>
#include<string>
#include <time.h>

int get_files(std::string fileFolderPath, std::string fileExtension, std::vector<std::string>&file)
{
	std::string fileFolder = fileFolderPath + "\\*" + fileExtension;
	std::string fileName;
	struct _finddata_t fileInfo;
	long findResult = _findfirst(fileFolder.c_str(), &fileInfo);
	if (findResult == -1)
	{
		_findclose(findResult);
		return 0;
	}
	bool flag = 0;

	do
	{
		fileName = fileFolderPath + "\\" + fileInfo.name;
		if (fileInfo.attrib & _A_ARCH)
		{
			//std::cout << fileName << std::endl;
			file.push_back(fileName);
		}
		else if (fileInfo.attrib & _A_SUBDIR)
		{
			//判断是否为"."当前目录，".."上一层目录
			if ((strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))
			{
				//std::cout << fileName << std::endl;
				get_files(fileName, fileExtension, file);
			}
			
		}
	} while (_findnext(findResult, &fileInfo) == 0);

	_findclose(findResult);
	return 0;
}

int main()
{
	//查询文件夹
	std::string fileFolderPath = "D:";

	//输出所有文件
	std::cout << clock() << std::endl;
	std::vector<std::string> all_files;
	std::string fileExtension_all = "";
	get_files(fileFolderPath, fileExtension_all, all_files);
	std::cout << clock() << std::endl;
	system("pause");
	for (size_t i = 0; i < all_files.size(); i++)
	{
		std::cout << all_files[i] << std::endl;
	}
	return 0;
}