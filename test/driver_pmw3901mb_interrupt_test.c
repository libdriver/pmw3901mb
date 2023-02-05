/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      driver_pmw3901mb_interrupt_test.c
 * @brief     driver pmw3901mb interrupt test source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2022-01-08
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2022/01/08  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_pmw3901mb_interrupt_test.h"

static pmw3901mb_handle_t gs_handle;        /**< pmw3901mb handle */
static volatile uint8_t gs_flag;            /**< inner flag */

/**
 * @brief     interrupt test irq
 * @param[in] height_m is the height(m)
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 * @note      none
 */
uint8_t pmw3901mb_interrupt_test_irq_handler(float height_m)
{
    uint8_t res;
    float delta_x;
    float delta_y;
    pmw3901mb_motion_t motion;

    /* burst read */
    res = pmw3901mb_burst_read(&gs_handle, &motion);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: burst read failed.\n");
        (void)pmw3901mb_set_motion(&gs_handle, 0x00);

        return 1;
    }

    /* check the result */
    if (motion.is_valid == 1)
    {
        /* convert the delta x */
        res = pmw3901mb_delta_raw_to_delta_cm(&gs_handle, motion.delta_x, height_m, (float *)&delta_x);
        if (res != 0)
        {
            pmw3901mb_interface_debug_print("pmw3901mb: delta raw to delta cm failed.\n");
            (void)pmw3901mb_set_motion(&gs_handle, 0x00);

            return 1;
        }

        /* convert the delta y */
        res = pmw3901mb_delta_raw_to_delta_cm(&gs_handle, motion.delta_y, height_m, (float *)&delta_y);
        if (res != 0)
        {
            pmw3901mb_interface_debug_print("pmw3901mb: delta raw to delta cm failed.\n");
            (void)pmw3901mb_set_motion(&gs_handle, 0x00);

            return 1;
        }

        /* print the result */
        pmw3901mb_interface_debug_print("pmw3901mb: find interrupt.\n");
        pmw3901mb_interface_debug_print("pmw3901mb: delta_x: %0.3fcm delta_y: %0.3fcm.\n", delta_x, delta_y);
        pmw3901mb_interface_debug_print("pmw3901mb: raw_average is 0x%02X.\n", motion.raw_average);
        pmw3901mb_interface_debug_print("pmw3901mb: raw_max is 0x%02X.\n", motion.raw_max);
        pmw3901mb_interface_debug_print("pmw3901mb: raw_min is 0x%02X.\n", motion.raw_min);
        pmw3901mb_interface_debug_print("pmw3901mb: observation is 0x%02X.\n", motion.observation);
        pmw3901mb_interface_debug_print("pmw3901mb: shutter is 0x%04X.\n", motion.shutter);
        pmw3901mb_interface_debug_print("pmw3901mb: surface quality is 0x%04X.\n", motion.surface_quality);

        /* set flag */
        gs_flag = 1;
    }

    /* clear the interrupt flag */
    res = pmw3901mb_set_motion(&gs_handle, 0x00);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: set motion failed.\n");

        return 1;
    }

    return 0;
}

/**
 * @brief     interrupt test
 * @param[in] times is the test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t pmw3901mb_interrupt_test(uint32_t times)
{
    uint8_t res;
    pmw3901mb_info_t info;

    /* link interface function */
    DRIVER_PMW3901MB_LINK_INIT(&gs_handle, pmw3901mb_handle_t);
    DRIVER_PMW3901MB_LINK_SPI_INIT(&gs_handle, pmw3901mb_interface_spi_init);
    DRIVER_PMW3901MB_LINK_SPI_DEINIT(&gs_handle, pmw3901mb_interface_spi_deinit);
    DRIVER_PMW3901MB_LINK_SPI_READ(&gs_handle, pmw3901mb_interface_spi_read);
    DRIVER_PMW3901MB_LINK_SPI_WRITE(&gs_handle, pmw3901mb_interface_spi_write);
    DRIVER_PMW3901MB_LINK_RESET_GPIO_INIT(&gs_handle, pmw3901mb_interface_reset_gpio_init);
    DRIVER_PMW3901MB_LINK_RESET_GPIO_DEINIT(&gs_handle, pmw3901mb_interface_reset_gpio_deinit);
    DRIVER_PMW3901MB_LINK_RESET_GPIO_WRITE(&gs_handle, pmw3901mb_interface_reset_gpio_write);
    DRIVER_PMW3901MB_LINK_DELAY_MS(&gs_handle, pmw3901mb_interface_delay_ms);
    DRIVER_PMW3901MB_LINK_DEBUG_PRINT(&gs_handle, pmw3901mb_interface_debug_print);

    /* get information */
    res = pmw3901mb_info(&info);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get info failed.\n");

        return 1;
    }
    else
    {
        /* print chip info */
        pmw3901mb_interface_debug_print("pmw3901mb: chip is %s.\n", info.chip_name);
        pmw3901mb_interface_debug_print("pmw3901mb: manufacturer is %s.\n", info.manufacturer_name);
        pmw3901mb_interface_debug_print("pmw3901mb: interface is %s.\n", info.interface);
        pmw3901mb_interface_debug_print("pmw3901mb: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        pmw3901mb_interface_debug_print("pmw3901mb: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        pmw3901mb_interface_debug_print("pmw3901mb: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        pmw3901mb_interface_debug_print("pmw3901mb: max current is %0.2fmA.\n", info.max_current_ma);
        pmw3901mb_interface_debug_print("pmw3901mb: max temperature is %0.1fC.\n", info.temperature_max);
        pmw3901mb_interface_debug_print("pmw3901mb: min temperature is %0.1fC.\n", info.temperature_min);
    }

    /* start the interrupt test */
    pmw3901mb_interface_debug_print("pmw3901mb: start the interrupt test.\n");

    /* init pmw3901mb */
    res = pmw3901mb_init(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: init failed.\n");

        return 1;
    }

    /* chip power up */
    res = pmw3901mb_power_up(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: power up failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);

        return 1;
    }

    /* set optimum performance */
    res = pmw3901mb_set_optimum_performance(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: set optimum performance failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);

        return 1;
    }

    /* wait */
    while (times != 0)
    {
        gs_flag = 0x00;
        while (gs_flag == 0x00)
        {
            pmw3901mb_interface_delay_ms(10);
        }
        times--;
    }

    /* finish the interrupt test */
    pmw3901mb_interface_debug_print("pmw3901mb: finish the interrupt test.\n");
    (void)pmw3901mb_deinit(&gs_handle);

    return 0;
}
