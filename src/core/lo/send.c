/*
 *  Copyright (C) 2014 Steve Harris et al. (see AUTHORS)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  $Id$
 */

#include "config.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#if !defined(_WIN32)
#include <ifaddrs.h> // ifaddr, see is_local_broadcast method
#endif

#if defined(_WIN32) || defined(_MSC_VER)
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#endif

#include "lo_types_internal.h"
#include "lo_internal.h"
#include "lo.h"

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#if defined(_WIN32) || defined(_MSC_VER)
int initWSock();
#endif

#if defined(_WIN32) || defined(_MSC_VER)
#define geterror() WSAGetLastError()
#else
#define geterror() errno
#endif

static int create_socket(lo_address a);
static int send_data(lo_address a, lo_server from, char *data,
                     const size_t data_len);

// message.c
int lo_message_add_varargs_internal(lo_message m, const char *types,
                                    va_list ap, const char *file,
                                    int line);

static
int lo_send_varargs_internal(lo_address t, const char *file,
                 const int line, const char *path,
                 const char *types, va_list ap)
{
    int ret;
    lo_message msg = lo_message_new();

    t->errnum = 0;
    t->errstr = NULL;

    ret = lo_message_add_varargs_internal(msg, types, ap, file, line);

    if (ret) {
        lo_message_free(msg);
        t->errnum = ret;
        if (ret == -1)
            t->errstr = "unknown type";
        else
            t->errstr = "bad format/args";
        return ret;
    }

    ret = lo_send_message(t, path, msg);
    lo_message_free(msg);

    return ret;
}

#if defined(USE_ANSI_C) || defined(DLL_EXPORT)
int lo_send(lo_address t, const char *path, const char *types, ...)
{
    const char *file = "";
    int line = 0;
    va_list ap;
    va_start(ap, types);
    int ret = lo_send_varargs_internal(t, file, line, path, types, ap);
    va_end(ap);
    return ret;
}
#endif

/* Don't call lo_send_internal directly, use lo_send, a macro wrapping this
 * function with appropriate values for file and line */

int lo_send_internal(lo_address t, const char *file, const int line,
                     const char *path, const char *types, ...)
{
    va_list ap;
    va_start(ap, types);
    int ret = lo_send_varargs_internal(t, file, line, path, types, ap);
    va_end(ap);
    return ret;
}

static
int lo_send_timestamped_varargs_internal(lo_address t, const char *file,
                     const int line, lo_timetag ts,
                     const char *path, const char *types,
                     va_list ap)
{
    int ret;
    lo_message msg = lo_message_new();
    lo_bundle b = lo_bundle_new(ts);

    t->errnum = 0;
    t->errstr = NULL;

    ret = lo_message_add_varargs_internal(msg, types, ap, file, line);

    if (ret == 0) {
        lo_bundle_add_message(b, path, msg);
        ret = lo_send_bundle(t, b);
    }

    lo_message_free(msg);
    lo_bundle_free(b);

    return ret;
}


#if defined(USE_ANSI_C) || defined(DLL_EXPORT)
int lo_send_timestamped(lo_address t, lo_timetag ts,
                        const char *path, const char *types, ...)
{
    const char *file = "";
    int line = 0;
    va_list ap;
    va_start(ap, types);
    int ret = lo_send_timestamped_varargs_internal(t, file, line, ts, path,
                                                   types, ap);
    va_end(ap);
    return ret;
}
#endif

/* Don't call lo_send_timestamped_internal directly, use lo_send_timestamped, a
 * macro wrapping this function with appropriate values for file and line */
int lo_send_timestamped_internal(lo_address t, const char *file,
                                 const int line, lo_timetag ts,
                                 const char *path, const char *types, ...)
{
    va_list ap;
    va_start(ap, types);
    int ret = lo_send_timestamped_varargs_internal(t, file, line, ts, path,
                                                   types, ap);
    va_end(ap);
    return ret;
}

