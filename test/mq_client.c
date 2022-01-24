/*
 * mq_client.c
 *  Client program to demonstrate interprocess communication
 *  with POSIX message queues
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

#define SERVER_QUEUE_NAME   "sp-example-server"
#define CLIENT_QUEUE_NAME   "sp-example-client"
#define TERMINATION_MSG     "<TERMINATE>"

#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256

int main (int argc, char **argv)
{
    mqd_t qd_server, qd_client;   // queue descriptors
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_client = mq_open (CLIENT_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1)
    {
        perror ("Client: mq_open (client)");
        return 1;
    }

    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1)
    {
        perror ("Client: mq_open (server)");
        return 1;
    }

    // send client queue name to server
    if (mq_send (qd_server, CLIENT_QUEUE_NAME, strlen(CLIENT_QUEUE_NAME), 1) == -1)
    {
        perror ("Client: Not able to send message to server");
        return 1;
    }

    char in_buffer [MAX_MSG_SIZE];
    char temp_buf [MAX_MSG_SIZE];

    // @NOTE: When the user enters '^D', this will send
    // TERMINATION_MSG to the server to shut it down.

    printf ("Enter a string (Press <ENTER>): ");
    while (fgets (temp_buf, MAX_MSG_SIZE, stdin))
    {
        // Remove the trailing newline
        temp_buf[strlen(temp_buf)-1] = '\0';

        if (strlen(temp_buf) < 1)
        {
            printf ("Enter a string (Press <ENTER>): ");
            continue;
        }

        // send message to server
        if (mq_send (qd_server, temp_buf, strlen(temp_buf), 1) == -1)
        {
            perror ("Client: Not able to send message to server");
            continue;
        }

        // receive response from server
        if (mq_receive (qd_client, in_buffer, MAX_MSG_SIZE, NULL) == -1)
        {
            perror ("Client: mq_receive");
            return 1;
        }
        // display token received from server
        printf ("Client: Message received from server: %s\n", in_buffer);

        printf ("Enter a string (Press <ENTER>): ");
    }
    printf ("\n");

    // --------------------------------
    // fgets() got an EOF (^D)
    // Attempt to terminate the server
    if (mq_send (qd_server, TERMINATION_MSG, strlen(TERMINATION_MSG), 1) == -1)
    {
        perror ("Client: Not able to send message to server");
    }
    // The server is not my problem now...
    // --------------------------------

    printf ("Client: mq_close\n");
    if (mq_close (qd_client) == -1)
    {
        perror ("Client: mq_close");
        return 1;
    }
    printf ("Client: bye\n");

    return EXIT_SUCCESS;
}
