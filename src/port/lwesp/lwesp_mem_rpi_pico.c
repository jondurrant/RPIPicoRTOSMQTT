/**
 * \file            lwesp_mem_rpi_pico.c
 * \brief           Dynamic memory manager implemented with LwMEM
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
#include "lwesp/lwesp.h"
#include "FreeRTOS.h"



void*
lwesp_mem_malloc(size_t size) {
    return pvPortMalloc(size);
}

void*
lwesp_mem_realloc(void* ptr, size_t size) {
	void *pointer = lwesp_mem_malloc(size);
	memcpy(pointer, ptr, size);
	lwesp_mem_free(ptr);
    return pointer;
}

void*
lwesp_mem_calloc(size_t num, size_t size) {
	size_t total = num * size;
	void *pointer = lwesp_mem_malloc(total);
	memset(pointer, 0, total);
	return pointer;
}

void
lwesp_mem_free(void* ptr) {

	vPortFree(ptr);
}


