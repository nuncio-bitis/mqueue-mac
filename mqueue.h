/* Copyright (C) 2004-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.
 */

/**
 * mqueue.h
 *   An implementation of Linux mqueue for Mac OS X using System V Messages.
 *   I copied the structure of this from Linux' mqueue.h
 *   This is needed because Mac OS doesn't support POSIX mqueue.
 *   @NOTE: This is not guaranteed to work exactly like mqueue (or even at all)
 *   I created this just to be able to debug Linux programs on my Mac.
 *
 *  Created on: 23-Jan-2022
 *      Author: jparziale
 */

#ifndef _MQUEUE_H
#define _MQUEUE_H
// ****************************************************************************

#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

// ****************************************************************************

typedef int mqd_t;

struct mq_attr
{
  int32_t mq_flags;	/* Message queue flags.  */
  int32_t mq_maxmsg;	/* Maximum number of messages.  */
  int32_t mq_msgsize;	/* Maximum message size.  */
  int32_t mq_curmsgs;	/* Number of messages currently queued.  */
  int32_t __pad[4];
};

// ****************************************************************************

__BEGIN_DECLS

/* Establish connection between a process and a message queue NAME and
   return message queue descriptor or (mqd_t) -1 on error.  OFLAG determines
   the type of access used.  If O_CREAT is on OFLAG, the third argument is
   taken as a `mode_t', the mode of the created message queue, and the fourth
   argument is taken as `struct mq_attr *', pointer to message queue
   attributes.  If the fourth argument is NULL, default attributes are
   used.  */
mqd_t mq_open (const char *__name, int __oflag, ...);

/* Removes the association between message queue descriptor MQDES and its
   message queue.  */
int mq_close (mqd_t __mqdes);

/* Query status and attributes of message queue MQDES.  */
int mq_getattr (mqd_t __mqdes, struct mq_attr *__mqstat);

/* Set attributes associated with message queue MQDES and if OMQSTAT is
   not NULL also query its old attributes.  */
int mq_setattr (
    mqd_t __mqdes,
    const struct mq_attr *__restrict __mqstat,
    struct mq_attr *__restrict __omqstat);

/* Remove message queue named NAME.  */
// @NOTE: mq_unlink does the same thing as mq_close.
// You don't need to use both.
int mq_unlink (const char *__name);

/* Register notification issued upon message arrival to an empty
   message queue MQDES.  */
int mq_notify (mqd_t __mqdes, const struct sigevent *__notification);

/* Receive the oldest from highest priority messages in message queue
   MQDES.  */
ssize_t mq_receive (
    mqd_t __mqdes,
    char *__msg_ptr,
    size_t __msg_len,
    unsigned int *__msg_prio);

/* Add message pointed by MSG_PTR to message queue MQDES.  */
int mq_send (
    mqd_t __mqdes,
    const char *__msg_ptr,
    size_t __msg_len,
    unsigned int __msg_prio);

#ifdef __USE_XOPEN2K
/* Receive the oldest from highest priority messages in message queue
   MQDES, stop waiting if ABS_TIMEOUT expires.  */
ssize_t mq_timedreceive (mqd_t __mqdes, char *__restrict __msg_ptr,
				size_t __msg_len,
				unsigned int *__restrict __msg_prio,
				const struct timespec *__restrict __abs_timeout)
  __nonnull ((2, 5));

/* Add message pointed by MSG_PTR to message queue MQDES, stop blocking
   on full message queue if ABS_TIMEOUT expires.  */
int mq_timedsend (mqd_t __mqdes, const char *__msg_ptr,
			 size_t __msg_len, unsigned int __msg_prio,
			 const struct timespec *__abs_timeout)
  __nonnull ((2, 5));
#endif

/* Define some inlines helping to catch common problems.  */
#if __USE_FORTIFY_LEVEL > 0 && defined __fortify_function \
    && defined __va_arg_pack_len
# include <bits/mqueue2.h>
#endif

__END_DECLS

// ****************************************************************************
#endif /* mqueue.h */