static
int lo_send_from_varargs_internal(lo_address to, lo_server from,
                  const char *file,
                  const int line, lo_timetag ts,
                  const char *path, const char *types,
                  va_list ap)
{
    lo_bundle b = NULL;
    int ret;

    lo_message msg = lo_message_new();
    if (ts.sec != LO_TT_IMMEDIATE.sec || ts.frac != LO_TT_IMMEDIATE.frac)
        b = lo_bundle_new(ts);

    // Clear any previous errors
    to->errnum = 0;
    to->errstr = NULL;

    ret = lo_message_add_varargs_internal(msg, types, ap, file, line);

    if (ret == 0) {
        if (b) {
            lo_bundle_add_message(b, path, msg);
            ret = lo_send_bundle_from(to, from, b);
        } else {
            ret = lo_send_message_from(to, from, path, msg);
        }
    }
    // Free-up memory
    lo_message_free(msg);
    if (b)
        lo_bundle_free(b);

    return ret;
}

#if defined(USE_ANSI_C) || defined(DLL_EXPORT)
int lo_send_from(lo_address to, lo_server from, lo_timetag ts,
                 const char *path, const char *types, ...)
{
    const char *file = "";
    int line = 0;
    va_list ap;
    va_start(ap, types);
    int ret = lo_send_from_varargs_internal(to, from, file, line, ts,
                                            path, types, ap);
    va_end(ap);
    return ret;
}
#endif

/* Don't call lo_send_from_internal directly, use macros wrapping this
 * function with appropriate values for file and line */

int lo_send_from_internal(lo_address to, lo_server from, const char *file,
                          const int line, const lo_timetag ts,
                          const char *path, const char *types, ...)
{
    va_list ap;
    va_start(ap, types);
    int ret = lo_send_from_varargs_internal(to, from, file, line, ts,
                                            path, types, ap);
    va_end(ap);
    return ret;
}

#if !defined(_WIN32)
static int is_local_broadcast(struct addrinfo *ai){ 
    struct ifaddrs *ifap, *ifa;
    uint32_t destination_addr = ((struct sockaddr_in *) ai->ai_addr)->sin_addr.s_addr;
    int result;

    getifaddrs(&ifap);

    result = 0;

    // loop over all network interfaces
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            // get this machine's address for the current interface (ie. 192.168.1.6)
            uint32_t cur_addr = ((struct sockaddr_in *) ifa->ifa_addr)->sin_addr.s_addr;
            // get the subnet mask for the current interface (ie. 255.255.0.0)
            uint32_t cur_subn = ((struct sockaddr_in *) ifa->ifa_netmask)->sin_addr.s_addr;

            // check if the subnet part (ie. 192.168._._) of the given address matches
            if ((cur_subn & cur_addr) == (cur_subn & destination_addr)) {
                // check if the non-subnet (ie. _._.1.6) part of the destination address
                // indicates that it is a broadcast address (all ones)
                if ((cur_subn | destination_addr) == 0xFFFFFFFF) {
                    result = 1;
                }

                break;
            }
        }
    }

    freeifaddrs(ifap);
    return result;
}
#endif

static int is_broadcast(struct addrinfo *ai) {
    struct sockaddr_in *si = (struct sockaddr_in *) ai->ai_addr;
    unsigned char *ip = (unsigned char *) &(si->sin_addr);
    if(AF_INET == ai->ai_family) {
        // is it the "global" broadcast address?
        if (ip[0] == 255 && ip[1] == 255 && ip[2] == 255 && ip[3] == 255)
            return 1;

        #if !defined(_WIN32)
            // check if the address is the "local" broadcast
            // addres of any of the active network interfaces
            return is_local_broadcast(ai);
        #endif
    }
    return 0;
}

