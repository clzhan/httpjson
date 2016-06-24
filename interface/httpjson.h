#ifndef HTTP_JSON_H_
#define HTTP_JSON_H_

#ifdef HTTPJSON_EXPORTS
#define HTTPJSON_API __declspec(dllexport)
#else
#ifdef HTTPJSON_IMPORTS
#define HTTPJSON_API __declspec(dllimport)
#else 
#define HTTPJSON_API extern
#endif
#endif


#include <sstream>
using namespace std;



typedef struct CurrentDllSoType
{
	std::string version3;
	std::string id3;
	std::string appId;
	std::string clientModel;
	std::string clientVersion;


}SRCurrentDllSoType, *PSRCurrentDllSoType;

HTTPJSON_API int HttpGet(const std::string & strUrl, SRCurrentDllSoType *dlltype, std::string & key3);

#endif