/*! ----------------------------------------------------------------------------
 * @file    deca_spi.c
 * @brief   SPI access functions
 *
 * @copyright
 * Copyright 2015 (c) DecaWave Ltd, Dublin, Ireland.
 * Copyright 2019 (c) Frederic Mes, RTLOC.
 * Copyright 2019 (c) Callender-Consulting LLC.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#include "deca_spi.h"

#include <errno.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/irq.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(deca_spi);


static const struct spi_dt_spec spi_dev = SPI_DT_SPEC_GET(DT_NODELABEL(dw1000), SPI_WORD_SET(8) | SPI_TRANSFER_MSB);

static const struct gpio_dt_spec dw_wakeup = GPIO_DT_SPEC_GET(DT_NODELABEL(dw1000), wakeup_gpios);
static const struct gpio_dt_spec int_gpio = GPIO_DT_SPEC_GET(DT_NODELABEL(dw1000), int_gpios);
static const struct gpio_dt_spec rst_gpio = GPIO_DT_SPEC_GET(DT_NODELABEL(dw1000), reset_gpios);
#if DT_NODE_EXISTS(DT_NODELABEL(xtal_enable))
static const struct gpio_dt_spec xtal_enable = GPIO_DT_SPEC_GET(DT_NODELABEL(xtal_enable), gpios);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(dw_cs1))
static const struct gpio_dt_spec cs1 = GPIO_DT_SPEC_GET(DT_NODELABEL(dw_cs1), gpios);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(en_1v8))
static const struct gpio_dt_spec en_1v8 = GPIO_DT_SPEC_GET(DT_NODELABEL(en_1v8), gpios);
#endif

const struct spi_config *spi_cfg;
static struct spi_config spi_cfg_slow;
static struct gpio_callback gpio_cb;

int dw1000_init(void)
{
    int ret = 0;
    ret = spi_is_ready_dt(&spi_dev);
    if (!ret) {
        LOG_ERR("SPI device not ready");
        return -ENODEV;
    }
    memcpy(&spi_cfg_slow, &spi_dev.config, sizeof(spi_cfg_slow));
    dw1000_set_spi_slow();

    ret = gpio_is_ready_dt(&rst_gpio);
    if (!ret) {
        LOG_ERR("RST GPIO device not ready");
        return -ENODEV;
    }
    ret = gpio_pin_configure_dt(&rst_gpio, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Failed to configure RST GPIO pin: %d", ret);
        return ret;
    }

#if DT_NODE_EXISTS(DT_NODELABEL(xtal_enable))
    if (!gpio_is_ready_dt(&xtal_enable)) {
		LOG_ERR("XTAL enable device not ready");
		return -ENODEV;
	}
	ret = gpio_pin_configure_dt(&xtal_enable, GPIO_OUTPUT_ACTIVE);
	if(ret < 0) {
		LOG_ERR("Failed to configure XTAL enable pin");
		return ret;
	}
	gpio_pin_set_dt(&xtal_enable, 1);
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(dw_cs1))
    if (!gpio_is_ready_dt(&cs1)) {
        LOG_ERR("CS GPIO device not ready");
        return -ENODEV;
    }
    ret = gpio_pin_configure_dt(&cs1, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure CS GPIO pin: %d", ret);
        return ret;
    }
    gpio_pin_set_dt(&cs1, 0);
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(en_1v8))
    if (!gpio_is_ready_dt(&en_1v8)) {
        LOG_ERR("EN 1.8V GPIO device not ready");
        return -ENODEV;
    }
    ret = gpio_pin_configure_dt(&en_1v8, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure EN 1.8V GPIO pin: %d", ret);
        return ret;
    }
    gpio_pin_set_dt(&en_1v8, 1);
#endif

    return 0;
}

void dw1000_reset(void)
{
    gpio_pin_configure_dt(&rst_gpio, GPIO_OUTPUT_ACTIVE);
    k_msleep(1); // Hold reset for at least 1ms
    gpio_pin_set_dt(&rst_gpio, 0);
    k_msleep(5);

    gpio_pin_configure_dt(&rst_gpio, GPIO_INPUT);
}

int port_set_deca_isr(port_deca_isr_t deca_isr) {
    int ret = 0;

    ret = gpio_is_ready_dt(&int_gpio);
    if (!ret) {
        LOG_ERR("INT GPIO device not ready");
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(&int_gpio, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Failed to configure INT GPIO pin: %d", ret);
        return ret;
    }
    
    gpio_pin_interrupt_configure_dt(&int_gpio, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&gpio_cb, (gpio_callback_handler_t)deca_isr, BIT(int_gpio.pin));
    gpio_add_callback(int_gpio.port, &gpio_cb);

    return 0;
}

void dw1000_set_spi_slow(void)
{
    spi_cfg_slow.frequency = 2000000;
    spi_cfg = &spi_cfg_slow;
}

void dw1000_set_spi_fast(void)
{
    spi_cfg = &spi_dev.config;
}

/*
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success
 */
int writetospi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t bodyLength,
               const uint8_t *bodyBuffer)
{
#if 0
    LOG_HEXDUMP_INF(headerBuffer, headerLength, "writetospi: Header");
    LOG_HEXDUMP_INF(bodyBuffer, bodyLength, "writetospi: Body");
#endif

    const struct spi_buf tx_bufs[2] = {
        {
            .buf = (uint8_t *)headerBuffer,
            .len = headerLength
        },
        {
            .buf = (uint8_t *)bodyBuffer,
            .len = bodyLength
        }
    };
    const struct spi_buf_set tx = {
        .buffers = tx_bufs,
        .count = 2
    };

    const struct spi_buf_set rx = {
        .buffers = NULL,
        .count = 0
    };

    int ret = spi_transceive(spi_dev.bus, spi_cfg, &tx, &rx);

    return ret;
}

/*
 * Function: readfromspi()
 *
 * Low level abstract function to read from the SPI
 * Takes two separate byte buffers for write header and read data
 * returns the offset into read buffer where first byte of read data
 * may be found, or returns 0
 */
int readfromspi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t readLength,
                uint8_t *readBuffer)
{
    const struct spi_buf tx_bufs = {
        .buf = (uint8_t *)headerBuffer,
        .len = headerLength
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_bufs,
        .count = 1
    };

    struct spi_buf rx_bufs[2] = {
        {
            .buf = NULL,
            .len = headerLength
        },
        {
            .buf = readBuffer,
            .len = readLength
        }
    };
    const struct spi_buf_set rx = {
        .buffers = rx_bufs,
        .count = 2
    };

    int ret = spi_transceive(spi_dev.bus, spi_cfg, &tx, &rx);

#if 0
    LOG_HEXDUMP_INF(headerBuffer, headerLength, "readfromspi: Header");
    LOG_HEXDUMP_INF(readBuffer, readLength, "readfromspi: Body");
#endif

    return ret;
}
