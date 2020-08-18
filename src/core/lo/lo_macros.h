/*
 *  Copyright (C) 2004 Steve Harris
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  $Id$
 */

#ifndef LO_MACROS_H
#define LO_MACROS_H

/* macros that have to be defined after function signatures */

#ifdef __cplusplus
extern "C" {
#endif

/* \brief Maximum length of UDP messages in bytes
 */
#define LO_MAX_MSG_SIZE 32768

/* \brief A set of macros to represent different communications transports
 */
#define LO_DEFAULT 0x0
#define LO_UDP     0x1
#define LO_UNIX    0x2
#define LO_TCP     0x4

/* an internal value, ignored in transmission but check against LO_MARKER in the
 * argument list. Used to do primitive bounds checking */
#define LO_MARKER_A 0xdeadbeef
#define LO_MARKER_B 0xf00baa23
#define LO_ARGS_END LO_MARKER_A, LO_MARKER_B

#define lo_message_add_varargs(msg, types, list) \
    lo_message_add_varargs_internal(msg, types, list, __FILE__, __LINE__)

#ifdef __GNUC__

#define lo_message_add(msg, types...)                         \
    lo_message_add_internal(msg, __FILE__, __LINE__, types,   \
                            LO_MARKER_A, LO_MARKER_B)

#define lo_send(targ, path, types...) \
        lo_send_internal(targ, __FILE__, __LINE__, path, types, \
			 LO_MARKER_A, LO_MARKER_B)

#define lo_send_timestamped(targ, ts, path, types...) \
        lo_send_timestamped_internal(targ, __FILE__, __LINE__, ts, path, \
		       	             types, LO_MARKER_A, LO_MARKER_B)

#define lo_send_from(targ, from, ts, path, types...) \
        lo_send_from_internal(targ, from, __FILE__, __LINE__, ts, path, \
		       	             types, LO_MARKER_A, LO_MARKER_B)

#else

/* In non-GCC compilers, there is no support for variable-argument
 * macros, so provide "internal" vararg functions directly instead. */

int lo_message_add(lo_message msg, const char *types, ...);
int lo_send(lo_address targ, const char *path, const char *types, ...);
int lo_send_timestamped(lo_address targ, lo_timetag ts, const char *path, const char *types, ...);
int lo_send_from(lo_address targ, lo_server from, lo_timetag ts, const char *path, const char *types, ...);

#endif

#ifdef __cplusplus
}
#endif

#endif
