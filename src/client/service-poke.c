#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <glib.h>

#include "service-poke.h"

#define BUFFSIZE 1024

int main(int argc, char *argv[])
{
	int delay = 0;
	int res = 0;
	gchar *socket_file = NULL;
	gchar *service = NULL;
	GError *error = NULL;
	char *error_str;
	GOptionContext *context;
	GOptionEntry option_entries[] =
	{
		{"socket", 'f', 0, G_OPTION_ARG_FILENAME, &socket_file, "Socket filename to use.", ""},
		{"service", 's', 0, G_OPTION_ARG_STRING, &service, "Service to poke.", ""},
		{"delay", 'd', 0, G_OPTION_ARG_INT, &delay, "Keep in pending for n seconds unless a newer poke comes in.", ""},
		{NULL}
	};
	context = g_option_context_new("- Service Poke Client");
	g_option_context_add_main_entries(context, option_entries, NULL);
	res = g_option_context_parse(context, &argc, &argv, &error);
	if(!res)
	{
		fprintf(stderr, "Failed to parse arguments.\n");
		exit(-1);
	}
	if(!socket_file)
	{
		fprintf(stderr, "You must specify the socket file with -f\n");
		exit(-1);
	}
	if(!service)
	{
		fprintf(stderr, "You must specify the service with -s\n");
		exit(-1);
	}

	res = service_poke(socket_file, service, delay, &error_str);
	if(res)
	{
		fprintf(stderr, "%s\n", error_str);
	}
	return res;
}
