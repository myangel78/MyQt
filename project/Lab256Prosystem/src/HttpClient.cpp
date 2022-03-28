#include "HttpClient.h"

#include <Log.h>

//#ifdef _DEBUG
//#pragma comment(lib, "libcurld.lib")
//#else
//#pragma comment(lib, "libcurl.lib")
//#endif


curl_slist* HttpClient::AddHeadToCurl(CURL* pCurl, std::string& strHeader)
{
	struct curl_slist* headers = nullptr;
	std::string strHead;
	for (auto&& ite : m_mapReqHeads)
	{
		strHead = ite.first + ": " + ite.second;
		strHeader += strHead;
		strHeader += ";";
		headers = curl_slist_append(headers, strHead.c_str());
	}
	CURLcode res = curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);
	if (CURLE_OK == res)
	{
		return headers;
	}
	return nullptr;
}

void HttpClient::ProcessData(char* pBuffer, size_t nSize)
{
	if (nSize > 0)
	{
		m_httpPackge.append(pBuffer, nSize);
		m_dwTotalSize += nSize;
	}
}

void HttpClient::ProcessHead(void* pBuffer, size_t nSize)
{
	std::string temp = (char*)pBuffer;
	std::string::size_type pos = temp.find(":");

	if (pos != std::string::npos)
	{
		std::string strKey = temp.substr(0, pos);
		std::string strValue = temp.substr(pos + 1);
		if (strKey != "Set-Cookie")
		{
			m_mapRspHeads[strKey] = strValue;
		}
		else
		{
			std::string::size_type pos2 = strValue.find(" ");
			if (pos2 != std::string::npos)
			{
				strValue = strValue.substr(pos2 + 1);
			}

			pos2 = strValue.find("\r");
			if (pos2 != std::string::npos)
			{
				strValue = strValue.substr(0, pos2);
			}
			m_listCookies.push_back(strValue);
		}

	}
}

size_t HttpClient::RecvDataCallBack(void* buffer, size_t size, size_t nmemb, void* stream)
{
	HttpClient* pWrapper = (HttpClient*)stream;
	pWrapper->ProcessData((char*)buffer, size * nmemb);
	return size * nmemb;
}

size_t HttpClient::RecvHeadCallBack(void* buffer, size_t size, size_t nmemb, void* stream)
{
	HttpClient* pWrapper = (HttpClient*)stream;
	pWrapper->ProcessHead(buffer, size * nmemb);
	return size * nmemb;
}

bool HttpClient::RequestGet(const std::string& strurl)
{
	CURL* curl = curl_easy_init();
	if (curl == nullptr)
	{
		LOGE("Failed to init libcurl. getURL={}", strurl.c_str());
		return false;
	}

	CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, strurl.c_str());
	std::string strHeader;
	curl_slist* pHead = AddHeadToCurl(curl, strHeader);

	curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_timeout);
	//curl_easy_setopt(curl, CURLOPT_HTTPHEADER , 1);

	//forbid the timeout singnal.for this can cause a thread safe issue
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

	if (m_strCookie.size() > 0)
	{
		res = curl_easy_setopt(curl, CURLOPT_COOKIE, m_strCookie.c_str());
	}
	//* we want to use our own read function */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, RecvDataCallBack);

	//* pointer to pass to our read function */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, RecvHeadCallBack);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);

	if (!m_bIngoreSSL)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	}

	if (m_strUserPWD.size() > 0)
	{
		curl_easy_setopt(curl, CURLOPT_USERPWD, m_strUserPWD.c_str());
	}

	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  //CURLOPT_VERBOSE 这个选项很常用 用来在屏幕上显示对服务器相关操作返回的信息

	//res = curl_easy_perform(curl);
	for (int i = 0; i < m_iRetryTimes; i++)
	{
		res = curl_easy_perform(curl);
		if (CURLE_OK == res)
		{
			break;
		}
		else
		{
			LOGE("GETcurl_error, res={}, errmsg={}, times={}", res, curl_easy_strerror(res), i);
		}
	}
	curl_easy_cleanup(curl);
	curl_slist_free_all(pHead);
	if (CURLE_OK != res)
	{
		LOGE("GETcurl_error, res={}, URL={}", res, strurl.c_str());
		return false;
	}

	return true;
}

