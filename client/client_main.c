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
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../common/ledcp.h"

int process_args(int argc, char **argv, struct sockaddr_in **addr_in, struct datagram **dg)
{
    char *ip = NULL;
    char *str_port = NULL;
    char *saveptr = NULL;
    int port;
    int res_inet_aton;
    char *sockaddr = NULL;

    if (argc != 4 || !argv || 
        *addr_in || *dg || 
        !addr_in || !dg) 
        { goto handle_error; }

    sockaddr = strdup(argv[1]);
    if (!sockaddr) { goto handle_error; }

    *addr_in = calloc(1, sizeof(struct sockaddr_in));
    if (!*addr_in) { goto handle_error; }

    *dg = calloc(1, sizeof(struct datagram));
    if (!*dg) { goto handle_error; }

    ip = strtok_r(sockaddr, ":", &saveptr);
    if (!ip) { goto handle_error; }
    str_port = strtok_r(NULL, ":", &saveptr);
    if (!str_port) { goto handle_error; }
    port = atoi(str_port);
    if (!port) { goto handle_error; }

    if (strcmp(argv[2], "green") == 0) { (*dg)->color = green; }
    else if (strcmp(argv[2], "red") == 0) { (*dg)->color = red; }
    else { goto handle_error; }

    if (strcmp(argv[3], "on") == 0) { (*dg)->mode = on; }
    else if (strcmp(argv[3], "off") == 0) { (*dg)->mode = off; }
    else { goto handle_error; }

    (*addr_in)->sin_family = AF_INET;
    res_inet_aton = inet_aton(ip, &(*addr_in)->sin_addr);
    if (!res_inet_aton) { goto handle_error; }
    (*addr_in)->sin_port = htons(port);

    free(sockaddr);
    return 0;

    handle_error:
        free(sockaddr);
        free(*addr_in);
        free(*dg);
        return -1;
}

int main(int argc, char **argv)
{
    int sockfd;
    int res_proc_arg, res_recv, res_send, res_ssop;
    struct sockaddr_in *dest_addr_in = NULL;
    struct datagram *send_buff = NULL;
    struct response_datagram *recv_buff = NULL;
    struct timeval tv;
    socklen_t addrlen;

    res_proc_arg = process_args(argc, argv, &dest_addr_in, &send_buff);
    if (res_proc_arg == -1) { 
        fputs("Usage: ledcon-client <ip:port> <green|red> <on|off>\n"
              "ip:port - server socket address\n"
              "green|red - microcomputer LED\n"
              "on|off - LED mode\n", stderr); 
        goto handle_error; 
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) { perror("ledcon-client"); goto handle_error; }

    memset(&tv, 0, sizeof(struct timeval));
    tv.tv_sec = 3;
    tv.tv_usec = 0;

    res_ssop = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (res_ssop == -1) { perror("ledcon-client"); goto handle_error; }

    addrlen = sizeof(struct sockaddr_in);
    res_send = sendto(sockfd, send_buff, sizeof(struct datagram), 0,
                      (struct sockaddr *) dest_addr_in, addrlen);
    if (res_send == -1) { perror("ledcon-client"); goto handle_error; }

    recv_buff = calloc(1, sizeof(struct response_datagram));
    if (!recv_buff) { 
        errno = ENOMEM;
        perror("ledcon_client");
        goto handle_error; 
    }

    res_recv = recvfrom(sockfd, recv_buff, sizeof(struct response_datagram), 0,
                        (struct sockaddr *) dest_addr_in, &addrlen);
    if (res_recv == -1) { perror("ledcon-client"); goto handle_error; }

    if (recv_buff->code == STAT_FAILURE) {
        fputs("ledcon-client: Error on the server side\n", stderr);
        goto handle_error;
    }

    close(sockfd);
    free(dest_addr_in);
    free(recv_buff);
    free(send_buff);
    return EXIT_SUCCESS;

    handle_error:
        close(sockfd);
        free(dest_addr_in);
        free(recv_buff);
        free(send_buff);
        return EXIT_FAILURE;
}
