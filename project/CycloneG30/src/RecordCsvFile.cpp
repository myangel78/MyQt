#include "RecordCsvFile.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#include "Log.h"


RecordFile::RecordFile(const std::string &filepathstr)
{
    OpenRecordFile(filepathstr);
}
RecordFile::~RecordFile()
{
    CloseRecordFile();
}

bool RecordFile::OpenRecordFile(const std::string &filepathstr)
{
    if(filepathstr.empty())
        return false;

    std::filesystem::path filePath(filepathstr);
    if (!std::filesystem::exists(filePath))
    {
        std::filesystem::create_directories(filePath);
    }

    if(m_pFile != nullptr && m_pFile->isOpen())
        return true;
    std::string datetime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toStdString();
    m_pFile = new QFile((filepathstr + datetime + ".csv").c_str());
    if (!m_pFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        LOGE("Save status file FAILED!!!");
        return false;
    }
    else
    {
        m_pTextStream = new QTextStream(m_pFile);
        if(m_pTextStream != nullptr)
        {
            return true;
        }
        else
        {
            m_pFile->close();
            delete m_pFile;
            m_pFile = nullptr;
        }

    }
    return false;

}
bool RecordFile::CloseRecordFile(void)
{
    if(m_pFile != nullptr)
    {
        m_pFile->close();
        delete m_pFile;
        m_pFile = nullptr;
    }
    if (m_pTextStream != nullptr)
    {
        delete m_pTextStream;
        m_pTextStream = nullptr;
    }
    return true;

}
bool RecordFile::RecordInfo(const std::string &strInfo)
{
    if(m_pFile != nullptr && m_pFile->isOpen() && m_pTextStream != nullptr)
    {
        *m_pTextStream << QString::fromStdString(strInfo);
        m_pTextStream->flush();
        return true;
    }
    return false;
}
