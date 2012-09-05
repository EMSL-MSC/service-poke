#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>
#include <fcntl.h>

#include "service-poke.h"

#define BUFFSIZE 1024

typedef struct
{
	int running;
	int pending;
	pid_t pid;
	gchar *command;
	struct timeval timeout;
} entry;

GHashTable *commands;

int debug;

static void do_execute(entry *e)
{
	if(debug > 0)
	{
		printf("Executing %s\n", e->command);
	}
	e->running = 1;
	e->pending = 0;
	e->pid = fork();
	if(e->pid < 0)
	{
		fprintf(stderr, "Failed to fork\n");
	}
	else if(e->pid > 0)
	{
		if(debug > 0)
		{
			printf("I'm the parent\n");
		}
	}
	else
	{
		execl("/bin/bash", "/bin/bash", "-c", e->command, (char*)NULL);
		fprintf(stderr, "Failed to execute child. %s\n", e->command);
		exit(-1);
	}
}

static void sigchld_handler_foreach(gpointer key, gpointer value, gpointer user_data)
{
	entry *e = (entry*)value;
	pid_t pid = GPOINTER_TO_INT(user_data);
	if(e->pid == pid)
	{
		e->running = 0;
		if(e->pending && (e->timeout.tv_sec == 0 || e->timeout.tv_sec <= time(NULL)))
		{
			do_execute(e);
		}
	}
}

