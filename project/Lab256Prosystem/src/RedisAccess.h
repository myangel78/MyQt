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

	//����key��Ӧ��value
	bool Set(const char* key, const char* value);

	//��ȡkey��Ӧ��value
	std::string Get(const char* key);

	//��һ��string���뵽List�б�ͷ��
	bool Lpush(const char* key, const char* value);

	//��һ��string���뵽List�б�β��
	bool Rpush(const char* key, const char* value);

	//��һ��string���뵽List�б�ͷ��
	bool Lpushb(const char* key, const char* value, size_t len);

	//��һ��string���뵽List�б�β��
	bool Rpushb(const char* key, const char* value, size_t len);

	//��һ��int���뵽List�б�ͷ��
	bool Lpush(const char* key, int value);

	//��һ��int���뵽List�б�β��
	bool Rpush(const char* key, int value);

	//�Ƴ�����ȡList���еĵ�һ��Ԫ��
	std::string Lpop(const char* key);

	//�Ƴ�����ȡList���е����һ��Ԫ��
	std::string Rpop(const char* key);

	//��ȡList���е�ָ��Ԫ��
	std::string Lindex(const char* key, int idx);

	//ɾ��List���е�ָ��Ԫ��
	bool Lrem(const char* key, int cnt, const char* pval);

	//��ȡList���г���
	long long GetListlen(const char* key);

	//����һ��hashֵ
	int SetHash(const char* key, const char* field, const char* value);

	//����һ��hashֵ
	std::string GetHash(const char* key, const char* field);

	//ɾ��key��Ӧ�����������
	bool DelAll(const char* key);
};

