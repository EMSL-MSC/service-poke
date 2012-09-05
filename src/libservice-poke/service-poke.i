/* service-poke.i */
%module servicepoke
%{
#include <string.h>
#include <glib.h>
#include "service-poke.h"

char *service_poke_wrapper(char *socket_file, char *service, int delay)
{
        char *ret = NULL;
        char *tmpret;
        int res;
        res = service_poke(socket_file, service, delay, &tmpret);
        if(res && tmpret)
        {
                ret = strdup(tmpret);
                g_free(tmpret);
        }
        return ret;
}
%}

#ifdef SWIGPYTHON
%rename(poke) service_poke_wrapper;
#else
%rename(service_poke) service_poke_wrapper;
#endif

%newobject service_poke_wrapper;
char *service_poke_wrapper(char *socket_file, char *service, int delay);

