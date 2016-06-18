#include "httpjson.h"
#include "httpClient.h"
#include "JsonBase.h"

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return nmemb;
}

int HttpGet(const std::string & strUrl, SRCurrentDllSoType dlltype, std::string & key3)
{
	if (strUrl.empty() && dlltype.version3.empty() && dlltype.id3.empty())
	{
		printf("param error\n");
		return -1;
	}
	printf("version3 :%s\n", dlltype.version3.c_str());
	printf("id3 :%s\n", dlltype.id3.c_str());
	printf("appid :%s\n", dlltype.appId.c_str());

	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl)
	{
		std::string strparam1 = "version3:" + dlltype.version3;
		std::string strparam2 = "id3:" + dlltype.id3;
		std::string strparam3 = "appId:" + dlltype.appId;
		std::string strparam4 = "clientModel:" + dlltype.clientModel;
		std::string strparam5 = "clientVersion:" + dlltype.clientVersion;

	
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
		struct curl_slist *headers = NULL; /* init to NULL is important */
		headers = curl_slist_append(headers, strparam1.c_str());
		headers = curl_slist_append(headers, strparam2.c_str());
		headers = curl_slist_append(headers, strparam3.c_str());
		headers = curl_slist_append(headers, strparam4.c_str());
		headers = curl_slist_append(headers, strparam5.c_str());
		/* pass our list of custom made headers */
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&key3);
		res = curl_easy_perform(curl);

		curl_slist_free_all(headers); /* free the header list */
		curl_easy_cleanup(curl);

		printf("get key3   = %s\n", key3.data());
	}
	else
	{
		return -1;
	}

	
	return 0;
}