bool HttpClient::RequestPost(const std::string& strurl, const std::string& strdata)
{
	CURL* curl = curl_easy_init();
	if (curl == nullptr)
	{
		LOGE("Failed to init libcurl. postURL={}, DATA={}", strurl.c_str(), strdata.c_str());
		return false;
	}

	std::string strHeader;
	curl_slist* pHead = AddHeadToCurl(curl, strHeader);

	CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, strurl.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_timeout);
	//curl_easy_setopt(curl, CURLOPT_HTTPHEADER , 1);

	//forbid the timeout singnal.for this can cause a thread safe issue
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	curl_easy_setopt(curl, CURLOPT_POST, 1);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strdata.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strdata.size());

	if (m_strCookie.size() > 0)
	{
		res = curl_easy_setopt(curl, CURLOPT_COOKIE, m_strCookie.c_str());
	}
	//* we want to use our own read function */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, RecvDataCallBack);

	//* pointer to pass to our read function */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, RecvHeadCallBack);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);

	if (!m_bIngoreSSL)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	}

	if (m_strUserPWD.size() > 0)
	{
		curl_easy_setopt(curl, CURLOPT_USERPWD, m_strUserPWD.c_str());
	}

	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  //CURLOPT_VERBOSE 这个选项很常用 用来在屏幕上显示对服务器相关操作返回的信息

	//res = curl_easy_perform(curl);
	for (int i = 0; i < m_iRetryTimes; i++)
	{
		res = curl_easy_perform(curl);
		if (CURLE_OK == res)
		{
			break;
		}
		else
		{
			LOGE("POSTcurl_error, res={}, errmsg={}, times={}", res, curl_easy_strerror(res), i);
		}
	}
	curl_easy_cleanup(curl);
	curl_slist_free_all(pHead);
	if (CURLE_OK != res)
	{
		LOGE("POSTcurl_error, res={}, URL={}", res, strurl.c_str());
		return false;
	}

	return true;
}

bool HttpClient::RequestPostFiles(const std::string& strurl, const std::string& strdata, const std::string& strfiles)
{
	CURL* curl = curl_easy_init();
	if (curl == nullptr)
	{
		LOGE("Failed to init libcurl. postfilesURL={}, DATA={}", strurl.c_str(), strdata.c_str());
		return false;
	}

	std::string strHeader;
	curl_slist* pHead = AddHeadToCurl(curl, strHeader);

	CURLcode res = curl_easy_setopt(curl, CURLOPT_URL, strurl.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_timeout);
	//curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

	//forbid the timeout singnal.for this can cause a thread safe issue
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");

	curl_mime* mime;
	curl_mimepart* part;
	mime = curl_mime_init(curl);

	part = curl_mime_addpart(mime);
	curl_mime_name(part, "mailDetail");
	curl_mime_data(part, strdata.c_str(), strdata.size());//CURL_ZERO_TERMINATED
	if (!strfiles.empty())
	{
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "files");
		curl_mime_filedata(part, strfiles.c_str());
	}
	res = curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

	if (m_strCookie.size() > 0)
	{
		res = curl_easy_setopt(curl, CURLOPT_COOKIE, m_strCookie.c_str());
	}
	//* we want to use our own read function */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, RecvDataCallBack);

	//* pointer to pass to our read function */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, RecvHeadCallBack);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);

	if (!m_bIngoreSSL)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	}

	if (m_strUserPWD.size() > 0)
	{
		curl_easy_setopt(curl, CURLOPT_USERPWD, m_strUserPWD.c_str());
	}

	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  //CURLOPT_VERBOSE 这个选项很常用 用来在屏幕上显示对服务器相关操作返回的信息

	//res = curl_easy_perform(curl);
	for (int i = 0; i < m_iRetryTimes; i++)
	{
		res = curl_easy_perform(curl);
		if (CURLE_OK == res)
		{
			break;
		}
		else
		{
			LOGE("POSTFILEScurl_error, res={}, errmsg={}, times={}", res, curl_easy_strerror(res), i);
		}
	}
	curl_mime_free(mime);

	curl_easy_cleanup(curl);
	curl_slist_free_all(pHead);
	if (CURLE_OK != res)
	{
		LOGE("POSTFILEScurl_error, res={}, URL={}", res, strurl.c_str());
		return false;
	}

	return true;
}

std::string HttpClient::GetReadString(size_t& sz) const
{
	sz = m_dwTotalSize;
	return m_httpPackge;
}

std::string HttpClient::GetHeader(const std::string& key) const
{
	const auto ite = m_mapRspHeads.find(key);
	if (ite != m_mapRspHeads.end())
	{
		return ite->second;
	}
	return std::string();
}

std::string HttpClient::GetCookie(const std::string& key) const
{
	for (auto&& ite : m_listCookies)
	{
		std::string::size_type pos = ite.find(key);
		if (pos != std::string::npos)
		{
			return ite;
		}
	}
	return std::string();
}
