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

static volatile int run = 1;

int start_server(int port)
{
    int sockfd = -1;
    int res_bind, res_recv, res_switch;
    struct sockaddr_in addr_in, sender_addr_in;
    struct sigaction sa;
    struct datagram *buff_from = NULL;
    struct response_datagram *buff_to = NULL;
    socklen_t addrlen;

    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &signal_handle;
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) { goto handle_error; }

    memset(&addr_in, 0, sizeof(struct sockaddr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = INADDR_ANY;

    res_bind = bind(sockfd, (struct sockaddr *) &addr_in, sizeof(addr_in));
    if (res_bind == -1) { goto handle_error; }

    buff_from = calloc(1, sizeof(struct datagram));
    if (!buff_from) { goto handle_error; }
    buff_to = calloc(1, sizeof(struct response_datagram));
    if (!buff_to) { goto handle_error; }

    while (run) {
        buff_to->code = STAT_SUCCESS;
        memset(buff_from, 0, sizeof(struct datagram));

        addrlen = sizeof(struct sockaddr_in);
        res_recv = recvfrom(sockfd, buff_from, sizeof(struct datagram), 0, 
                            (struct sockaddr *) &sender_addr_in, &addrlen);
        if (res_recv == -1) { continue; } 
        else if (res_recv != sizeof(struct datagram)) {
            buff_to->code = STAT_FAILURE;
        }
        else {
            res_switch = led_switch(buff_from);
            if (res_switch == -1) { buff_to->code = STAT_FAILURE; }
        }

        sendto(sockfd, buff_to, sizeof(struct response_datagram), 0,
               (struct sockaddr *) &sender_addr_in, addrlen);
    }

    close(sockfd);
    free(buff_from);
    free(buff_to);
    return EXIT_SUCCESS;

    handle_error:
        close(sockfd);
        free(buff_from);
        free(buff_to);
        return EXIT_FAILURE;
}

static void signal_handle(int signum) { run = 0; }

static int led_switch(struct datagram *dg)
{
    /* TODO: getenv() for GREEN_DEV/RED_DEV */
    char *path_to_dev = NULL;
    char buff[1];
    int fd = -1, res_write;

    if (!dg) { goto handle_error; }

    switch (dg->color) {
        case green:
            path_to_dev = "/sys/class/leds/green:status/brightness";
            break;
        case red:
            path_to_dev = "/sys/class/leds/red:power/brightness";
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
