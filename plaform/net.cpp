#include <net.h>

#include <stdio.h>

#include <winsock.h>

int net_init(void)
{
    WSADATA WSAData;

	if (WSAStartup(MAKEWORD(1, 1), &WSAData))
	{
		printf("WSA init error!\n");
		WSACleanup();
		return -1;
	}

    return 0;
}

