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
 * @file      driver_pmw3901mb.h
 * @brief     driver pmw3901mb header file
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

#ifndef DRIVER_PMW3901MB_H
#define DRIVER_PMW3901MB_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup pmw3901mb_driver pmw3901mb driver function
 * @brief    pmw3901mb driver modules
 * @{
 */

/**
 * @addtogroup pmw3901mb_basic_driver
 * @{
 */

/**
 * @brief pmw3901mb motion structure definition
 */
typedef struct pmw3901mb_motion_s
{
    uint8_t raw[12];                 /**< raw buffer */
    int16_t delta_x;                 /**< delta_x */
    int16_t delta_y;                 /**< delta_y */
    uint16_t surface_quality;        /**< surface quality */
    uint8_t raw_max;                 /**< raw max */
    uint8_t raw_average;             /**< raw average */
    uint8_t raw_min;                 /**< raw min */
    uint8_t observation;             /**< observation */
    uint16_t shutter;                /**< shutter */
    uint8_t is_valid;                /**< valid flag, 0 meas invalid, 1 meas invalid, 2 meas inner errors */
} pmw3901mb_motion_t;

/**
 * @brief pmw3901mb handle structure definition
 */
typedef struct pmw3901mb_handle_s
{
    uint8_t (*reset_gpio_init)(void);                                     /**< point to a reset_gpio_init function address */
    uint8_t (*reset_gpio_deinit)(void);                                   /**< point to a reset_gpio_deinit function address */
    uint8_t (*reset_gpio_write)(uint8_t value);                           /**< point to a reset_gpio_write function address */
    uint8_t (*spi_init)(void);                                            /**< point to a spi_init function address */
    uint8_t (*spi_deinit)(void);                                          /**< point to a spi_deinit function address */
    uint8_t (*spi_read)(uint8_t reg, uint8_t *buf, uint16_t len);         /**< point to a spi_read function address */
    uint8_t (*spi_write)(uint8_t reg, uint8_t *buf, uint16_t len);        /**< point to a spi_write function address */
    void (*delay_ms)(uint32_t ms);                                        /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...);                      /**< point to a debug_print function address */
    uint8_t inited;                                                       /**< inited flag */
} pmw3901mb_handle_t;

/**
 * @brief pmw3901mb information structure definition
 */
typedef struct pmw3901mb_info_s
{
    char chip_name[32];                /**< chip name */
    char manufacturer_name[32];        /**< manufacturer name */
    char interface[8];                 /**< chip interface name */
    float supply_voltage_min_v;        /**< chip min supply voltage */
    float supply_voltage_max_v;        /**< chip max supply voltage */
    float max_current_ma;              /**< chip max current */
    float temperature_min;             /**< chip min operating temperature */
    float temperature_max;             /**< chip max operating temperature */
    uint32_t driver_version;           /**< driver version */
} pmw3901mb_info_t;

/**
 * @}
 */

/**
 * @defgroup pmw3901mb_link_driver pmw3901mb link driver function
 * @brief    pmw3901mb link driver modules
 * @ingroup  pmw3901mb_driver
 * @{
 */

/**
 * @brief     initialize pmw3901mb_handle_t structure
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] STRUCTURE is pmw3901mb_handle_t
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_INIT(HANDLE, STRUCTURE)               memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link spi_init function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a spi_init function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_SPI_INIT(HANDLE, FUC)                 (HANDLE)->spi_init = FUC

/**
 * @brief     link spi_deinit function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a spi_deinit function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_SPI_DEINIT(HANDLE, FUC)               (HANDLE)->spi_deinit = FUC

/**
 * @brief     link spi_read function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a spi_read function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_SPI_READ(HANDLE, FUC)                 (HANDLE)->spi_read = FUC

/**
 * @brief     link spi_write function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a spi_write function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_SPI_WRITE(HANDLE, FUC)                (HANDLE)->spi_write = FUC

/**
 * @brief     link reset_gpio_init function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a reset_gpio_init function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_RESET_GPIO_INIT(HANDLE, FUC)          (HANDLE)->reset_gpio_init = FUC

/**
 * @brief     link reset_gpio_deinit function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a reset_gpio_deinit function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_RESET_GPIO_DEINIT(HANDLE, FUC)        (HANDLE)->reset_gpio_deinit = FUC

/**
 * @brief     link reset_gpio_write function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a reset_gpio_write function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_RESET_GPIO_WRITE(HANDLE, FUC)         (HANDLE)->reset_gpio_write = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a delay_ms function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_DELAY_MS(HANDLE, FUC)                 (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE points to a pmw3901mb handle structure
 * @param[in] FUC points to a debug_print function address
 * @note      none
 */
