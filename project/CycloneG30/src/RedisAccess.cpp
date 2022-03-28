//#include "stdafx.h"
#include "RedisAccess.h"

#include <algorithm>

#include <Log.h>


RedisAccess::RedisAccess()
	: m_iAccess(0)
	, m_bConnected(false)
	, m_iPort(6379)
	, m_indexDB(-1)
	, m_pContext(nullptr)
	, m_pReply(nullptr)
{
}


RedisAccess::~RedisAccess()
{
	Disconnect();
}

bool RedisAccess::IsCmdError(void)
{
	if (nullptr == m_pReply)
	{
        LOGE("Redis Server has gone away!={}", m_pContext->errstr);
		Connecting();  //若_reply返回NULL，默认为连接断开
		return true;
	}
#if 1
	else if (m_pReply->type == REDIS_REPLY_ERROR)
	{
		if (m_pReply->str != nullptr)
		{
            LOGE(m_pReply->str);
			freeReplyObject(m_pReply);
			m_pReply = nullptr;
			return true;
		}
	}
	else if (m_pReply->type == REDIS_REPLY_STATUS)
	{
		if (m_pReply->str != nullptr)
		{
			std::string str = m_pReply->str;
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
			//str.compare("OK");
			if (str != "OK")
			{
                LOGE(m_pReply->str);
				freeReplyObject(m_pReply);
				m_pReply = nullptr;
				return true;
			}
		}
	}
#endif
	return false;
}

bool RedisAccess::SetAccessParam(const char * pHost, int port, const char * ppsw)
{
	if (pHost == nullptr)
	{
        LOGE("Redis host is null!!!");
		return false;
	}
	m_strHost = pHost;
	m_iPort = port;
	if (ppsw != nullptr)
	{
		m_strPsw = ppsw;
	}

	return true;
}

bool RedisAccess::Connecting(void)
{
	Disconnect();
	if (m_strHost.size() <= 0)
	{
        LOGE("redis host is empty!!!");
		return false;
	}
	m_pContext = redisConnect(m_strHost.c_str(), m_iPort);
	if (m_pContext->err)
	{
        LOGE("Connect to Redis Server failed!!!={}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = nullptr;
		return false;
	}
	if (m_strPsw.size() > 0)
	{
		m_pReply = (redisReply*)redisCommand(m_pContext, "AUTH %s", m_strPsw.c_str());
		if (IsCmdError())
		{
			return false;
		}
	}
	m_bConnected = true;

	return true;
}

void RedisAccess::Disconnect(void)
{
	//if (m_pReply != nullptr)
	//{
	//	freeReplyObject(m_pReply);
	//	m_pReply = nullptr;
	//}
	if (m_pContext != nullptr)
	{
		redisFree(m_pContext);
		m_pContext = nullptr;
	}
	m_bConnected = false;
}

bool RedisAccess::selectDB(int index)
{
	if (index == m_indexDB)
	{
		return true;
	}

	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}
	m_pReply = (redisReply*)redisCommand(m_pContext, "SELECT %d", index);
	if (IsCmdError())
	{
		return false;
	}
	m_indexDB = index;
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

bool RedisAccess::Set(const char * key, const char * value)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "SET %s %s", key, value);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

std::string RedisAccess::Get(const char * key)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return "";
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "GET %s", key);
	if (IsCmdError())
	{
		return "";
	}
	std::string ret;
	if (nullptr != m_pReply->str)
	{
		ret = m_pReply->str;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return ret;
}

bool RedisAccess::Lpush(const char * key, const char * value)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "LPUSH %s %s", key, value);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

bool RedisAccess::Rpush(const char * key, const char * value)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "RPUSH %s %s", key, value);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

bool RedisAccess::Lpushb(const char* key, const char* value, size_t len)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "LPUSH %s %b", key, value, len);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

bool RedisAccess::Rpushb(const char* key, const char* value, size_t len)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "RPUSH %s %b", key, value, len);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

bool RedisAccess::Lpush(const char* key, int value)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "LPUSH %s %X", key, value);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

bool RedisAccess::Rpush(const char* key, int value)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "RPUSH %s %X", key, value);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

std::string RedisAccess::Lpop(const char * key)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return "";
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "LPOP %s", key);
	if (IsCmdError())
	{
		return "";
	}
	std::string ret;
	if (nullptr != m_pReply->str)
	{
		ret = std::string(m_pReply->str, m_pReply->len);
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return ret;
}

std::string RedisAccess::Rpop(const char * key)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return "";
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "RPOP %s", key);
	if (IsCmdError())
	{
		return "";
	}
	std::string ret;
	if (nullptr != m_pReply->str)
	{
		ret = std::string(m_pReply->str, m_pReply->len);
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return ret;
}

std::string RedisAccess::Lindex(const char * key, int idx)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return "";
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "LINDEX %s %d", key, idx);
	if (IsCmdError())
	{
		return "";
	}
	std::string ret;
	if (nullptr != m_pReply->str)
	{
		ret = m_pReply->str;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return ret;
}

bool RedisAccess::Lrem(const char * key, int cnt, const char* pval)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "LREM %s %d %s", key, cnt, pval);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}

long long RedisAccess::GetListlen(const char * key)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return 0;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "LLEN %s", key);
	if (IsCmdError())
	{
		return 0;
	}
	long long length = m_pReply->integer;
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return length;
}

int RedisAccess::SetHash(const char* key, const char* field, const char* value)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "hset %s %s %s", key, field, value);
	if (IsCmdError())
	{
		return false;
	}
	int ret = m_pReply->integer;
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return ret;
}

std::string RedisAccess::GetHash(const char* key, const char* field)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return "";
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "hget %s %s", key, field);
	if (IsCmdError())
	{
		return "";
	}
	std::string ret;
	if (nullptr != m_pReply->str)
	{
		ret = m_pReply->str;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return ret;
}

bool RedisAccess::DelAll(const char * key)
{
	std::unique_lock<std::mutex> lck(m_mutexTask);
	if (!m_bConnected && !Connecting())
	{
		return false;
	}

	m_pReply = (redisReply*)redisCommand(m_pContext, "DEL %s", key);
	if (IsCmdError())
	{
		return false;
	}
	freeReplyObject(m_pReply);
	m_pReply = nullptr;

	return true;
}
