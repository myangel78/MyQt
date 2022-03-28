#pragma once
#include <Global.h>


class ValidOptimized
{
public:
    ValidOptimized(void);


private:
    std::vector<std::vector<CHANNEL_VALID_ENUM>> m_vctValidStore;
    std::vector<int> m_vctValidStoreCnt;

private:
    void Init(void);
    void ClearStore(void);
public:

    void ResetTimes(void);
    void MultiValidFlitOptimize(std::vector<std::vector<CHANNEL_VALID_ENUM>>& vetValidState);
    void StoreValidState(const int &curTime,const std::vector<std::vector<CHANNEL_VALID_ENUM>>& vetValidState,\
                         const std::vector<std::vector<int>> &vctValidCnt);
};

