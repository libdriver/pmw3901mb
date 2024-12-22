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
 * @file      driver_pmw3901mb_interrupt.h
 * @brief     driver pmw3901mb interrupt header file
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

#ifndef DRIVER_PMW3901MB_INTERRUPT_H
#define DRIVER_PMW3901MB_INTERRUPT_H

#include "driver_pmw3901mb_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @addtogroup pmw3901mb_example_driver
 * @{
 */

/**
 * @brief     interrupt irq
 * @param[in] height_m height(m)
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 * @note      none
 */
uint8_t pmw3901mb_interrupt_irq_handler(float height_m);

/**
 * @brief     interrupt example init
 * @param[in] *callback pointer to a callback function
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t pmw3901mb_interrupt_init(void (*callback)(pmw3901mb_motion_t *motion, float delta_x, float delta_y));

/**
 * @brief  interrupt example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t pmw3901mb_interrupt_deinit(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