#define DRIVER_PMW3901MB_LINK_DEBUG_PRINT(HANDLE, FUC)              (HANDLE)->debug_print = FUC

/**
 * @}
 */

/**
 * @defgroup pmw3901mb_basic_driver pmw3901mb basic driver function
 * @brief    pmw3901mb basic driver modules
 * @ingroup  pmw3901mb_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info points to a pmw3901mb info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t pmw3901mb_info(pmw3901mb_info_t *info);

/**
 * @brief     initialize the chip
 * @param[in] *handle points to a pmw3901mb handle structure
 * @return    status code
 *            - 0 success
 *            - 1 spi or gpio initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 *            - 4 id is invalid
 *            - 5 reset failed
 * @note      none
 */
uint8_t pmw3901mb_init(pmw3901mb_handle_t *handle);

/**
 * @brief     close the chip
 * @param[in] *handle points to a pmw3901mb handle structure
 * @return    status code
 *            - 0 success
 *            - 1 spi or gpio deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 power down failed
 * @note      none
 */
uint8_t pmw3901mb_deinit(pmw3901mb_handle_t *handle);

/**
 * @brief     power up the chip
 * @param[in] *handle points to a pmw3901mb handle structure
 * @return    status code
 *            - 0 success
 *            - 1 power up failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_power_up(pmw3901mb_handle_t *handle);

/**
 * @brief     shutdown the chip
 * @param[in] *handle points to a pmw3901mb handle structure
 * @return    status code
 *            - 0 success
 *            - 1 shutdown failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_shutdown(pmw3901mb_handle_t *handle);

/**
 * @brief     reset the chip
 * @param[in] *handle points to a pmw3901mb handle structure
 * @return    status code
 *            - 0 success
 *            - 1 reset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_reset(pmw3901mb_handle_t *handle);

/**
 * @brief     set the optimum performance
 * @param[in] *handle points to a pmw3901mb handle structure
 * @return    status code
 *            - 0 success
 *            - 1 set optimum performance failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_set_optimum_performance(pmw3901mb_handle_t *handle);

/**
 * @brief      burst read data
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *motion points to a motion structure
 * @return     status code
 *             - 0 success
 *             - 1 burst read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_burst_read(pmw3901mb_handle_t *handle, pmw3901mb_motion_t *motion);

/**
 * @brief      convert the delta raw to the delta cm
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[in]  raw is the delta raw
 * @param[in]  height_m is the height(m)
 * @param[out] *cm points to a cm buffer
 * @return     status code
 *             - 0 success
 *             - 1 delta raw to delta cm failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_delta_raw_to_delta_cm(pmw3901mb_handle_t *handle, int16_t raw, float height_m, float *cm);

/**
 * @brief     start frame capture
 * @param[in] *handle points to a pmw3901mb handle structure
 * @return    status code
 *            - 0 success
 *            - 1 start frame capture failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_start_frame_capture(pmw3901mb_handle_t *handle);

/**
 * @brief     stop frame capture
 * @param[in] *handle points to a pmw3901mb handle structure
 * @return    status code
 *            - 0 success
 *            - 1 stop frame capture failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_stop_frame_capture(pmw3901mb_handle_t *handle);

/**
 * @brief      get the frame
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] **frame points to a frame buffer
 * @return     status code
 *             - 0 success
 *             - 1 get frame failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       0   1     2    ...   32   33   34 (byte)
 *             .    .    .    ...    .    .    .
 *             .    .    .    ...    .    .    .
 *             .    .    .    ...    .    .    .
 *             1190 1191 1192 ... 1222 1223 1224
 */
uint8_t pmw3901mb_get_frame(pmw3901mb_handle_t *handle, uint8_t frame[35][35]);

