#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>

#include "service-poke.h"

#define BUFFSIZE 1024

int service_poke(char *socket_file, char *service, int delay, char **error)
{
	struct sockaddr_un addr;
	int socket_fd;
	int size;
	int res = 0;
	char buffer[BUFFSIZE];
	service_poke_header *sph;
	sph = (service_poke_header*)buffer;
	memcpy(sph->header, "SP10", strlen("SP10"));
	sph->delay = GUINT64_TO_LE(delay);
	size = strlen(service);
	if(size > BUFFSIZE - sizeof(service_poke_header) - 1)
	{
		if(error)
		{
			*error = g_strdup("Service name too big");
		}
		return -1;
	}
	strcpy(buffer + sizeof(service_poke_header), service);

	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if(socket_fd < 0)
	{
		if(error)
		{
			*error = g_strdup("Failed to create socket");
		}
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	strcpy(addr.sun_path, socket_file);
	addr.sun_family = AF_UNIX;

	if(connect(socket_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) != 0)
	{
		if(error)
		{
			*error = g_strdup_printf("Failed to connect. %d", errno);
		}
		return -1;
	}

	size = write(socket_fd, buffer, sizeof(service_poke_header) + strlen(service) + 1);
	if(size < 1)
	{
		if(error)
		{
			*error = g_strdup_printf("Something happened while poking. %d", errno);
		}
		res = -1;
	}
	close(socket_fd);
	return res;
}
