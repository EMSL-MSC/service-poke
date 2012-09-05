typedef struct
{
	char header[4];
	guint64 delay;
	char data[0];
} __attribute__ ((packed)) service_poke_header;

int service_poke(char *socket_file, char *service, int delay, char **error);