static int create_socket(lo_address a)
{
    switch(a->protocol) {
    case LO_UDP:
        a->socket = socket(a->ai->ai_family, a->ai->ai_socktype, 0);
        if (a->socket == -1) {
            a->errnum = geterror();
            a->errstr = NULL;
            return -1;
        }
        // if UDP and destination address is broadcast,
        // then allow broadcast on the socket
        if (is_broadcast(a->ai)) {
            int opt = 1;
            setsockopt(a->socket, SOL_SOCKET, SO_BROADCAST,
                (const char*)&opt, sizeof(int));
        }
        break;
    case LO_TCP: {
        struct addrinfo *ai = a->ai;
        int sfd = -1;
        for(ai=a->ai; ai != NULL; ai = ai->ai_next) {
            sfd = socket(ai->ai_family, ai->ai_socktype, 0);
            if (-1 == sfd)
                continue;
            if(-1 != connect(sfd, ai->ai_addr, ai->ai_addrlen))
                break;
            closesocket(sfd);
        }
        if(NULL == ai) {
            /* all addresses failed */
            a->socket = -1;
            a->errnum = geterror();
            a->errstr = NULL;
            return -1;
        }
        a->socket = sfd;
    }
        break;
#if !defined(_WIN32) && !defined(_MSC_VER)
    case LO_UNIX: {
        struct sockaddr_un sa;

        a->socket = socket(PF_UNIX, SOCK_DGRAM, 0);
        if (a->socket == -1) {
            a->errnum = geterror();
            a->errstr = NULL;
            return -1;
        }

        sa.sun_family = AF_UNIX;
        strncpy(sa.sun_path, a->port, sizeof(sa.sun_path) - 1);

        if ((connect(a->socket, (struct sockaddr *) &sa, sizeof(sa))) < 0) {
            a->errnum = geterror();
            a->errstr = NULL;
            closesocket(a->socket);
            a->socket = -1;
            return -1;
        }
    }
        break;
#endif
    default:
        /* unknown protocol */
        return -2;
    }

#ifdef SO_NOSIGPIPE
    {
        // On Mac OS X: Prevent the socket from causing a SIGPIPE.
        // cf MSG_NOSIGNAL on Linux.
        int option = 1; // yes, we don't want SIGPIPE
        setsockopt(a->socket, SOL_SOCKET, SO_NOSIGPIPE, &option,
                   sizeof(option));
    }
#endif

#ifdef TCP_NODELAY
    if (a->flags & LO_NODELAY) {
        int option = 1;
        setsockopt(a->socket, IPPROTO_TCP, TCP_NODELAY,
				   (const char*)&option, sizeof(option));
    }
#endif

    return 0;
}


// From http://tools.ietf.org/html/rfc1055
#define SLIP_END        0300    /* indicates end of packet */
#define SLIP_ESC        0333    /* indicates byte stuffing */
#define SLIP_ESC_END    0334    /* ESC ESC_END means END data byte */
#define SLIP_ESC_ESC    0335    /* ESC ESC_ESC means ESC data byte */

static unsigned char *slip_encode(const unsigned char *data,
                                  size_t *data_len,
                                  int double_end_slip_enabled)
{
    size_t i, j = 0, len=*data_len;
    unsigned char *slipdata = (unsigned char *) malloc(len*2);
    if (double_end_slip_enabled)
        slipdata[j++] = SLIP_END;
    for (i=0; i<len; i++) {
        switch (data[i])
        {
        case SLIP_ESC:
            slipdata[j++] = SLIP_ESC;
            slipdata[j++] = SLIP_ESC_ESC;
            break;
        case SLIP_END:
            slipdata[j++] = SLIP_ESC;
            slipdata[j++] = SLIP_ESC_END;
            break;
        default:
            slipdata[j++] = data[i];
        }
    }
    slipdata[j++] = SLIP_END;
    slipdata[j] = 0;
    *data_len = j;
    return slipdata;
}

