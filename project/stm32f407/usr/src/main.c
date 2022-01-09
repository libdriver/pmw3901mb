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
 * @file      main.c
 * @brief     main source file
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
#include "driver_pmw3901mb_frame_test.h"
#include "driver_pmw3901mb_read_test.h"
#include "driver_pmw3901mb_register_test.h"
#include "driver_pmw3901mb_basic.h"
#include "driver_pmw3901mb_frame.h"
#include "driver_pmw3901mb_interrupt.h"
#include "shell.h"
#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"
#include <stdlib.h>

/**
 * @brief global var definition
 */
uint8_t g_buf[256];                                  /**< uart buffer */
uint16_t g_len;                                      /**< uart buffer length */
uint8_t (*g_gpio_irq)(float height_m) = NULL;        /**< gpio irq */
volatile static uint8_t gs_flag;                     /**< inner flag */
static uint8_t gs_frame[35][35];                     /**< frame array */

/**
 * @brief exti 0 irq
 * @note  none
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief     gpio exti callback
 * @param[in] pin is the gpio pin
 * @note      none
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0)
    {
        if (g_gpio_irq)
        {
            g_gpio_irq(1.0f);
        }
    }
}

/**
 * @brief     callback
 * @param[in] *motion points to a pmw3901mb_motion_t structure
 * @param[in] delta_x is the delta_x in cm
 * @param[in] delta_y is the delta_y in cm
 * @return    status code
 *             - 0 success
 * @note      none
 */
static uint8_t _callback(pmw3901mb_motion_t *motion, float delta_x, float delta_y)
{
    /* check the result */
    if (motion->is_valid == 1)
    {
        /* print the result */
        pmw3901mb_interface_debug_print("pmw3901mb: find interrupt.\n");
        pmw3901mb_interface_debug_print("pmw3901mb: delta_x: %0.3fcm delta_y: %0.3fcm.\n", delta_x, delta_y);
        pmw3901mb_interface_debug_print("pmw3901mb: raw_average is 0x%02X.\n", motion->raw_average);
        pmw3901mb_interface_debug_print("pmw3901mb: raw_max is 0x%02X.\n", motion->raw_max);
        pmw3901mb_interface_debug_print("pmw3901mb: raw_min is 0x%02X.\n", motion->raw_min);
        pmw3901mb_interface_debug_print("pmw3901mb: observation is 0x%02X.\n", motion->observation);
        pmw3901mb_interface_debug_print("pmw3901mb: shutter is 0x%04X.\n", motion->shutter);
        pmw3901mb_interface_debug_print("pmw3901mb: surface quality is 0x%04X.\n\n", motion->surface_quality);
        
        /* set flag */
        gs_flag = 1;
    }
    
    return 0;
}

/**
 * @brief     pmw3901mb full function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 *             - 1 run failed
 *             - 5 param is invalid
 * @note      none
 */
