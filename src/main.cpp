
#include <string>
#include <iostream>
using namespace std;

#include <sstream>
#include <cstring>

#include "httpjson.h"

//去除首位空格
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

int main(int argc, char* argv[])
{

	std:string url = "http://103.249.255.237/utils/v1/utils/getkey3";

	SRCurrentDllSoType dlltype;

	dlltype.version3 = "2.6";
	dlltype.id3 = "78:2b:cb:47:69:a8";
	dlltype.appId = "12345";
	dlltype.clientModel = "android";
	dlltype.clientVersion = "1.1";
	string key3;
	int ret = HttpGet(url, dlltype, key3);

	printf("ret = %d\n", ret);

	return 0;
}