static int send_data(lo_address a, lo_server from, char *data,
                     const size_t data_len)
{
    ssize_t ret = 0;
    int sock = -1;

#if defined(_WIN32) || defined(_MSC_VER)
    if (!initWSock())
        return -1;
#endif

    if (a->protocol == LO_UDP && data_len > LO_MAX_UDP_MSG_SIZE) {
        a->errnum = 99;
        a->errstr = "Attempted to send message in excess of maximum "
            "message size";
        return -1;
    }
    // Resolve the destination address, if not done already
    if (!a->ai) {
        ret = lo_address_resolve(a);
        if (ret)
            return (int) ret;
    }
    // Re-use existing socket?
    if (from && a->protocol == LO_UDP) {
        sock = from->sockets[0].fd;
    } else if (a->protocol == LO_UDP && lo_client_sockets.udp != -1) {
        sock = lo_client_sockets.udp;
    } else {
        if (a->socket == -1) {
            ret = create_socket(a);
            if (ret)
                return (int) ret;
        }

        // If we are sending TCP, we may later receive on sending
        // socket, so add it to the from server's socket list.
        if (from && a->protocol == LO_TCP)
        {
            if (a->socket >= from->sources_len
                || from->sources[a->socket].host == NULL)
            {
                lo_server_add_socket(from, a->socket, a, 0, 0);

                // If a socket is added to the server, the server is
                // now responsible for closing it.
                a->ownsocket = 0;
            }
        }
        sock = a->socket;
    }

    if (a->protocol == LO_TCP && !(a->flags & LO_SLIP)) {
        // For TCP only, send the length of the following data
        int32_t size = htonl(data_len);
        ret = send(sock, (const void*)&size, sizeof(size), MSG_NOSIGNAL);
    }
    // Send the data
    if (ret != -1) {
        if (a->protocol == LO_UDP) {
            struct addrinfo* ai;
            if (a->addr.size == sizeof(struct in_addr)) {
                setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF,
                           (const char*)&a->addr.a, (socklen_t) a->addr.size);
            }
#ifdef ENABLE_IPV6
            else if (a->addr.size == sizeof(struct in6_addr)) {
                setsockopt(sock, IPPROTO_IP, IPV6_MULTICAST_IF,
                           (const char*)&a->addr.a, (socklen_t) a->addr.size);
            }
#endif
            if (a->ttl >= 0) {
                unsigned char ttl = (unsigned char) a->ttl;
                setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,
						   (const char*)&ttl, sizeof(ttl));
            }

            ai = a->ai;

            do {
                ret = sendto(sock, data, data_len, MSG_NOSIGNAL,
                             ai->ai_addr, ai->ai_addrlen);
                ai = ai->ai_next;
            } while (ret == -1 && ai != NULL);
            if (ret == -1 && ai != NULL && a->ai!=ai)
                a->ai = ai;
#if !defined(_WIN32) && !defined(_MSC_VER)
        } else if (a->protocol == LO_UNIX && from && from->protocol == LO_UNIX) {
            struct sockaddr_un saddr;
            size_t len = data_len;

            saddr.sun_family = AF_UNIX;
            strncpy(saddr.sun_path, a->port, sizeof(saddr.sun_path) - 1);

            ret = sendto(from->sockets[0].fd, data, len, MSG_NOSIGNAL, (struct sockaddr*)&saddr, sizeof(struct sockaddr_un));
#endif
        } else {

            size_t len = data_len;
            if (a->flags & LO_SLIP)
                data = (char*)slip_encode((unsigned char*)data, &len,
                                          a->flags & LO_SLIP_DBL_END);

            ret = send(sock, data, len, MSG_NOSIGNAL);

            if (a->flags & LO_SLIP)
                free(data);
        }
    }

    if (ret == -1) {
        if (a->protocol == LO_TCP) {
            closesocket(a->socket);
            if (from)
                lo_server_del_socket(from, -1, a->socket);
            a->socket = -1;
        }

        a->errnum = geterror();
        a->errstr = NULL;
    } else {
        a->errnum = 0;
        a->errstr = NULL;
    }

    return (int) ret;
}


int lo_send_message(lo_address a, const char *path, lo_message msg)
{
    return lo_send_message_from(a, NULL, path, msg);
}

int lo_send_message_from(lo_address a, lo_server from, const char *path,
                         lo_message msg)
{
    const size_t data_len = lo_message_length(msg, path);
    char *data = (char*) lo_message_serialise(msg, path, NULL, NULL);

    // Send the message
    int ret = send_data(a, from, data, data_len);

    // For TCP, retry once if it failed.  The first try will return
    // error if the connection was closed, so the second try will
    // attempt to re-open the connection.
    if (ret == -1 && a->protocol == LO_TCP)
        ret = send_data(a, from, data, data_len);

    // Free the memory allocated by lo_message_serialise
    if (data)
        free(data);

    return ret;
}


int lo_send_bundle(lo_address a, lo_bundle b)
{
    return lo_send_bundle_from(a, NULL, b);
}


int lo_send_bundle_from(lo_address a, lo_server from, lo_bundle b)
{
    size_t data_len;
    char *data = (char*) lo_bundle_serialise(b, NULL, &data_len);

    // Send the bundle
    int ret = send_data(a, from, data, data_len);

    // Free the memory allocated by lo_bundle_serialise
    if (data)
        free(data);

    return ret;
}

/* vi:set ts=8 sts=4 sw=4: */
