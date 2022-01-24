/*
 * mq_server.c
 *  Sample POSIX mqueue server to demonstrate interprocess communication
 *
 *  Created on: Sep 5, 2016
 *      Author: Jim Parziale
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

const char SERVER_QUEUE_NAME[] = "/sp-example-server";
const char CLIENT_QUEUE_NAME[] = "/sp-example-client";
const char TERMINATION_MSG[] = "<TERMINATE>";

#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10

int main (int argc, char **argv)
{
    mqd_t qd_server, qd_client;   // queue descriptors
    long token_number = 1; // next token to be given to client

    struct mq_attr attr;

    attr.mq_flags   = 0;
    attr.mq_maxmsg  = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1)
    {
        perror ("Server: mq_open (server)");
        return 1;
    }
    char in_buffer [MAX_MSG_SIZE+1];
    char out_buffer [MAX_MSG_SIZE+1];
    memset(in_buffer, 0, sizeof(in_buffer));
    memset(out_buffer, 0, sizeof(out_buffer));

    printf ("Server: Hello, World!\n");

    // Wair for client queue name
    if (mq_receive (qd_server, in_buffer, MAX_MSG_SIZE, NULL) == -1)
    {
        perror ("Server: mq_receive");
        return 1;
    }

    printf ("Server: Client queue name received: %s\n", in_buffer);
    if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1)
    {
        perror ("Server: Not able to open client queue");
    }

    // Client will send a termination message that will shut down the server.
    while (1)
    {
        // get the oldest message with highest priority
        memset(in_buffer, '\0', sizeof(in_buffer));
        if (mq_receive (qd_server, in_buffer, MAX_MSG_SIZE, NULL) == -1)
        {
            perror ("Server: mq_receive");
            return 1;
        }

        printf ("Server: Received %s\n", in_buffer);

        // Check if we just got a termination message
        if (0 == strncmp(TERMINATION_MSG, in_buffer, sizeof(in_buffer)))
        {
            break;
        }

        // send reply message to client
        snprintf (out_buffer, sizeof(out_buffer), "(%ld) %s", token_number, in_buffer);
        if (mq_send (qd_client, out_buffer, strlen (out_buffer), 1) == -1)
        {
            perror ("Server: Not able to send message to client");
            continue;
        }

        printf ("Server: response sent to client.\n");
        token_number++;
    }

    printf ("Server: mq_close\n");
    if (mq_close (qd_server) == -1)
    {
        perror ("Server: mq_close");
        return 1;
    }

    printf ("Server: bye\n");

    return EXIT_SUCCESS;
}
