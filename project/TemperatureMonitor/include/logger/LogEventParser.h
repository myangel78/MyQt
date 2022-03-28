#ifndef LOGEVENTPARSE_H
#define LOGEVENTPARSE_H

#include <string>
#include <vector>

const int c_nMaxStringSize = 40;
//[2017-12-23 15:40:53:110][error][001121]

class LogEventParser
{
public:
    LogEventParser();
public:
    static bool Parse(const char* pSrc, std::string& sTime, std::string& sType, std::string& sMsg)
    {
        bool bRet = false;
        if (pSrc != NULL && strlen(pSrc) > c_nMaxStringSize)
        {
            char* pChar = NULL;
            char* pToken = NULL;
            char szSeps[] = "[]";
            std::vector<std::string> vecStrs;
    #ifdef linux
            pChar = strtok_r((char*)pSrc, szSeps, &pToken);
    #else
            pChar = strtok_s((char*)pSrc, szSeps, &pToken);
    #endif
            while (pChar != NULL)
            {
                vecStrs.push_back(pChar);
    #ifdef linux
                pChar = strtok_r(NULL, szSeps, &pToken);
    #else
                pChar = strtok_s(NULL, szSeps, &pToken);
    #endif
            }

            if(vecStrs.size()>=4)
            {
                sTime = vecStrs[0];
                sType = vecStrs[1];
                sMsg = vecStrs[3];
            }

            bRet = true;
        }
        return bRet;
    }
};

#endif // LOGEVENTPARSE_H
