/**
 * \file            lwesp_opts.h
 * \brief           user config file
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.0-dev
 */
#ifndef LWESP_HDR_OPTS_H
#define LWESP_HDR_OPTS_H

#define LWESP_CFG_ESP8266           1
#define LWESP_CFG_RESTORE_ON_INIT   1
#define LWESP_CFG_INPUT_USE_PROCESS 0
#define LWESP_CFG_MEM_CUSTOM        1
#define LWESP_MEM_SIZE              8192
#define LWESP_CFG_NETCONN           1
#define LWESP_CFG_DNS               1
#define LWESP_CFG_WEBSERVER         0
#define LWESP_CFG_AT_ECHO           0
#define LWESP_CFG_SNTP              1

#define LWESP_CFG_MODE_STATION		1



/* Enable global debug */
 #define LWESP_CFG_DBG               LWESP_DBG_ON

/*
* Enable debug types.
* Application may use bitwise OR | to use multiple types:
*    LWESP_DBG_TYPE_TRACE | LWESP_DBG_TYPE_STATE
*/
#define LWESP_CFG_DBG_TYPES_ON      LWESP_DBG_TYPE_TRACE

#endif /* LWESP_HDR_OPTS_H */
