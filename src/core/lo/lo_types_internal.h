#ifndef LO_TYPES_H
#define LO_TYPES_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_POLL
#include <poll.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#ifdef _MSC_VER
typedef SSIZE_T ssize_t;
typedef unsigned __int64 uint64_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef __int32 int32_t;
#endif

#ifndef WIN32
#include <pthread.h>
#else
#ifndef HAVE_PTHREAD_H
typedef void * pthread_t;
#endif
#endif // WIN32

#include "lo_osc_types.h"

typedef void (*lo_err_handler)(int num, const char *msg, const char *path);

struct _lo_method;

typedef struct _lo_address {
	char            *host;
	int              socket;
	char            *port;
	int              protocol;
	struct addrinfo *ai;
	int              errnum;
	const char      *errstr;
	int              ttl;
} *lo_address;

typedef struct _lo_blob {
	uint32_t  size;
	char     *data;
} *lo_blob;

typedef struct _lo_message {
	char      *types;
	size_t     typelen;
	size_t     typesize;
	void      *data;
	size_t     datalen;
	size_t     datasize;
	lo_address source;
        lo_arg   **argv;
        /* timestamp from bundle (LO_TT_IMMEDIATE for unbundled messages) */
        lo_timetag ts;
} *lo_message;

typedef int (*lo_method_handler)(const char *path, const char *types,
				 lo_arg **argv, int argc, struct _lo_message
				 *msg, void *user_data);

typedef struct _lo_method {
	const char        *path;
	const char        *typespec;
	lo_method_handler  handler;
	char              *user_data;
	struct _lo_method *next;
} *lo_method;

typedef struct _lo_server {
	struct addrinfo         *ai;
	lo_method                first;
	lo_err_handler           err_h;
	int	 	         port;
	char                   	*hostname;
	char                   	*path;
	int            	         protocol;
	void		        *queued;
	struct sockaddr_storage  addr;
	socklen_t 	         addr_len;
	int                  sockets_len;
	int                  sockets_alloc;
#ifdef HAVE_POLL
	struct pollfd        *sockets;
#else
	struct { int fd; }   *sockets;
#endif
} *lo_server;

typedef struct _lo_server_thread {
lo_server    s;
	pthread_t    thread;
	volatile int active;
	volatile int done;
} *lo_server_thread;

typedef struct _lo_bundle {
	size_t      size;
	size_t	    len;
	lo_timetag  ts;
	lo_message *msgs;
	char      **paths;
} *lo_bundle;

typedef struct _lo_strlist {
	char *str;
	struct _lo_strlist *next;
} lo_strlist;

typedef union {
    int32_t  i;
    float    f;
    char     c;
    uint32_t nl;
} lo_pcast32;
    
typedef union {
    int64_t    i;
    double     f;
    uint64_t   nl;
    lo_timetag tt;
} lo_pcast64;

extern struct lo_cs {
	int udp;
	int tcp;
} lo_client_sockets;
	
#endif
