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
 * @file      driver_pmw3901mb_basic.c
 * @brief     driver pmw3901mb basic source file
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

#include "driver_pmw3901mb_basic.h"

static pmw3901mb_handle_t gs_handle;        /**< pmw3901mb handle */

/**
 * @brief  basic example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t pmw3901mb_basic_init(void)
{
    uint8_t res;

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

    return 0;
}

/**
 * @brief      basic example read
 * @param[in]  height_m is the height(m)
 * @param[out] *motion points to a motion structure
 * @param[out] *delta_x points to a delta_x buffer
 * @param[out] *delta_y points to a delta_y buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t pmw3901mb_basic_read(float height_m, pmw3901mb_motion_t *motion, float *delta_x, float *delta_y)
{
    uint8_t res;

    if (pmw3901mb_burst_read(&gs_handle, motion) != 0)
    {
        return 1;
    }
    else
    {
        if (motion->is_valid == 1)
        {
            /* convert the delta x */
            res = pmw3901mb_delta_raw_to_delta_cm(&gs_handle, motion->delta_x, height_m, (float *)delta_x);
            if (res != 0)
            {
                return 1;
            }

            /* convert the delta y */
            res = pmw3901mb_delta_raw_to_delta_cm(&gs_handle, motion->delta_y, height_m, (float *)delta_y);
            if (res != 0)
            {
                return 1;
            }
        }
        else
        {
            *delta_x = 0;
            *delta_y = 0;
        }

        return 0;
    }
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t pmw3901mb_basic_deinit(void)
{
    if (pmw3901mb_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
