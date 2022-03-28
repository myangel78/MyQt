#pragma once

#include <string>
#include <mutex>

#include "hiredis/hiredis.h"


class RedisAccess
{
public:
	RedisAccess();
	~RedisAccess();

private:
	int m_iAccess;
	bool m_bConnected;
	int m_iPort;
	std::string m_strHost;
	//std::string m_strUser;
	std::string m_strPsw;
	int m_indexDB;

	redisContext* m_pContext;
	redisReply* m_pReply;

	std::mutex m_mutexTask;
protected:
	bool IsCmdError(void);
public:
	bool SetAccessParam(const char* pHost, int port, const char* ppsw);
	bool Connecting(void);
	void Disconnect(void);

	bool selectDB(int index = 0);

	//设置key对应的value
	bool Set(const char* key, const char* value);

	//获取key对应的value
	std::string Get(const char* key);

	//将一个string插入到List列表头部
	bool Lpush(const char* key, const char* value);

	//将一个string插入到List列表尾部
	bool Rpush(const char* key, const char* value);

	//将一个string插入到List列表头部
	bool Lpushb(const char* key, const char* value, size_t len);

	//将一个string插入到List列表尾部
	bool Rpushb(const char* key, const char* value, size_t len);

	//将一个int插入到List列表头部
	bool Lpush(const char* key, int value);

	//将一个int插入到List列表尾部
	bool Rpush(const char* key, int value);

	//移除并获取List队列的第一个元素
	std::string Lpop(const char* key);

	//移除并获取List队列的最后一个元素
	std::string Rpop(const char* key);

	//获取List队列的指定元素
	std::string Lindex(const char* key, int idx);

	//删除List队列的指定元素
	bool Lrem(const char* key, int cnt, const char* pval);

	//获取List队列长度
	long long GetListlen(const char* key);

	//设置一个hash值
	int SetHash(const char* key, const char* field, const char* value);

	//设置一个hash值
	std::string GetHash(const char* key, const char* field);

	//删除key对应域的所有数据
	bool DelAll(const char* key);
};