uint8_t pmw3901mb(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        goto help;
    }
    else if (argc == 2)
    {
        if (strcmp("-i", argv[1]) == 0)
        {
            pmw3901mb_info_t info;
            
            /* print pmw3901mb info */
            pmw3901mb_info(&info);
            pmw3901mb_interface_debug_print("pmw3901mb: chip is %s.\n", info.chip_name);
            pmw3901mb_interface_debug_print("pmw3901mb: manufacturer is %s.\n", info.manufacturer_name);
            pmw3901mb_interface_debug_print("pmw3901mb: interface is %s.\n", info.interface);
            pmw3901mb_interface_debug_print("pmw3901mb: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000)/100);
            pmw3901mb_interface_debug_print("pmw3901mb: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
            pmw3901mb_interface_debug_print("pmw3901mb: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
            pmw3901mb_interface_debug_print("pmw3901mb: max current is %0.2fmA.\n", info.max_current_ma);
            pmw3901mb_interface_debug_print("pmw3901mb: max temperature is %0.1fC.\n", info.temperature_max);
            pmw3901mb_interface_debug_print("pmw3901mb: min temperature is %0.1fC.\n", info.temperature_min);
            
            return 0;
        }
        else if (strcmp("-p", argv[1]) == 0)
        {
            /* print pin connection */
            pmw3901mb_interface_debug_print("pmw3901mb: SCK connected to GPIOA PIN5.\n");
            pmw3901mb_interface_debug_print("pmw3901mb: MISO connected to GPIOA PIN6.\n");
            pmw3901mb_interface_debug_print("pmw3901mb: MOSI connected to GPIOA PIN7.\n");
            pmw3901mb_interface_debug_print("pmw3901mb: CS connected to GPIOA PIN4.\n");
            pmw3901mb_interface_debug_print("pmw3901mb: RESET connected to GPIOA PIN8.\n");
            pmw3901mb_interface_debug_print("pmw3901mb: INT connected to GPIOB PIN0.\n");
            
            return 0;
        }
        else if (strcmp("-h", argv[1]) == 0)
        {
            /* show pmw3901mb help */
            
            help:
            pmw3901mb_interface_debug_print("pmw3901mb -i\n\tshow pmw3901mb chip and driver information.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -h\n\tshow pmw3901mb help.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -p\n\tshow pmw3901mb pin connections of the current board.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -t reg\n\trun pmw3901mb register test.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -t read <height> <times>\n\trun pmw3901mb read test.height is the chip height.times is the test times.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -t frame <times>\n\trun pmw3901mb frame capture test.times is the test times.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -t interrupt <times>\n\trun pmw3901mb interrupt test.times is the test times.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -c read <height> <times>\n\trun pmw3901mb read function.height is the chip height.times is the test times.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -c frame <times>\n\trun pmw3901mb frame capture function.times is the test times.\n");
            pmw3901mb_interface_debug_print("pmw3901mb -c interrupt <times>\n\trun pmw3901mb interrupt function.times is the test times.\n");
            
            return 0;
        }
        else
        {
            return 5;
        }
    }
    else if (argc == 3)
    {
        /* run test */
        if (strcmp("-t", argv[1]) == 0)
        {
            /* reg test */
            if (strcmp("reg", argv[2]) == 0)
            {
                volatile uint8_t res;
                
                res = pmw3901mb_register_test();
                if (res)
                {
                    return 1;
                }
                
                return 0;
            }
            
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        /* param is invalid */
        else
        {
            return 5;
        }
    }
    else if (argc == 4)
    {
        /* run the test */
        if (strcmp("-t", argv[1]) == 0)
        {
            if (strcmp("frame", argv[2]) == 0)
            {
                volatile uint8_t res;
                
                res = pmw3901mb_frame_test(atoi(argv[3]));
                if (res)
                {
                    return 1;
                }
                
                return 0;
            }
            else if (strcmp("interrupt", argv[2]) == 0)
            {
                volatile uint8_t res;
                
                res = gpio_interrupt_init();
                if (res)
                {
                    return 1;
                }
                g_gpio_irq = pmw3901mb_interrupt_test_irq_handler;
                res = pmw3901mb_interrupt_test(atoi(argv[3]));
                if (res)
                {
                    gpio_interrupt_deinit();
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                gpio_interrupt_deinit();
                g_gpio_irq = NULL;
                
                return 0;
            }
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        /* run the function */
        else if (strcmp("-c", argv[1]) == 0)
        {
            if (strcmp("frame", argv[2]) == 0)
            {
                volatile uint8_t res;
                volatile uint32_t k, times;
                volatile uint32_t i, j;
                
                times = atoi(argv[3]);
                res = pmw3901mb_frame_init();
                if (res)
                {
                    return 1;
                }
                
                for (k = 0; k < times; k++)
                {
                    res = pmw3901mb_frame_read(gs_frame);
                    if (res)
                    {
                        pmw3901mb_frame_deinit();
                        
                        return 1;
                    }
                    
                    pmw3901mb_interface_debug_print("pmw3901mb: %d/%d.\n", k + 1, times);
                    /* print frame */
                    for (i = 0; i < 35; i++)
                    {
                        for (j = 0; j < 35; j++)
                        {
                            pmw3901mb_interface_debug_print("0x%02X ", gs_frame[i][j]);
                        }
                        pmw3901mb_interface_debug_print("\n");
                    }
                    pmw3901mb_interface_debug_print("\n");
                    
                    /* delay 1000 ms */
                    pmw3901mb_interface_delay_ms(1000);
                }
                
                return pmw3901mb_frame_deinit();
            }
            else if (strcmp("interrupt", argv[2]) == 0)
            {
                volatile uint8_t res;
                volatile uint32_t i, times;
                
                times = atoi(argv[3]);
                res = gpio_interrupt_init();
                if (res)
                {
                    return 1;
                }
                g_gpio_irq = pmw3901mb_interrupt_irq_handler;
                
                res = pmw3901mb_interrupt_init(_callback);
                if (res)
                {
                    pmw3901mb_interrupt_deinit();
                    
                    return 1;
                }
                for (i = 0; i < times; i++)
                {
                    pmw3901mb_interface_debug_print("pmw3901mb: %d/%d.\n", i + 1, times);
                    
                    gs_flag = 0;
                    while (gs_flag == 0)
                    {
                        pmw3901mb_interface_delay_ms(10);
                    }
                }
                
                pmw3901mb_interrupt_deinit();
                gpio_interrupt_deinit();
                g_gpio_irq = NULL;
                
                return 0;
            }
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        /* param is invalid */
        else
        {
            return 5;
        }
    }
    else if (argc == 5)
    {
        /* run the test */
        if (strcmp("-t", argv[1]) == 0)
        {
            if (strcmp("read", argv[2]) == 0)
            {
                volatile uint8_t res;
                
                res = pmw3901mb_read_test(atof(argv[3]), atoi(argv[4]));
                if (res)
                {
                    return 1;
                }
                
                return 0;
            }
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        /* run the function */
        else if (strcmp("-c", argv[1]) == 0)
        {
            if (strcmp("read", argv[2]) == 0)
            {
                volatile uint8_t res;
                volatile float height;
                volatile float delta_x;
                volatile float delta_y;
                volatile uint32_t i, times;
                pmw3901mb_motion_t motion;
                
                height = atof(argv[3]);
                times = atoi(argv[4]);
                res = pmw3901mb_basic_init();
                if (res)
                {
                    return 1;
                }
                for (i = 0; i < times; i++)
                {
                    read:
                    
                    res = pmw3901mb_basic_read(height, &motion, (float *)&delta_x, (float *)&delta_y);
                    if (res)
                    {
                        pmw3901mb_basic_deinit();
                        
                        return 1;
                    }
                    
                    /* check the result */
                    if (motion.is_valid == 1)
                    {
                        /* print the result */
                        
                        pmw3901mb_interface_debug_print("pmw3901mb: %d/%d.\n", i + 1, times);
                        pmw3901mb_interface_debug_print("pmw3901mb: delta_x: %0.3fcm delta_y: %0.3fcm.\n", delta_x, delta_y);
                        pmw3901mb_interface_debug_print("pmw3901mb: raw_average is 0x%02X.\n", motion.raw_average);
                        pmw3901mb_interface_debug_print("pmw3901mb: raw_max is 0x%02X.\n", motion.raw_max);
                        pmw3901mb_interface_debug_print("pmw3901mb: raw_min is 0x%02X.\n", motion.raw_min);
                        pmw3901mb_interface_debug_print("pmw3901mb: observation is 0x%02X.\n", motion.observation);
                        pmw3901mb_interface_debug_print("pmw3901mb: shutter is 0x%04X.\n", motion.shutter);
                        pmw3901mb_interface_debug_print("pmw3901mb: surface quality is 0x%04X.\n\n", motion.surface_quality);
                    }
                    else
                    {
                        pmw3901mb_interface_delay_ms(500);
                        
                        goto read;
                    }
                    
                    /* delay 1000 ms */
                    pmw3901mb_interface_delay_ms(1000);
                }
                
                return pmw3901mb_basic_deinit();
            }
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        /* param is invalid */
        else
        {
            return 5;
        }
    }
    /* param is invalid */
    else
    {
        return 5;
    }
}

/**
 * @brief main function
 * @note  none
 */
int main(void)
{
    volatile uint8_t res;
    
    /* stm32f407 clock init and hal init */
    clock_init();
    
    /* delay init */
    delay_init();
    
    /* uart1 init */
    uart1_init(115200);
    
    /* shell init && register pmw3901mb fuction */
    shell_init();
    shell_register("pmw3901mb", pmw3901mb);
    uart1_print("pmw3901mb: welcome to libdriver pmw3901mb.\n");
    
    while (1)
    {
        /* read uart */
        g_len = uart1_read(g_buf, 256);
        if (g_len)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart1_print("pmw3901mb: run failed.\n");
            }
            else if (res == 2)
            {
                uart1_print("pmw3901mb: unknow command.\n");
            }
            else if (res == 3)
            {
                uart1_print("pmw3901mb: length is too long.\n");
            }
            else if (res == 4)
            {
                uart1_print("pmw3901mb: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart1_print("pmw3901mb: param is invalid.\n");
            }
            else
            {
                uart1_print("pmw3901mb: unknow status code.\n");
            }
            uart1_flush();
        }
        delay_ms(100);
    }
}
