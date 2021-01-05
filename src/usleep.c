/*                              */
/* xemeraldia   -----  usleep.c */
/*                              */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
 
#ifdef SYSV
#include <time.h>
#ifdef sgi
#include <sys/time.h>
#endif

void usleep(unsigned useconds)
{
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = useconds;

    select (0, 0, 0, 0, &timeout);
}
#endif
