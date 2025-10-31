/*! ----------------------------------------------------------------------------
 * @file    ex_10a_main.c
 * @brief   This example demonstrates how to enable DW IC GPIOs as inputs 
 *           and outputs.
 *           And drive the output to turn on/off LED on EVB1000 HW.
 *
 *           GPIO2 will be used to flash the RXOK LED (LED4 on EVB1000 HW)
 *           GPIO5 and GPIO6 are configured as inputs, toggling S3-3 and 
 *           S3-4 will change them:
 *           S3-3 is connected to GPIO5 and S3-4 to GPIO6
 *
 *           NOTE!!! The switch S3-3 and S3-4 on EVB1000 HW should be OFF 
 *                   before the example is run to make sure the DW1000 SPI 
 *                   mode is set to 0 on IC start up.
 * @copyright
 * Copyright 2017 (c) Decawave Ltd, Dublin, Ireland.
 * Copyright 2019 (c) Frederic Mes, RTLOC.
 *
 * All rights reserved.
 *
 * @author Decawave
 * 
 * @note
 * 1. When enabling the GPIO mode/value, the GPIO clock needs to be enabled 
 *    and GPIO reset set.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#include "deca_device_api.h"
#include "deca_spi.h"


#define LOG_LEVEL 3 
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

/* Example application name and version to display on console. */
#define APP_NAME "GPIO        v1.1"

/**
 * Application entry point.
 */
int main(void)
{
    /* Display application name on console. */
    printk(APP_NAME);

    /* Configure DW1000 SPI */
    dw1000_init();
    
    /* During initialisation and continuous frame mode activation, 
     *  DW1000 clocks must be set to crystal speed so SPI rate have to be 
     *  lowered and will not be increased again in this example.
     */
    dw1000_set_spi_slow();

    /* NOTE!!! The switch S3-3 and S3-4 on EVB1000 HW should be OFF at 
     * this point to make sure the DW1000 SPI mode is set to 0 on 
     * IC start up
     */

    /* Reset and initialise DW1000 */

    /* Target specific drive of RSTn line into DW1000 low for a period. */
    dw1000_reset();

    if (dwt_initialise(DWT_LOADNONE) == DWT_ERROR) {
        printk("INIT FAILED");
        k_sleep(K_MSEC(500));
        while (1) { /* spin */ };
    }

    dw1000_set_spi_fast();

    /* See NOTE 1: 1st enable GPIO clocks */
    dwt_enablegpioclocks();

    /*
     * GPIO2 will be used to flash the RXOK LED (LED4 on EVB1000 HW)
     *
     * GPIO5 and GPIO6 are configured as inputs, toggling S3-3 and
     * S3-4 will change their values:
     *     S3-3 is connected to GPIO5 and S3-4 to GPIO6
     */
    dwt_setgpiodirection(DWT_GxM2 | DWT_GxM6 | DWT_GxM5, DWT_GxP6 | DWT_GxP5);

    while (1) {

        /* Set GPIO2 high (LED4 will light up) */
        dwt_setgpiovalue(DWT_GxM2, DWT_GxP2); 

        /* Set GPIO6 is high use short Sleep ON period */
        if (dwt_getgpiovalue(DWT_GxP6))
            k_msleep(100);
        else
            k_msleep(400);

        dwt_setgpiovalue(DWT_GxM2, 0); /* set GPIO2 low (LED4 will be off) */

        /* Set GPIO5 is high use short Sleep OFF period */
        if (dwt_getgpiovalue(DWT_GxP5))
            k_msleep(100);
        else
            k_msleep(400);
    }
}
