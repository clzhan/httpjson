
#include <string>
#include <iostream>
using namespace std;

#include <sstream>
#include <cstring>
#include<dlfcn.h>

#include "httpjson.h"

	  
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
	  #include <arpa/inet.h>

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

static int GetWlanMac(char *current_mac, int current_mac_len)
{
	if(!current_mac && current_mac_len < 12)
	{
		return -1;
	}

	struct ifreq s;
	int socketfd = -1;

	socketfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	if(socketfd < 0)
	{
		return -1;
	}


	const char *eth_name = "wlan0";

	strcpy(s.ifr_name, eth_name);

	if (ioctl(socketfd, SIOCGIFHWADDR, &s) < 0) 
	{
		close(socketfd);
		return -1;
	}
	sprintf(current_mac, "%02X%02X%02X%02X%02X%02X", (unsigned char)s.ifr_addr.sa_data[0], 
			(unsigned char)s.ifr_addr.sa_data[1],
			(unsigned char)s.ifr_addr.sa_data[2], 
			(unsigned char)s.ifr_addr.sa_data[3], 
			(unsigned char)s.ifr_addr.sa_data[4], 
			(unsigned char)s.ifr_addr.sa_data[5]);
	printf("current_mac %s\n", current_mac);

	close(socketfd);

	if(strlen(current_mac) < 5)
	{
		return -1;
	}

	return 0;
}
//WARNING! NO HANDLING FOR VERY_LONG COMMAND READS YET
//IMSI
static int GetLikeMac(char* like_mac, int like_mac_len) 
{

	if(!like_mac && like_mac_len < 12)
	{
		return -1;
	}


	char cmd_res_line[256];
	char total_cmd_res[25600];

	FILE* pipe = NULL;

	memset(cmd_res_line, 0, 256*sizeof(char));
	memset(total_cmd_res, 0, 25600 * sizeof(char));


	const char * mycmd = "service call iphonesubinfo 3";

	pipe = popen(mycmd, "r");
	if (!pipe) 
	{
		return -1;
	}



	while(!feof(pipe)) 
	{
		if(fgets(cmd_res_line, 256, pipe) != NULL)
		{
			//TODO: add handling for long command reads...
			strcat(total_cmd_res,cmd_res_line);
		}
	}

	pclose(pipe);

	char imsi[15];
	memset(imsi, 0, 15 * sizeof(char));

	if(strlen(total_cmd_res) > 210)
	{
		imsi[0] = total_cmd_res[75];
		imsi[1] = total_cmd_res[77];
		imsi[2] = total_cmd_res[79];
		imsi[3] = total_cmd_res[81];
		imsi[4] = total_cmd_res[136];
		imsi[5] = total_cmd_res[138];
		imsi[6] = total_cmd_res[140];
		imsi[7] = total_cmd_res[142];
		imsi[8] = total_cmd_res[144];
		imsi[9] = total_cmd_res[146];
		imsi[10] = total_cmd_res[148];
		imsi[11] = total_cmd_res[150];
		imsi[12] = total_cmd_res[205];
		imsi[13] = total_cmd_res[207];
		imsi[14] = total_cmd_res[209];
		printf("imsi: %s len %d\r\n",imsi,strlen(imsi));

		like_mac[0] = imsi[3];
		like_mac[1] = imsi[4];
		like_mac[2] = imsi[5];
		like_mac[3] = imsi[6];
		like_mac[4] = imsi[7];
		like_mac[5] = imsi[8];
		like_mac[6] = imsi[9];
		like_mac[7] = imsi[10];
		like_mac[8] = imsi[11];
		like_mac[9] = imsi[12];
		like_mac[10] = imsi[13];
		like_mac[11] = imsi[14];

		printf("like_mac: %s len %d\r\n",like_mac,strlen(like_mac));

	}
	else
	{
		return -1;
	}

	return 0;

}
static int GetMac(char *mac, int mac_len)
{
	if(!mac && mac_len < 12)
	{
		return -1;
	}

	// first get android wlan0 mac
	if(GetWlanMac(mac, mac_len) >= 0)
	{
		return 0;
	}
	//second get imsi info

	if(GetLikeMac(mac, mac_len) >= 0)
	{
		return 0;
	}

	return -1;

}



int main(int argc, char* argv[])
{

	std:string url = "http://103.249.255.237/utils/v1/utils/getkey3";

	char mac[13] = {0};
	memset(mac, 0, 64*sizeof(char));

	if(GetMac(mac, 13) >= 0)
	{
		printf("mac %d\n", mac);
		if(strlen(mac) > 5)
		{
			SRCurrentDllSoType dlltype;

			dlltype.version3 = "26";
			dlltype.id3 = mac;
			//dlltype.id3 = "78F5FD5CE9E3";
			dlltype.appId = "12345";
			dlltype.clientModel = "android";
			dlltype.clientVersion = "1.1";

			char key3[64] ={0};
			int ret = HttpGet(url, &dlltype, key3, 64);

			printf("ret = %d\n", ret);
			printf("key3 = %s\n", key3);

		}

	}
	

	return 0;
}
