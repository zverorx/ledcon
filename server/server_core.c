/* 
 * This file is part of ledcon.
 * ledcon - Server and client for controlling LEDs 
 *          on an orange pi microcomputer.
 *  
 * Copyright (C) 2026 Egorov Konstantin
 *
 * ledcon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ledcon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ledcon. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Main Server operations.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "../common/ledcp.h"
#include "server_core.h"

/**
 * @brief SIGTERM and SIGINT signal handler for exiting 
 *        an infinite loop while waiting for a datagram.
 * @param signum Signal number (SIGTERM or SIGINT).
 */
static void signal_handle(int signum);

/**
 * @brief Turns on or off the desired LED.
 * @param dg Datagram from the client.
 * @return 0 on success, -1 otherwise.
 */
static int led_switch(struct datagram *dg);

/**
 * @brief Server running flag (1=running, 0=stop on SIGTERM/SIGINT).
 * 
 * @note Used in while() condition of the main UDP socket listening loop.
 */
static volatile int run = 1;

int start_server(int port)
{
    int sockfd = -1;
    int res_bind, res_recv, res_switch;
    struct sockaddr_in own_addr_in, sender_addr_in;
    struct sigaction sa;
    struct datagram *recv_buff = NULL;
    struct response_datagram *send_buff = NULL;
    socklen_t addrlen;

    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &signal_handle;
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) { goto handle_error; }

    memset(&sender_addr_in, 0, sizeof(struct response_datagram));
    memset(&own_addr_in, 0, sizeof(struct sockaddr_in));
    own_addr_in.sin_family = AF_INET;
    own_addr_in.sin_addr.s_addr = INADDR_ANY;
    own_addr_in.sin_port = htons(port);

    res_bind = bind(sockfd, (struct sockaddr *) &own_addr_in, sizeof(own_addr_in));
    if (res_bind == -1) { goto handle_error; }

    recv_buff = calloc(1, sizeof(struct datagram));
    if (!recv_buff) { goto handle_error; }
    send_buff = calloc(1, sizeof(struct response_datagram));
    if (!send_buff) { goto handle_error; }

    while (run) {
        send_buff->code = STAT_SUCCESS;
        memset(recv_buff, 0, sizeof(struct datagram));

        addrlen = sizeof(struct sockaddr_in);
        res_recv = recvfrom(sockfd, recv_buff, sizeof(struct datagram), 0, 
                            (struct sockaddr *) &sender_addr_in, &addrlen);
        if (res_recv == -1) { continue; } 
        else if (res_recv != sizeof(struct datagram)) {
            send_buff->code = STAT_FAILURE;
        }
        else {
            res_switch = led_switch(recv_buff);
            if (res_switch == -1) { send_buff->code = STAT_FAILURE; }
        }

        sendto(sockfd, send_buff, sizeof(struct response_datagram), 0,
               (struct sockaddr *) &sender_addr_in, addrlen);
    }

    close(sockfd);
    free(recv_buff);
    free(send_buff);
    return EXIT_SUCCESS;

    handle_error:
        close(sockfd);
        free(recv_buff);
        free(send_buff);
        return EXIT_FAILURE;
}

static void signal_handle(int signum) { run = 0; }

static int led_switch(struct datagram *dg)
{
    const char *path_to_dev = NULL;
    char buff[1];
    int fd = -1, res_write;

    if (!dg) { goto handle_error; }

    switch (dg->color) {
        case green:
            path_to_dev = getenv("GREEN_DEV");
            if (!path_to_dev) { 
                path_to_dev = "/sys/class/leds/green:status/brightness";
            } 
            break;
        case red:
            path_to_dev = getenv("RED_DEV");
            if (!path_to_dev) { 
                path_to_dev = "/sys/class/leds/red:power/brightness";
            }
            break;
        default: goto handle_error;
    }

    fd = open(path_to_dev, O_WRONLY);
    if (fd == -1) { goto handle_error; }

    switch (dg->mode) {
        case on: buff[0] = '1'; break;
        case off: buff[0] = '0'; break;
        default: goto handle_error;
    }

    res_write = write(fd, buff, sizeof(buff));
    if (res_write == -1) { goto handle_error; }

    close(fd);
    return 0;

    handle_error:
        close(fd);
        return -1;
}
