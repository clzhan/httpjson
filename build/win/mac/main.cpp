
#include <ws2tcpip.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib,"ws2_32.lib")
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))


static int  _GetAdaptInfo(char *ip,char * mac)
{
	/* Declare and initialize variables */

	// It is possible for an adapter to have multiple
	// IPv4 addresses, gateways, and secondary WINS servers
	// assigned to the adapter.
	//
	// Note that this sample code only prints out the
	// first entry for the IP address/mask, and gateway, and
	// the primary and secondary WINS server for each adapter.

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	/* variables used to print DHCP time info */
	struct tm newtime;
	char buffer[32];

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		return -1;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL)
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return -1;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			if (strcmp(pAdapter->IpAddressList.IpAddress.String, ip) == 0)
			{
				printf("MAC: %02X%02X%02X%02X%02X%02X\n", pAdapter->Address[0], pAdapter->Address[1],
					pAdapter->Address[2], pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
				sprintf(mac, "%02X%02X%02X%02X%02X%02X", pAdapter->Address[0], pAdapter->Address[1],
					pAdapter->Address[2], pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);

				if (pAdapterInfo)
				{
					FREE(pAdapterInfo);
					pAdapterInfo = NULL;
				}
				return  0;
			}
			pAdapter = pAdapter->Next;
		}

	}

	if (pAdapterInfo)
	{
		FREE(pAdapterInfo);
		pAdapterInfo = NULL;
	}

	return -1;
}

static int _GetIpAddrTabletFromIterface(int index, char * local_ip)
{
	int i;

	PMIB_IPADDRTABLE pIPAddrTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	IN_ADDR IPAddr;
	LPVOID lpMsgBuf;

	if (local_ip == NULL)
	{
		return -1;
	}

	pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(sizeof(MIB_IPADDRTABLE));

	if (pIPAddrTable)
	{
		if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==
			ERROR_INSUFFICIENT_BUFFER)
		{
			FREE(pIPAddrTable);
			pIPAddrTable = (MIB_IPADDRTABLE *)MALLOC(dwSize);

		}
		if (pIPAddrTable == NULL)
		{
			return -1;
		}
	}
	if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR)
	{
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // Default language
			(LPTSTR)& lpMsgBuf, 0, NULL))
		{
			LocalFree(lpMsgBuf);
		}
	}

	memset(local_ip, '\0', strlen(local_ip));
	for (i = 0; i < (int)pIPAddrTable->dwNumEntries; i++)
	{
		if (pIPAddrTable->table[i].dwIndex == index)
		{
			IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
			strcpy(local_ip, inet_ntoa(IPAddr));
			break;
		}
	}

	if (i == (int)pIPAddrTable->dwNumEntries)
	{
		if (pIPAddrTable)
		{
			FREE(pIPAddrTable);
			pIPAddrTable = NULL;
		}
		return -1;
	}

	if (pIPAddrTable)
	{
		FREE(pIPAddrTable);
		pIPAddrTable = NULL;
	}
	return 0;
}






static int _GetIpFromAdapterVPN(char *ip)
{

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	/* variables used to print DHCP time info */
	struct tm newtime;
	char buffer[32];
	errno_t error;

	if (ip == NULL)
	{
		return -1;
	}

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		return -1;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL)
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return -1;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			//µ±Ç°ÊÇvpn
			if (pAdapter->Type == MIB_IF_TYPE_PPP ||
				strstr(pAdapter->Description, "VPN") != NULL ||
				strstr(pAdapter->Description, "vpn") != NULL)
			{
				if (pAdapter->IpAddressList.IpAddress.String != NULL &&
					strlen(pAdapter->IpAddressList.IpAddress.String) > 0 &&
					strcmp(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
				{
					strcpy(ip, pAdapter->IpAddressList.IpAddress.String);
					if (pAdapterInfo)
					{
						FREE(pAdapterInfo);
						pAdapter = NULL;
					}

					return 0;
				}
			}
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
	{
		FREE(pAdapterInfo);
		pAdapter = NULL;
	}
		
	return -1;
}

int   SRE_GetBestIpAddr(char * local_ip)
{
	int ret = -1;

	if (local_ip == NULL)
	{
		return -1;
	}
	memset(local_ip, '\0', strlen(local_ip));
	ret = _GetIpFromAdapterVPN(local_ip);
	if (ret != 0)
	{
		IPAddr dwDestAddr = inet_addr("10.10.10.10");
		DWORD dwBestIfIndex = -1;
		DWORD dwRes = GetBestInterface(dwDestAddr, &dwBestIfIndex);
		if (dwRes != ERROR_SUCCESS)
		{
			return -1;
		}
		return _GetIpAddrTabletFromIterface(dwBestIfIndex, local_ip);
	}
	return -1;
}

int SRE_GetIpAddrInfo(char *mac)
{
	int ret = -1;
	char local_ip[30] = { 0 };

	memset(local_ip, '\0', strlen(local_ip));

	ret = _GetIpFromAdapterVPN(local_ip);
	if (ret == 0)
	{
		if (_GetAdaptInfo(local_ip, mac) != 0)
		{
			return -1;
		}
	}
	else
	{
		if (SRE_GetBestIpAddr(local_ip) != 0)
		{
			return -1;
		}
		else
		{
			if (_GetAdaptInfo(local_ip, mac) != 0)
			{
				return -1;
			}
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{

	char mac[200] = { 0 };
	if (SRE_GetIpAddrInfo(mac) != 0)
	{
		printf("mac get failed\n");
	}
	else
	{
		printf("The Mac Address is : %s   \n", mac);
		//TODO
	}

	
	return 0;
}