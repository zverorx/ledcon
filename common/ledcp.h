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
 * @brief LED Control Protocol. Contains a definition of the structures 
 *        for communication between the client and the server.
 */

#ifndef LEDCP_H_SENTRY
#define LEDCP_H_SENTRY

/**
 * @enum
 * @brief Server success or failure.
 * @see struct response_datagram
 */
enum status {
    STAT_SUCCESS    = 0,
    STAT_FAILURE    = 1
};

/**
 * @enum
 * @brief A set of LED colors on the server.
 * @see struct datagram
 */
enum color {
    green   = 0,
    red     = 1
};

/**
 * @enum
 * @brief LED mode.
 * @see struct datagram
 */
enum mode {
    on  = 0,
    off = 1
};

/**
 * @struct
 * @brief A datagram for the server containing information 
 *        about which LED to turn on or off.
 */
struct datagram {
    enum color color;
    enum mode mode;
};

/**
 * @struct
 * @brief Reply datagram to the client.
 */
struct response_datagram {
    enum status code;
};

#endif /* LEDCP_H_SENTRY */