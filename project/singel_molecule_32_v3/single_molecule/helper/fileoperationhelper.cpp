#include "fileoperationhelper.h"

#include <stdio.h>
#include <time.h>

#include <QDir>

bool FileOperationHelper::IsPathExist(const std::string& path)
{
    QString qPath = QString::fromStdString(path);
    return IsPathExist(qPath);
}

bool FileOperationHelper::IsPathExist(const QString& path)
{
    bool bRet = false;
    QFile srcFile(path);
    if(!(bRet = srcFile.exists()))
    {
        QDir dirPath(path);
        bRet = dirPath.exists();
    }
    return bRet;
}

bool FileOperationHelper::copyDir(const std::string& src, const std::string& dest)
{
    QString qSrc = QString::fromStdString(src);
    QString qDest = QString::fromStdString(dest);

    return copyDir(qSrc, qDest);
}

bool FileOperationHelper::copyDir(const QString& src, const QString& dest)
{
    bool bRet = false;

    QDir sourceDir(src);
    if(sourceDir.exists())
    {
        QDir destDir(dest);
        if(tryMakeDir(dest))
        {
            QStringList files = sourceDir.entryList(QDir::Files);
            for(int i = 0; i< files.count(); i++)
            {
                QString srcName = sourceDir.absolutePath() + QDir::separator() + files[i];
                QString destName = destDir.absolutePath() + QDir::separator() + files[i];
                if(!(bRet = QFile::copy(srcName, destName)))
                {
                    break;
                }
            }

            if( bRet )
            {
                QStringList folders = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                for(int i = 0; i< folders.count(); i++)
                {
                    QString srcName = sourceDir.absolutePath() + QDir::separator() + folders[i];
                    QString destName = destDir.absolutePath() + QDir::separator() + folders[i];
                    if(!(bRet = copyDir(srcName.toStdString(), destName.toStdString())))
                    {
                        break;
                    }
                }
            }
        }
    }
    return bRet;
}

bool FileOperationHelper::deleteDir(const std::string& src, bool bDelRoot /*= true*/)
{
    return deleteDir(QString::fromStdString(src), bDelRoot);
}

bool FileOperationHelper::deleteDir(const QString& src, bool bDelRoot /*= true*/)
{
    bool bRet = false;
    QDir sourceDir(src);
    if(sourceDir.exists()
            && sourceDir.removeRecursively()
            )
    {
        if( bDelRoot )
        {
            bRet = sourceDir.rmdir(src);
        }
        else
        {
            bRet = true;
        }
    }
    else
    {
        bRet = true;
    }
    return bRet;
}

bool FileOperationHelper::copyFile(const std::string& srcF, const std::string& dest)
{
    QString qSrc = QString::fromStdString(srcF);
    QString qDest = QString::fromStdString(dest);

    return copyFile(qSrc, qDest);
}

bool FileOperationHelper::copyFile(const QString& srcF, const QString& dest)
{
    bool bRet = false;

    QFile srcFile(srcF);
    if(srcFile.exists())
    {
        QDir destDir(dest);
        if(tryMakeDir(dest))
        {
            QString destName = destDir.absolutePath() + QDir::separator() + srcFile.fileName();
            bRet = QFile::copy(srcF, destName);
        }
    }
    return bRet;
}

bool FileOperationHelper::deleteFile(const std::string& src)
{
    return deleteFile(QString::fromStdString(src));
}

bool FileOperationHelper::deleteFile(const QString& src)
{
    bool bRet = false;
    QFile srcFile(src);
    if(srcFile.exists())
    {
        bRet = QFile::remove(src);
    }
    else
    {
        bRet = true;
    }
    return bRet;
}

bool FileOperationHelper::getAllFiles(std::vector<std::string>& vecFiles, const std::string& src, bool bRecursion /*= true*/)
{
    bool bRet = false;
    QDir sourceDir(QString::fromStdString(src));
    if(sourceDir.exists())
    {
        QFileInfoList files = sourceDir.entryInfoList(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
        for(int i = 0; i< files.count(); i++)
        {
            if(files[i].isFile())
            {
                vecFiles.push_back(files[i].absoluteFilePath().toStdString());
            }
            else if(files[i].isDir())
            {
                if(bRecursion)
                {
                    std::vector<std::string> vecSubFiles;
                    if( (bRet = getAllFiles(vecSubFiles, files[i].absoluteFilePath().toStdString())))
                    {
                        vecFiles.insert(vecFiles.end(), vecSubFiles.cbegin(), vecSubFiles.cend());
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                bRet = false;
                break;
            }
        }

        bRet = true;
    }

    return bRet;
}

bool FileOperationHelper::getAllFolders(std::vector<std::string>& vecFolders, const std::string& src)
{
    bool bRet = false;
    QDir sourceDir(QString::fromStdString(src));
    if(sourceDir.exists())
    {
        QFileInfoList files = sourceDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
        for(int i = 0; i< files.count(); i++)
        {
            if(files[i].isDir())
            {
                vecFolders.push_back(files[i].absoluteFilePath().toStdString());
            }
            else
            {
                continue;
            }
        }

        bRet = true;
    }

    return bRet;
}

std::string FileOperationHelper::getNameWithoutExt(const std::string& path)
{
    QString qPath = QString::fromStdString(path);
    QFileInfo srcFile(qPath);

    auto name = srcFile.fileName().toStdString();

    auto pos = name.rfind('.');
    return name.substr(0, pos);
}

bool FileOperationHelper::tryMakeDir(const QString& sPath)
{
    bool bRet = false;
    QDir newDir(sPath);
    if(!(bRet = newDir.exists()))
    {
        QDir dir("");
        bRet = dir.mkpath(sPath);
    }
    return bRet;
}
