#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <string.h>
#include <stdarg.h>

#include <net.h>

#include "info.h"
#include "line.h"
#include "cmd.h"

int main()
{
	int ret;

    char linebuf[256];

    char cmdname[16];
    
    show_title();

	net_init();

    while(1)
    {
		int i;

        printf(">>");

        ret = read_line(linebuf, sizeof(linebuf));

        if(ret <= 0)
        {
            continue;
        }

        sscanf_s(linebuf, "%s ", cmdname, sizeof(cmdname));
		
        for(i=0; i<CMD_MAX; i++)
        {
			if (0 == cmd[i].name[0])
			{
				continue;
			}

            char *ptr = strstr(linebuf, cmd[i].name);
			if (NULL != ptr)
            {
                cmd[i].func();
            }
        }

		if (CMD_MAX != i)
		{
			continue;
		}

		for (i = 0; i < CMD_MAX; i++)
		{
			printf("%s\n", cmd[i].name);
		}
    }

	WSACleanup();
	
	return 0;
}

