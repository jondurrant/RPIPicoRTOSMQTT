/**
 * \file            lwesp_ll_stm32_threadx.c
 * \brief           Generic STM32 driver for ThreadX, included in various STM32 driver variants
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
 * Author:          Neo Xiong <xiongyu0523@gmail.com>
 * Version:         v1.1.0-dev
 */

#include "lwesp/lwesp.h"
#include "lwesp/lwesp_mem.h"
#include "lwesp/lwesp_input.h"
#include "system/lwesp_ll.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#if !__DOXYGEN__

#if LWESP_CFG_INPUT_USE_PROCESS
#error "LWESP_CFG_INPUT_USE_PROCESS must be disabled in `lwesp_opts.h` to use this driver."
#endif /* LWESP_CFG_INPUT_USE_PROCESS */

//#define LWESP_RESET_PIN         2
#define LWESP_UART              uart0
#define LWESP_UART_IRQn         UART0_IRQ
#define LWESP_UART_TX_PIN       12
#define LWESP_UART_RX_PIN       13

static uint8_t  initialized;

static void     configure_uart(uint32_t baudrate);
static uint8_t  reset_device(uint8_t state);
static size_t   send_data(const void* data, size_t len);
static void     uart_isr(void);

/**
 * \brief           Configure UART
 */
static void
configure_uart(uint32_t baudrate) {

    if (!initialized) {

#if defined(LWESP_RESET_PIN)
        /* Configure RESET pin */
        gpio_init(LWESP_RESET_PIN);
        gpio_disable_pulls(LWESP_RESET_PIN);
        gpio_set_dir(LWESP_RESET_PIN, GPIO_OUT);
        gpio_put(LWESP_RESET_PIN, 1);
#endif /* defined(LWESP_RESET_PIN) */

        /* Configure UART */
        uart_init(LWESP_UART, baudrate);
        uart_set_format(LWESP_UART, 8, 1, UART_PARITY_NONE);
        uart_set_fifo_enabled(LWESP_UART, false);
        uart_set_hw_flow(LWESP_UART, false, false);

        /* Configure USART pins */
        gpio_set_function(LWESP_UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(LWESP_UART_RX_PIN, GPIO_FUNC_UART);

        /* Enable UART interrupts */
        irq_set_exclusive_handler(LWESP_UART_IRQn, uart_isr);
        irq_set_enabled(LWESP_UART_IRQn, true);

        /* Now enable the UART interrupts */
        uart_set_irq_enables(LWESP_UART, true, false);
    } else {
        uart_set_baudrate(LWESP_UART, baudrate);
    }
}

#if defined(LWESP_RESET_PIN)
/**
 * \brief           Hardware reset callback
 */
static uint8_t
reset_device(uint8_t state) {

    if (state) {                                /* Activate reset line */
        gpio_put(LWESP_RESET_PIN, 0);
    } else {
        gpio_put(LWESP_RESET_PIN, 1);
    }
    return 1;
}
#endif /* defined(LWESP_RESET_PIN) */

/**
 * \brief           Send data to ESP device
 * \param[in]       data: Pointer to data to send
 * \param[in]       len: Number of bytes to send
 * \return          Number of bytes sent
 */
static size_t
send_data(const void* data, size_t len) {
    uart_write_blocking(LWESP_UART, data, len);

    const char *ch;
    ch = (const char *)data;
/*
    for (int i=0 ; i < len; i++){
    	printf("%c",ch[i]);
    }
 */
    return len;
}

/**
 * \brief           Callback function called from initialization process
 */
lwespr_t
lwesp_ll_init(lwesp_ll_t* ll) {

    if (!initialized) {
        ll->send_fn = send_data;                /* Set callback function to send data */
#if defined(LWESP_RESET_PIN)
        ll->reset_fn = reset_device;            /* Set callback for hardware reset */
#endif /* defined(LWESP_RESET_PIN) */
    }

    configure_uart(ll->uart.baudrate);          /* Initialize UART for communication */
    initialized = 1;
    return lwespOK;
}

/**
 * \brief           Callback function to de-init low-level communication part
 */
lwespr_t
lwesp_ll_deinit(lwesp_ll_t* ll) {

    initialized = 0;
    LWESP_UNUSED(ll);
    return lwespOK;
}

/**
 * \brief           UART global interrupt handler
 */
void
uart_isr(void) {
    while (uart_is_readable(LWESP_UART)) {
        uint8_t ch = uart_getc(LWESP_UART);
        lwesp_input(&ch, 1);
//        printf("%c",ch);
    }
}

#endif /* !__DOXYGEN__ */
