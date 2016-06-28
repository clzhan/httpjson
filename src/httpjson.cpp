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

int HttpGet(const std::string & strUrl, SRCurrentDllSoType *dlltype, char* key3, int key3Len)
{

	if(key3 == NULL && key3Len < 64)
	{
		return -1;
	}
	
	if (strUrl.empty() && dlltype->version3.empty() && dlltype->id3.empty())
	{
		printf("param error\n");
		return -1;
	}
	printf("version3 :%s\n", dlltype->version3.c_str());
	printf("id3 :%s\n", dlltype->id3.c_str());
	printf("appid :%s\n", dlltype->appId.c_str());


	CURL *curl;
	CURLcode res;

	std::string  strResponse;


	curl = curl_easy_init();
	if (curl)
	{
	
		std::string strparam6 = "?version3=" + dlltype->version3;
		strparam6 = strparam6 + "&id3=" + dlltype->id3;
		strparam6 = strparam6 + "&appId=" + dlltype->appId;

		string currentAllurl = strUrl + strparam6;

		printf("current url  %s\n", currentAllurl.c_str());
		
		curl_easy_setopt(curl, CURLOPT_URL, currentAllurl.c_str());
		//struct curl_slist *headers = NULL; /* init to NULL is important */
		//headers = curl_slist_append(headers, strparam1.c_str());
		//headers = curl_slist_append(headers, strparam2.c_str());
		//headers = curl_slist_append(headers, strparam3.c_str());
		//headers = curl_slist_append(headers, strparam4.c_str());
		//headers = curl_slist_append(headers, strparam5.c_str());
		///* pass our list of custom made headers */
		//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);


		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);


		res = curl_easy_perform(curl);

		//curl_slist_free_all(headers); /* free the header list */
		curl_easy_cleanup(curl);

		printf("get strResponse   = %s\n", strResponse.data());
	}
	else
	{
		return -1;
	}

	if(strResponse.empty())
	{
		return -1;
	}

	Json::Reader reader;
	Json::Value value;
	

	if (reader.parse(strResponse, value))
	{

		Json::Value data = value["data"];
		std::string code = value["code"].asString();


		if (code.compare("200") != 0)
		{
			return -1;
		}

		std::string version3_from_server = data["version3"].asString();

		std::string id3_from_server = data["id3"].asString();

		std::string key3_from_server = data["key3"].asString();

	
		if(key3_from_server.empty())
		{
			return -1;
		}
		strncpy(key3, key3_from_server.c_str(), key3_from_server.size());

		printf("version3_from_server %s\n", version3_from_server.c_str());
		printf("id3_from_server %s\n", id3_from_server.c_str());
		printf("key3_from_server %s\n", key3_from_server.c_str());

	}
	else
	{
		return -1;
	}

	
	return 0;
}


