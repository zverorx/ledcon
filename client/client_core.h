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
 * @brief The main interface of the client.
 */

#ifndef CLIENT_CORE_H_SENTRY
#define CLIENT_CORE_H_SENTRY

#include <netinet/in.h>

#include "../common/ledcp.h"

/**
 * @brief Executes LED control request to server.
 * @param addr_in Server address structure.
 * @param buff LED control datagram.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int start_client(struct sockaddr_in *addr_in, struct datagram *buff);

/**
 * @brief Parses command line arguments.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param[out] addr_in Parsed server address.
 * @param[out] dg Parsed LED control datagram.
 * @return 0 on success, -1 on error.
 */
int process_args(int argc, char **argv, 
                 struct sockaddr_in **addr_in, struct datagram **dg);

#endif /* CLIENT_CORE_H_SENTRY */
