#ifndef ILOGEVENTUPDATE_H
#define ILOGEVENTUPDATE_H

#include <string>

class ILogEventUpdate
{
public:
    virtual void logUpdate(int nType, const std::string& sMsg) = 0;
};

#endif // ILOGEVENTUPDATE_H
