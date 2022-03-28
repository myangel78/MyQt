#ifndef FILE_OPERATION_HELPER_H
#define FILE_OPERATION_HELPER_H

#include <string>
#include <vector>
#include <QString>

class FileOperationHelper
{
public:
    static bool IsPathExist(const std::string& path);
    static bool IsPathExist(const QString& path);

    static bool copyDir(const std::string& src, const std::string& dest);
    static bool copyDir(const QString& src, const QString& dest);

    static bool deleteDir(const std::string& src, bool bDelRoot = true);
    static bool deleteDir(const QString& src, bool bDelRoot = true);

    static bool copyFile(const std::string& srcF, const std::string& dest);
    static bool copyFile(const QString& srcF, const QString& dest);

    static bool deleteFile(const std::string& src);
    static bool deleteFile(const QString& src);

    static bool getAllFiles(std::vector<std::string>& vecFiles, const std::string& src, bool bRecursion = true);
    static bool getAllFolders(std::vector<std::string>& vecFolders, const std::string& src);

    static std::string getNameWithoutExt(const std::string& path);

private:
    static bool tryMakeDir(const QString& sPath);

};

#endif // FILE_OPERATION_HELPER_H
