#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <string.h>

void speed_add(int bytes)
{
	static DWORD pretick = 0;
	static DWORD total3s = 0;

	static DWORD total_bytes = 0, test_cnt = 0;
	DWORD average_speed;
	static DWORD high_speed=0, low_speed=0;

	DWORD tmp_speed;

	total_bytes += bytes;

	total3s += bytes;

	if (pretick + 3000 < GetTickCount())
	{
		if (0 == pretick)
		{
			pretick = GetTickCount();
		}
		else
		{
			tmp_speed = (total3s >> 10) / ((GetTickCount() - pretick) / 1000);
			printf("speed >> %d kB/s\n", tmp_speed);

			test_cnt++;

			average_speed = (total3s >> 10) / ((GetTickCount() - pretick) / 1000);

			total3s = 0;
			pretick = GetTickCount();
		}
	}
}

int main()
{
	int iServerSock;
	int iClientSock;
	int ret;
	PCHAR recvbuf;
	struct sockaddr_in ServerAddr;
	struct sockaddr_in ClientAddr;

	unsigned char *start = NULL;

	recvbuf = (PCHAR)malloc(1024);

	int sin_size;

	WSADATA WSAData;

	if (WSAStartup(MAKEWORD(1, 1), &WSAData))//³õÊ¼»¯
	{
		printf("initializationing error!\n");
		WSACleanup();
		exit(0);
	}

	printf("WSA init ok\n");

	if ((iServerSock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("socket create error!\n");
		WSACleanup();
		exit(0);
	}

	printf("server created\n");

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(8080);
	ServerAddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero));

	if (bind(iServerSock, (struct sockaddr *)&ServerAddr, sizeof(struct sockaddr)) == -1)
	{
		printf("bind error!\n");
		WSACleanup();
		exit(0);
	}

	printf("server bind ok\n");

	if (listen(iServerSock, 10) == -1)
	{
		printf("listen error!\n");
		WSACleanup();
		exit(0);
	}

	printf("server listen ok\n");

	sin_size = sizeof(struct sockaddr_in);

	// suspend here
	iClientSock = accept(iServerSock, (struct sockaddr *)&ClientAddr, &sin_size);
	if (iClientSock == -1)
	{
		printf("accept error!\n");
		WSACleanup();
		exit(0);
	}
	else
	{
		int yes = 1;
		//setsockopt(iClientSock, IPPROTO_TCP, TCP_NODELAY, (PCHAR)&yes, sizeof(yes));
	}

	printf("server accept one client\n");
	printf("client: %s\n", inet_ntoa(ClientAddr.sin_addr));

	while (1)
	{
		ret = recv(iClientSock, recvbuf, 1024, 0);
		if (ret > 0)
		{
			//printf("recv %d bytes\n", ret);
			speed_add(ret);
		}
		else
		{
			printf("recv error\n");
			break;
		}
	}

	printf("server quit\n");

	WSACleanup();
	free(recvbuf);
	return 0;
}