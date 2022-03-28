#pragma once
#include <string.h>
#include<iostream>
class QFile;
class QTextStream;

class RecordFile
{
public:
    RecordFile(const std::string &filepathstr);
    ~RecordFile();

private:
    QFile *m_pFile = nullptr;
    QTextStream *m_pTextStream = nullptr;
public:
    bool OpenRecordFile(const std::string &filepathstr);
    bool CloseRecordFile(void);
    bool RecordInfo(const std::string &strInfo);
};
