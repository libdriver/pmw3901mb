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
 * @file      driver_pmw3901mb_register_test.c
 * @brief     driver pmw3901mb register test source file
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

#include "driver_pmw3901mb_register_test.h"

static pmw3901mb_handle_t gs_handle;        /**< pmw3901mb handle */

/**
 * @brief  register test
 * @return status code
 *         - 0 success
 *         - 1 test failed
 * @note   none
 */
uint8_t pmw3901mb_register_test(void)
{
    uint8_t res;
    uint8_t id;
    uint8_t motion;
    int16_t delta;
    uint8_t burst;
    uint8_t squal;
    uint8_t sum;
    uint8_t max;
    uint8_t min;
    uint8_t observation;
    uint8_t grab;
    uint8_t status;
    uint16_t shutter;
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
    
    /* start the register test */
    pmw3901mb_interface_debug_print("pmw3901mb: start the register test.\n");
    
    /* init pmw3901mb */
    res = pmw3901mb_init(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: init failed.\n");
       
        return 1;
    }
    
    /* power up */
    res = pmw3901mb_power_up(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: power up failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    
    /* pmw3901mb_get_product_id test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_product_id test.\n");
    
    /* get product id */
    res = pmw3901mb_get_product_id(&gs_handle, (uint8_t *)&id);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get product id failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: product id 0x%02X.\n", id);
    
    /* pmw3901mb_get_inverse_product_id test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_inverse_product_id test.\n");
    
    /* get inverse product id */
    res = pmw3901mb_get_inverse_product_id(&gs_handle, (uint8_t *)&id);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get inverse product id failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: inverse product id 0x%02X.\n", id);
    
    /* pmw3901mb_get_revision_id test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_revision_id test.\n");
    
    /* get revision id */
    res = pmw3901mb_get_revision_id(&gs_handle, (uint8_t *)&id);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get revision id failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get revision id 0x%02X.\n", id);
    
    /* pmw3901mb_get_motion test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_motion test.\n");
    
    /* get motion */
    res = pmw3901mb_get_motion(&gs_handle, (uint8_t *)&motion);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get motion failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get motion 0x%02X.\n", motion);
    
    /* pmw3901mb_set_motion test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_set_motion test.\n");
    
    /* set motion */
    motion = 0x00;
    res = pmw3901mb_set_motion(&gs_handle, motion);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: set motion failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: check motion %s.\n", res == 0 ? "ok" : "error");
    
    /* pmw3901mb_get_delta_x test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_delta_x test.\n");
    
    /* get delta x */
    res = pmw3901mb_get_delta_x(&gs_handle, (int16_t *)&delta);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get delta x failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: delta x is %d.\n", delta);
    
    /* pmw3901mb_get_delta_y test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_delta_y test.\n");
    
    /* get delta y */
    res = pmw3901mb_get_delta_y(&gs_handle, (int16_t *)&delta);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get delta y failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: delta y is %d.\n", delta);
    
    /* pmw3901mb_get_motion_burst test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_motion_burst test.\n");
    
    /* get motion burst */
    res = pmw3901mb_get_motion_burst(&gs_handle, (uint8_t *)&burst, 1);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get motion burst failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: motion burst is 0x%02X.\n", burst);
    
    /* pmw3901mb_get_squal test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_squal test.\n");
    
    /* get squal */
    res = pmw3901mb_get_squal(&gs_handle, (uint8_t *)&squal);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get squal failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get squal is 0x%02X.\n", squal);
    
    /* pmw3901mb_get_raw_data_sum test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_raw_data_sum test.\n");
    
    /* get raw data sum */
    res = pmw3901mb_get_raw_data_sum(&gs_handle, (uint8_t *)&sum);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get raw data sum failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get raw data sum is 0x%02X.\n", sum);
    
    /* pmw3901mb_get_max_raw_data test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_max_raw_data test.\n");
    
    /* get max raw data */
    res = pmw3901mb_get_max_raw_data(&gs_handle, (uint8_t *)&max);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get max raw data failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get max raw data is 0x%02X.\n", max);
    
    /* pmw3901mb_get_min_raw_data test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_min_raw_data test.\n");
    
    /* get min raw data */
    res = pmw3901mb_get_min_raw_data(&gs_handle, (uint8_t *)&min);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get min raw data failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get min raw data is 0x%02X.\n", min);
    
    /* pmw3901mb_get_shutter test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_shutter test.\n");
    
    /* get shutter */
    res = pmw3901mb_get_shutter(&gs_handle, (uint16_t *)&shutter);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get shutter failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get shutter is 0x%04X.\n", shutter);
    
    /* pmw3901mb_get_observation test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_observation test.\n");
    
    /* get observation */
    res = pmw3901mb_get_observation(&gs_handle, (uint8_t *)&observation);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get observation failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get observation is 0x%02X.\n", observation);
    
    /* pmw3901mb_set_observation test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_set_observation test.\n");
    
    /* set observation */
    observation = 0x00;
    res = pmw3901mb_set_observation(&gs_handle, observation);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: set observation failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: check observation %s.\n", res == 0 ? "ok" : "error");
    
    /* pmw3901mb_get_raw_data_grab test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_raw_data_grab test.\n");
    
    /* get raw data grab */
    res = pmw3901mb_get_raw_data_grab(&gs_handle, (uint8_t *)&grab, 1);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get raw data grab failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get grab is 0x%02X.\n", grab);
    
    /* pmw3901mb_set_raw_data_grab test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_set_raw_data_grab test.\n");
    
    /* set raw data grab */
    grab = 0x00;
    res = pmw3901mb_set_raw_data_grab(&gs_handle, (uint8_t *)&grab, 1);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: set raw data grab failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: check grab %s.\n", res == 0 ? "ok" : "error");
    
    /* pmw3901mb_get_raw_data_grab_status test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_get_raw_data_grab_status test.\n");
    
    /* get raw data grab status */
    res = pmw3901mb_get_raw_data_grab_status(&gs_handle, (uint8_t *)&status);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: get raw data grab status failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: get status is 0x%02X.\n", status);
    
    /* pmw3901mb_reset test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_reset test.\n");
    
    /* reset */
    res = pmw3901mb_reset(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: reset failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: check reset %s.\n", res == 0 ? "ok" : "error");
    
    /* pmw3901mb_shutdown test */
    pmw3901mb_interface_debug_print("pmw3901mb: pmw3901mb_shutdown test.\n");
    
    /* shutdown */
    res = pmw3901mb_shutdown(&gs_handle);
    if (res != 0)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: shutdown failed.\n");
        (void)pmw3901mb_deinit(&gs_handle);
        
        return 1;
    }
    pmw3901mb_interface_debug_print("pmw3901mb: check shutdown %s.\n", res == 0 ? "ok" : "error");
    
    /* finish the register test */
    pmw3901mb_interface_debug_print("pmw3901mb: finish the register test.\n");
    (void)pmw3901mb_deinit(&gs_handle);
    
    return 0;
}
