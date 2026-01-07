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
 * @brief The main interface of the server.
 */

#include "../common/ledcp.h"

/**
 * @brief Creating a socket, starting an endless 
 *        cycle of receiving a datagram.
 * @param port The port that the socket will bind to.
 * @return In case of a critical error EXIT_FAILURE, 
 *         when receiving the signal EXIT_SUCCESS.
 */
int start_server(int port);

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