static void sigchld_handler(int sig)
{
	pid_t pid;
	while((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
		g_hash_table_foreach(commands, sigchld_handler_foreach, GINT_TO_POINTER(pid));
	}
}

static void timeout_foreach(gpointer key, gpointer value, gpointer user_data)
{
	time_t now = time(NULL);
	entry *e = (entry*)value;
	struct timeval *timeout = (struct timeval*)user_data;
	if(e->pending)
	{
		if(e->running == 0 && (e->timeout.tv_sec == 0 || e->timeout.tv_sec <= now))
		{
			do_execute(e);
		}
		if(e->timeout.tv_sec != 0 && timeout->tv_sec > e->timeout.tv_sec)
		{
			timeout->tv_sec = e->timeout.tv_sec - now;
		}
	}
}

int main(int argc, char *argv[])
{
	time_t tmptime;
	struct timeval timeout;
	sigset_t child_block_mask;
	FILE *pidfile;
	socklen_t alen;
	struct sockaddr_un addr;
	int socket_fd;
	int connection_fd;
	int size;
	char buffer[BUFFSIZE];
	GKeyFile *keyfile;
	gsize gs;
	gchar **groups;
	int i;
	int res;
	int daemonize = 0;
	entry *e;
	struct sigaction sa;
	gchar *pid_file = NULL;
	gchar *socket_file = NULL;
	gchar *ini_file = NULL;
	GError *error = NULL;
	pid_t pid;
	GOptionContext *context;
	GOptionEntry option_entries[] =
	{
		{"socket", 'f', 0, G_OPTION_ARG_FILENAME, &socket_file, "Socket filename to use.", ""},
		{"ini", 'i', 0, G_OPTION_ARG_STRING, &ini_file, "Configuration ini file to use.", ""},
		{"debug", 'd', 0, G_OPTION_ARG_INT, &debug, "Debug level.", "N"},
		{"daemon", 'D', 0, G_OPTION_ARG_NONE, &daemonize, "Daemonize.", ""},
		{"pidfile", 'p', 0, G_OPTION_ARG_FILENAME, &pid_file, "Name of pid file.", ""},
		{NULL}
	};
	debug = 0;
	context = g_option_context_new("- Service Poke Server");
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
	if(!ini_file)
	{
		fprintf(stderr, "You must specify the ini file with -i\n");
		exit(-1);
	}
	if(pid_file)
	{
		pidfile = fopen(pid_file, "w");
		if(!pidfile)
		{
			fprintf(stderr, "Failed to open pid file %s\n", pid_file);
			exit(-1);
		}
	}

	sigemptyset(&child_block_mask);
	sigaddset(&child_block_mask, SIGCHLD);

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigchld_handler;
	if(sigaction(SIGCHLD, &sa, 0))
	{
		fprintf(stderr, "Failed to set signal handler\n");
		exit(-1);
	}

	commands = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

	keyfile = g_key_file_new();
	g_key_file_load_from_file(keyfile, ini_file, G_KEY_FILE_NONE, NULL);

	groups = g_key_file_get_groups(keyfile, &gs);
	for(i = 0; groups && groups[i]; i++)
	{
		if(debug > 1)
		{
			printf("Group %s\n", groups[i]);
		}
		e = (entry*)malloc(sizeof(entry));
		memset(e, 0, sizeof(entry));
		if(!e)
		{
			fprintf(stderr, "Failed to malloc\n");
			exit(-1);
		}
		e->command = g_key_file_get_string(keyfile, groups[i], "command", NULL);
		if(!e->command)
		{
			fprintf(stderr, "Command for %s not found\n", groups[i]);
			exit(-1);
		}
		else
		{
			g_hash_table_insert(commands, g_strdup(groups[i]), e);
		}
	}
	g_strfreev(groups);

	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if(socket_fd < 0)
	{
		fprintf(stderr, "Failed to create socket\n");
		return -1;
	}

	unlink(socket_file);

	memset(&addr, 0, sizeof(addr));
	strcpy(addr.sun_path, socket_file);
	addr.sun_family = AF_UNIX;

	if(bind(socket_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) != 0)
	{
		fprintf(stderr, "Failed to bind. %d\n", errno);
		return -1;
	}

	if(listen(socket_fd, 8))
	{
		fprintf(stderr, "Failed to listen, %d\n", errno);
		return -1;
	}

	if(fcntl(socket_fd, F_SETFL, O_NONBLOCK))
	{
		fprintf(stderr, "Failed to set socket nonblocking, %d\n", errno);
		return -1;
	}

	if(daemonize)
	{
		if(daemon(0, 0))
		{
			fprintf(stderr, "Failed to daemonize. %d\n", errno);
			exit(-1);
		}
	}
	pid = getpid();
	res = fprintf(pidfile, "%d\n", pid);
	fclose(pidfile);

	timeout.tv_sec = 60l * 60 * 24 * 365 * 100;
	timeout.tv_usec = 0;
	while((connection_fd = accept(socket_fd, NULL, NULL)))
	{
		if(connection_fd == -1)
		{
			if(errno == EAGAIN)
			{
				fd_set rfds;
				FD_ZERO(&rfds);
				FD_SET(socket_fd, &rfds);
				res = select(socket_fd + 1, &rfds, NULL, NULL, &timeout);
				if(debug > 0)
				{
					if(res == 0)
					{
						printf("Timed out.\n");
					}
					else
					{
						printf("Woke up.\n");
					}
				}
			}
			else if(errno != EINTR)
			{
				fprintf(stderr, "Accept failed. %d\n", errno);
			}
		}
		else
		{
			size = read(connection_fd, buffer, BUFFSIZE - 1);
			if(size >= 0)
			{
				sigprocmask(SIG_BLOCK, &child_block_mask, NULL);
				buffer[size] = '\0';
				if(size < sizeof(service_poke_header) + 1 || memcmp(buffer, "SP10", strlen("SP10")))
				{
					fprintf(stderr, "Unknown packet. Size %d\n", size);
					if(size > 4)
					{
						fprintf(stderr, "%c%c%c%c\n", buffer[0], buffer[1], buffer[2], buffer[3]);
					}
				}
				else
				{
					e = g_hash_table_lookup(commands, buffer + sizeof(service_poke_header));
					if(!e)
					{
						fprintf(stderr, "Command |%s| not found. %d\n", buffer + sizeof(service_poke_header), g_hash_table_size(commands));
					}
					else
					{
						e->timeout.tv_usec = 0;
						tmptime = GUINT64_FROM_LE(((service_poke_header*)buffer)->delay);
						if(debug > 0)
						{
							printf("Got |%s|%" G_GUINT64_FORMAT " from client. Running %s\n", buffer + sizeof(service_poke_header), tmptime, e->command);
						}
						if(e->pending == 0)
						{
							e->timeout.tv_sec = tmptime;
							if(tmptime != 0)
							{
								e->timeout.tv_sec += time(NULL);
							}
						}
						else
						{
							if(e->timeout.tv_sec != 0)
							{
								if(tmptime == 0)
								{
									e->timeout.tv_sec = tmptime;
								}
								else
								{
									e->timeout.tv_sec = MIN(e->timeout.tv_sec, tmptime + time(NULL));
								}
							}
						}
						e->pending = 1;
/*
						if(e->running == 0 && e->pending == 1)
						{
							do_execute(e);
						}*/
					}
				}
				sigprocmask(SIG_UNBLOCK, &child_block_mask, NULL);
			}
			else
			{
				fprintf(stderr, "Failed to read. %d\n", errno);
			}
			close(connection_fd);
		}
		sigprocmask(SIG_BLOCK, &child_block_mask, NULL);
		//Look for and process pending timed out vars.
		timeout.tv_sec = 60l * 60 * 24 * 365 * 100;
		timeout.tv_usec = 0;
		g_hash_table_foreach(commands, timeout_foreach, &timeout);
		if(debug > 0)
		{
			printf("Timeout %ld\n", timeout.tv_sec);
		}
		sigprocmask(SIG_UNBLOCK, &child_block_mask, NULL);
	}
	return 0;
}
