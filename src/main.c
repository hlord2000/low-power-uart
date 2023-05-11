/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/pm/device.h>

#include <string.h>

/* change this to any other UART peripheral if desired */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

#define MSG_SIZE 32


static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
uart_callback_t uart_callback(const struct device *dev, struct uart_event *evt, void *user_data) {
	printk("Event: %d\n", evt->type);
}

/*
 * Print a null-terminated string character by character to the UART interface
 */
void print_uart(char *buf)
{
	int err;
	int msg_len = strlen(buf);
	err = uart_tx(uart_dev, buf, msg_len, SYS_FOREVER_MS);
	if (err) {
		printk("UART TX error: %d\n", err);
	}
}

void main(void)
{

	if (!device_is_ready(uart_dev)) {
		printk("UART device not found!");
		return;
	}

	char tx_buf[MSG_SIZE];

	uart_rx_enable(uart_dev, tx_buf, MSG_SIZE, 1000);

	int err;

	//err = uart_callback_set(uart_dev, uart_callback, NULL);
	if (err) {
		printk("Cannot set UART callback: %d\n", err);
		return;
	}


	/* configure interrupt and callback to receive data */

	print_uart("Hello world!\r\n");
	k_sleep(K_SECONDS(1));

	/* Disable UART */
	err = uart_rx_disable(uart_dev);
	// Will need to await uart_rx_disable event to return before turning everything off. Handle in callback.
	// I just put a busy wait here for demonstration
	k_sleep(K_SECONDS(1));
	if (err) {
		printk("Cannot disable UART: %d\n", err);
		return;
	}
	err = pm_device_action_run(uart_dev, PM_DEVICE_ACTION_SUSPEND);
	err = pm_device_action_run(uart_dev, PM_DEVICE_ACTION_TURN_OFF);
	while (true) {
		k_sleep(K_SECONDS(5));
	}
}