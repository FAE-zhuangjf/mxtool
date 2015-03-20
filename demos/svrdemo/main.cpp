#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <string.h>

#include <net.h>

DWORD bytes_in_3_seconds = 0;

DWORD WINAPI thread_monitor(void *arg)
{
	FILE *output = NULL;
	fopen_s(&output, "plot.txt", "w+");
    int line = 0;
    int flush_interval = 0;
    
    while(1)
    {
        Sleep(100);

        fprintf(output, "%d %d\n", line++, bytes_in_3_seconds/3);

        bytes_in_3_seconds = 0;

        if(flush_interval++ >= 10)
        {
            fflush(output);
        }
    }

    fclose(output);
}


DWORD WINAPI thread_client(void *arg)
{
    int ret;

    int clientfd = (int)arg;
    
    PCHAR recvbuf = (PCHAR)malloc(1024);
    
    while(1)
    {
        ret = recv(clientfd, recvbuf, 1024, 0);
		if (ret > 0)
		{
			bytes_in_3_seconds += ret;
		}
        else
        {
            printf("client close\n");
            break;
        }
    }

    closesocket(clientfd);

    free(recvbuf);

    ExitThread(0);
}

int main()
{
	int svrfd;
	int clientfd;
	int ret;
	
	struct sockaddr_in svraddr;
	struct sockaddr_in clientaddr;

	int sin_size;

	net_init();

    svrfd = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == svrfd)
	{
		printf("socket create error!\n");
		WSACleanup();
		return -1;
	}

	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(8080);
	svraddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(svraddr.sin_zero), 0, sizeof(svraddr.sin_zero));

    ret = bind(svrfd, (struct sockaddr *)&svraddr, sizeof(struct sockaddr));
	if (-1 == ret)
	{
		printf("bind error!\n");
		WSACleanup();
		return -1;
	}

    ret = listen(svrfd, 10);
	if (-1 == ret)
	{
		printf("listen error!\n");
		WSACleanup();
		return -1;
	}

	sin_size = sizeof(struct sockaddr_in);

    CreateThread(NULL, 0, thread_monitor, NULL, 0, NULL);

    while(1)
    {
        clientfd = accept(svrfd, (struct sockaddr *)&clientaddr, &sin_size);

        printf("client addr: %s\n", inet_ntoa(clientaddr.sin_addr));

        if(-1 != clientfd)
        {
            CreateThread(NULL, 0, thread_client, (void*)clientfd, 0, NULL);
        }
    }

	WSACleanup();
	
	return 0;
}

