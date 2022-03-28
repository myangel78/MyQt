#pragma once

#include <curl/curl.h>
#include <string>
#include <list>
#include <map>


class HttpClient
{

private:
	bool m_bIngoreSSL = false;
	int m_timeout = 1200;
	int m_iRetryTimes = 1;
	size_t m_dwTotalSize = 0;    // Size of the total data
	std::string m_strUserPWD;
	std::string m_httpPackge;
	std::string m_strCookie;
	std::list<std::string> m_listCookies;
	std::map<std::string, std::string> m_mapReqHeads;
	std::map<std::string, std::string> m_mapRspHeads;

private:
	curl_slist* AddHeadToCurl(CURL* pCurl, std::string& strHeader);

	void ProcessData(char* pBuffer, size_t nSize);
	void ProcessHead(void* pBuffer, size_t nSize);

	static size_t RecvDataCallBack(void* buffer, size_t size, size_t nmemb, void* stream);
	static size_t RecvHeadCallBack(void* buffer, size_t size, size_t nmemb, void* stream);

public:
	void SetIngorSSL(bool val) { m_bIngoreSSL = val; }
	void SetTimeOut(int val) { m_timeout = val; }
	void SetRetryTimes(int val) { m_iRetryTimes = val; }
	void SetPassword(const std::string& val) { m_strUserPWD = val; }
	void SetCookie(const std::string& val) { m_strCookie = val; }

	std::map<std::string, std::string>& GetReqHeader(void) { return m_mapReqHeads; }

	std::string GetHeader(const std::string& key) const;
	std::string GetCookie(const std::string& key) const;


	bool RequestGet(const std::string& strurl);
	bool RequestPost(const std::string& strurl, const std::string& strdata);
	bool RequestPostFiles(const std::string& strurl, const std::string& strdata, const std::string& strfiles);
	std::string GetReadString(size_t& sz) const;

};

