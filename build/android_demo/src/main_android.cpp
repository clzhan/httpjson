
#include <string>
#include <iostream>
using namespace std;

#include <sstream>
#include <cstring>
#include<dlfcn.h>

#include "httpjson.h"

string& trim(string &s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}
// MacAddress.cpp : Defines the entry point for the console application.
//


int main(int argc, char* argv[])
{

	std:string url = "http://103.249.255.237/utils/v1/utils/getkey3";

	SRCurrentDllSoType dlltype;

	dlltype.version3 = "26";
	dlltype.id3 = "408D5C84E4D7";
	//dlltype.id3 = "78F5FD5CE9E3";
	dlltype.appId = "12345";
	dlltype.clientModel = "android";
	dlltype.clientVersion = "1.1";


	//int a,b;
	//void *p;
	//int (*func)(const std::string & strUrl, SRCurrentDllSoType *dlltype, std::string & key3);
	//p=dlopen("./libhttp-json.so",RTLD_NOW);
	//func=dlsym(p,"HttpGet");
	//printf("%d与%d相比，%d为大数。\n",a,b,(*func)(a,b));

	
	char key3[64] ={0};


	int ret = HttpGet(url, &dlltype, key3, 64);

	printf("ret = %d\n", ret);
	printf("key3 = %s\n", key3);
	//dlclose(p);


	return 0;
}
