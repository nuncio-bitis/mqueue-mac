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
 * mqueue.c
 *   An implementation of Linux mqueue for Mac OS X using System V Messages.
 *   This is needed because Mac OS doesn't support POSIX mqueue.
 *   @NOTE: This is not guaranteed to work exactly like mqueue (or even at all)
 *   I created this just to be able to debug Linux programs on my Mac.
 *
 *  Created on: 23-Jan-2022
 *      Author: jparziale
 */

#include "mqueue.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// ****************************************************************************

/*
 * Declare the message structure.
 */
#define MSGSZ 1024 // Max message size
typedef struct msgbuf_t {
    long mtype; // use for message priority
    char mtext[MSGSZ];
} message_buf;

// ****************************************************************************

/**
 * Open a message queue.
 * Returns mqueue ID on success, or -1 with errno set on failure.
 */
mqd_t mq_open (const char *__name, int __oflag, ...)
{
    // Start with invalid value.
    mqd_t mqd = -1;

    va_list argp;

    va_start (argp, __oflag);

    // arg 3 - mode of the created message queue.
    mode_t mode = (mode_t)va_arg (argp, uint32_t);

    // arg 4 - pointer to message queue attributes, or NULL for defaults.
    struct mq_attr * attr = (struct mq_attr *)va_arg (argp, struct mq_attr *);

    (void) mode; // IGNORED
    (void) attr; // IGNORED

    // Convert name to a numeric key
    key_t key = 0;
    for (size_t i = 0; __name[i]; i++)
    {
        key += __name[i];
    }

    __oflag |= (IPC_CREAT | 0666);
    if ((mqd = msgget(key, __oflag)) < 0)
    {
        perror("mq_open : msgget");
    }

    va_end (argp);

    return mqd;
}

// ****************************************************************************

/**
 * Close a message queue descriptor.
 * Returns 0 on success, or -1 with errno set on failure.
 */
int mq_close (mqd_t __mqdes)
{
    int ret = -1;
    struct msqid_ds msqds;
    ret = msgctl(__mqdes, IPC_RMID, &msqds);
    if (-1 == ret)
    {
        printf("ERROR: Couldn't remove message queue %d\n", __mqdes);
        perror("mq_close : msgctl(IPC_RMID)");
    }

    return ret;
}

// ****************************************************************************

/**
 * Get message queue attributes.
 * Returns 0 on success, or -1 with errno set on failure.
 */
int mq_getattr (mqd_t __mqdes, struct mq_attr *__mqstat)
{
    struct msqid_ds msqds;
    if (-1 == msgctl(__mqdes, IPC_STAT, &msqds))
    {
        printf("ERROR: Couldn't get mqueue stats for id = %d\n", __mqdes);
        perror("mq_getattr : msgctl(IPC_STAT)");
        return -1;
    }

    return msqds.msg_perm.mode;
}

// ****************************************************************************

/**
 * Set message queue attributes.
 * Returns 0 on success, or -1 with errno set on failure.
 */
int mq_setattr (
    mqd_t __mqdes,
    const struct mq_attr *__restrict __mqstat,
    struct mq_attr *__restrict __omqstat)
{
    struct msqid_ds msqds;

    if (-1 == msgctl(__mqdes, IPC_STAT, &msqds))
    {
        printf("ERROR: Couldn't get mqueue stats for id = %d\n", __mqdes);
        perror("mq_getattr : msgctl(IPC_STAT)");
        return -1;
    }
    if (__omqstat)
    {
        __omqstat->mq_flags = msqds.msg_perm.mode;
    }

    msqds.msg_perm.mode = __mqstat->mq_flags;
    if (-1 == msgctl(__mqdes, IPC_SET, &msqds))
    {
        printf("ERROR: Couldn't set mqueue stats for id = %d\n", __mqdes);
        perror("mq_getattr : msgctl(IPC_SET)");
        return -1;
    }

    return 0;
}

// ****************************************************************************

/**
 * Remove a message queue.
 * Returns 0 on success, or -1 with errno set on failure.
 */
int mq_unlink (const char *__name)
{
    int ret = -1;

    // First find a message queue from the given name
    mqd_t mqd = -1;

    // Convert name to a numeric key
    key_t key = 0;
    for (size_t i = 0; __name[i]; i++)
    {
        key += __name[i];
    }

    // msgget must fail, meaning the queue exists
    if ((mqd = msgget(key, IPC_EXCL)) > 0)
    {
        printf("ERROR: Message queue (%d) didn't exist.\n", mqd);
        perror("mq_unlink : msgget");
    }
    else if (errno == EEXIST)
    {
        ret = mq_close(mqd);
    }
    else
    {
        printf("Error: %d\n", errno);
        perror("mq_unlink : msgget");
    }

    return ret;
}

// ****************************************************************************

/**
 * Register for notification when a message is available.
 * Returns 0 on success, or -1 with errno set on failure.
 */
int mq_notify (mqd_t __mqdes, const struct sigevent *__notification)
{
    return -1; // @TODO
}

// ****************************************************************************

/**
 * Receive a message from a message queue.
 * Returns number of bytes in the message on success,
 * or -1 with errno set on failure.
 */
ssize_t mq_receive (
    mqd_t __mqdes,
    char *__msg_ptr,
    size_t __msg_len,
    unsigned int *__msg_prio)
{
    message_buf rbuf;
    size_t nbytes = 0;

    nbytes = msgrcv(__mqdes, &rbuf, __msg_len, 0, 0);
    if (nbytes < 0)
    {
        perror("mq_receive : msgrcv");
        return -1;
    }

    if (__msg_prio)
    {
        *__msg_prio = rbuf.mtype;
    }
    (void) strncpy(__msg_ptr, rbuf.mtext, MSGSZ);

    return nbytes;
}

// ****************************************************************************

/**
 * Send a message to a message queue.
 * Returns 0 on success, or -1 with errno set on failure.
 */
int mq_send (
    mqd_t __mqdes,
    const char *__msg_ptr,
    size_t __msg_len,
    unsigned int __msg_prio)
{
    message_buf sbuf;

    sbuf.mtype = __msg_prio;
    (void) strncpy(sbuf.mtext, __msg_ptr, MSGSZ-1);

    size_t buf_length = strlen(sbuf.mtext);

    int ret = msgsnd(__mqdes, &sbuf, buf_length, IPC_NOWAIT);
    if (ret < 0)
    {
        perror("mq_send : msgsnd");
        return -1;
    }

    return 0;
}

// ****************************************************************************