/**
 * @brief      get the product id
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *id points to an id buffer
 * @return     status code
 *             - 0 success
 *             - 1 get product id failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_product_id(pmw3901mb_handle_t *handle, uint8_t *id);

/**
 * @brief      get the inverse product id
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *id points to an id buffer
 * @return     status code
 *             - 0 success
 *             - 1 get inverse product id failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_inverse_product_id(pmw3901mb_handle_t *handle, uint8_t *id);

/**
 * @brief      get the revision id
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *id points to an id buffer
 * @return     status code
 *             - 0 success
 *             - 1 get revision id failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_revision_id(pmw3901mb_handle_t *handle, uint8_t *id);

/**
 * @brief      get the motion
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *motion points to a motion buffer
 * @return     status code
 *             - 0 success
 *             - 1 get motion failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_motion(pmw3901mb_handle_t *handle, uint8_t *motion);

/**
 * @brief     set the motion
 * @param[in] *handle points to a pmw3901mb handle structure
 * @param[in] motion is the set motion
 * @return    status code
 *            - 0 success
 *            - 1 set motion failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_set_motion(pmw3901mb_handle_t *handle, uint8_t motion);

/**
 * @brief      get the delta x
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *delta points to a delta buffer
 * @return     status code
 *             - 0 success
 *             - 1 get delta x failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_delta_x(pmw3901mb_handle_t *handle, int16_t *delta);

/**
 * @brief      get the delta y
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *delta points to a delta buffer
 * @return     status code
 *             - 0 success
 *             - 1 get delta y failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_delta_y(pmw3901mb_handle_t *handle, int16_t *delta);

/**
 * @brief      get the motion burst
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *burst points to a burst buffer
 * @param[in]  len is the burst length
 * @return     status code
 *             - 0 success
 *             - 1 get motion burst failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_motion_burst(pmw3901mb_handle_t *handle, uint8_t *burst, uint8_t len);

/**
 * @brief      get the surface quality
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *squal points to a surface quality buffer
 * @return     status code
 *             - 0 success
 *             - 1 get squal failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_squal(pmw3901mb_handle_t *handle, uint8_t *squal);

/**
 * @brief      get the average raw data value
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *sum points to a sum buffer
 * @return     status code
 *             - 0 success
 *             - 1 get raw data sum failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_raw_data_sum(pmw3901mb_handle_t *handle, uint8_t *sum);

/**
 * @brief      get the maximum raw data value
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *max points to a max buffer
 * @return     status code
 *             - 0 success
 *             - 1 get max raw data failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_max_raw_data(pmw3901mb_handle_t *handle, uint8_t *max);

/**
 * @brief      get the minimum raw data value
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *min points to a min buffer
 * @return     status code
 *             - 0 success
 *             - 1 get min raw data failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_min_raw_data(pmw3901mb_handle_t *handle, uint8_t *min);

/**
 * @brief      get the shutter
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *shutter points to a shutter buffer
 * @return     status code
 *             - 0 success
 *             - 1 get shutter failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_shutter(pmw3901mb_handle_t *handle, uint16_t *shutter);

/**
 * @brief      get the observation
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *observation points to an observation buffer
 * @return     status code
 *             - 0 success
 *             - 1 get observation failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_observation(pmw3901mb_handle_t *handle, uint8_t *observation);

/**
 * @brief     set the observation
 * @param[in] *handle points to a pmw3901mb handle structure
 * @param[in] *observation is the input observation
 * @return    status code
 *            - 0 success
 *            - 1 set observation failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_set_observation(pmw3901mb_handle_t *handle, uint8_t observation);

/**
 * @brief      get the raw data grab
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *grab points to a grab buffer
 * @param[in]  len is the grab length
 * @return     status code
 *             - 0 success
 *             - 1 get raw data grab failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_raw_data_grab(pmw3901mb_handle_t *handle, uint8_t *grab, uint16_t len);

/**
 * @brief     set the raw data grab
 * @param[in] *handle points to a pmw3901mb handle structure
 * @param[in] *grab points to a raw data grab buffer
 * @param[in] len is the grab length
 * @return    status code
 *            - 0 success
 *            - 1 set raw data grab failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_set_raw_data_grab(pmw3901mb_handle_t *handle, uint8_t *grab, uint16_t len);

/**
 * @brief      get the raw data grab status
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[out] *status points to a status buffer
 * @return     status code
 *             - 0 success
 *             - 1 get raw data grab status failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_raw_data_grab_status(pmw3901mb_handle_t *handle, uint8_t *status);

/**
 * @}
 */

/**
 * @defgroup pmw3901mb_extern_driver pmw3901mb extern driver function
 * @brief    pmw3901mb extern driver modules
 * @ingroup  pmw3901mb_driver
 * @{
 */

/**
 * @brief     set the chip register
 * @param[in] *handle points to a pmw3901mb handle structure
 * @param[in] reg is the register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the data buffer length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t pmw3901mb_set_reg(pmw3901mb_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief      get the chip register
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[in]  reg is the register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the data buffer length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t pmw3901mb_get_reg(pmw3901mb_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
