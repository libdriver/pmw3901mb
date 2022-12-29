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
 * @file      driver_pmw3901mb_frame_test.c
 * @brief     driver pmw3901mb frame source file
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

#include "driver_pmw3901mb_frame_test.h"

static pmw3901mb_handle_t gs_handle;        /**< pmw3901mb handle */
static uint8_t gs_frame[35][35];            /**< frame array */

/**
 * @brief     frame capture test
 * @param[in] times is the test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t pmw3901mb_frame_test(uint32_t times)
{
    uint8_t res;
    uint8_t i, j;
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
    
    /* start the frame capture test */
    pmw3901mb_interface_debug_print("pmw3901mb: start frame capture test.\n");
    
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
    
    /* start frame capture */
    res = pmw3901mb_start_frame_capture(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: start frame capture failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    
    /* wait */
    while (times != 0)
    {
        /* get frame */
        res = pmw3901mb_get_frame(&gs_handle, gs_frame);
        if (res != 0)
        {
            pmw3901mb_interface_debug_print("pmw3901mb: get frame failed.\n");
            (void)pmw3901mb_deinit(&gs_handle);
            
            return 1;
        }
        
        /* print frame */
        for (i = 0; i < 35; i++)
        {
            for (j = 0; j < 35; j++)
            {
                pmw3901mb_interface_debug_print("0x%02X ", gs_frame[i][j]);
            }
            pmw3901mb_interface_debug_print("\n");
        }
        
        /* if the last */
        if (times != 1)
        {
            pmw3901mb_interface_debug_print("\n");
        }
        
        /* delay 1000 ms */
        pmw3901mb_interface_delay_ms(1000);
        
        /* times-- */
        times--;
    }
    
    /* stop frame capture */
    res = pmw3901mb_stop_frame_capture(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: stop frame capture failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    
    /* finish the frame capture test */
    pmw3901mb_interface_debug_print("pmw3901mb: finish frame capture test.\n");
    (void)pmw3901mb_deinit(&gs_handle);
    
    return 0;
}
