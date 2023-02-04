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
#include "gpio.h"
#include <getopt.h>
#include <stdlib.h>

static volatile uint8_t gs_flag;           /**< interrupt flag */
static uint8_t gs_frame[35][35];           /**< frame array */
uint8_t (*g_gpio_irq)(float m) = NULL;     /**< gpio irq function address */

/**
 * @brief     callback
 * @param[in] *motion points to a pmw3901mb_motion_t structure
 * @param[in] delta_x is the delta_x in cm
 * @param[in] delta_y is the delta_y in cm
 * @note      none
 */
static void a_callback(pmw3901mb_motion_t *motion, float delta_x, float delta_y)
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
        pmw3901mb_interface_debug_print("pmw3901mb: surface quality is 0x%04X.\n", motion->surface_quality);
        
        /* set flag */
        gs_flag = 1;
    }
}

/**
 * @brief     pmw3901mb full function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 5 param is invalid
 * @note      none
 */
uint8_t pmw3901mb(uint8_t argc, char **argv)
{
    int c;
    int longindex = 0;
    const char short_options[] = "hipe:t:";
    const struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"information", no_argument, NULL, 'i'},
        {"port", no_argument, NULL, 'p'},
        {"example", required_argument, NULL, 'e'},
        {"test", required_argument, NULL, 't'},
        {"height", required_argument, NULL, 1},
        {"times", required_argument, NULL, 2},
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknown";
    uint32_t times = 3;
    float height = 1.0f;
    
    /* if no params */
    if (argc == 1)
    {
        /* goto the help */
        goto help;
    }
    
    /* init 0 */
    optind = 0;
    
    /* parse */
    do
    {
        /* parse the args */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);
        
        /* judge the result */
        switch (c)
        {
            /* help */
            case 'h' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");
                
                break;
            }
            
            /* information */
            case 'i' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");
                
                break;
            }
            
            /* port */
            case 'p' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");
                
                break;
            }
            
            /* example */
            case 'e' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);
                
                break;
            }
            
            /* test */
            case 't' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);
                
                break;
            }
            
            /* height */
            case 1 :
            {
                /* set the height */
                height = atof(optarg);
                
                break;
            } 

            /* running times */
            case 2 :
            {
                /* set the times */
                times = atol(optarg);
                
                break;
            } 
            
            /* the end */
            case -1 :
            {
                break;
            }
            
            /* others */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);

    /* run the function */
    if (strcmp("t_reg", type) == 0)
    {
        uint8_t res;
        
        res = pmw3901mb_register_test();
        if (res != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("t_read", type) == 0)
    {
        uint8_t res;
        
        res = pmw3901mb_read_test(height, times);
        if (res != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("t_frame", type) == 0)
    {
        uint8_t res;
        
        res = pmw3901mb_frame_test(times);
        if (res != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("t_int", type) == 0)
    {
        uint8_t res;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* set interrupt irq */
        g_gpio_irq = pmw3901mb_interrupt_test_irq_handler;
        
        /* run the interrupt test */
        res = pmw3901mb_interrupt_test(times);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("e_read", type) == 0)
    {
        uint8_t res;
        float delta_x;
        float delta_y;
        uint32_t i;
        pmw3901mb_motion_t motion;
        
        /* basic init */
        res = pmw3901mb_basic_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* loop */
        for (i = 0; i < times; i++)
        {
            read:
            /* read data */
            res = pmw3901mb_basic_read(height, &motion, (float *)&delta_x, (float *)&delta_y);
            if (res != 0)
            {
                (void)pmw3901mb_basic_deinit();
                
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
                pmw3901mb_interface_debug_print("pmw3901mb: surface quality is 0x%04X.\n", motion.surface_quality);
            }
            else
            {
                /* delay 500ms */
                pmw3901mb_interface_delay_ms(500);
                
                goto read;
            }
            
            /* delay 1000 ms */
            pmw3901mb_interface_delay_ms(1000);
        }
        
        /* basic deinit */
        (void)pmw3901mb_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_frame", type) == 0)
    {
        uint8_t res;
        uint32_t k;
        uint32_t i;
        uint32_t j;
        
        /* frame init */
        res = pmw3901mb_frame_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* loop */
        for (k = 0; k < times; k++)
        {
            /* read data */
            res = pmw3901mb_frame_read(gs_frame);
            if (res != 0)
            {
                (void)pmw3901mb_frame_deinit();
                
                return 1;
            }
            
            /* read data */
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
            
            /* delay 1000 ms */
            pmw3901mb_interface_delay_ms(1000);
        }
        
        (void)pmw3901mb_frame_deinit();
        
        return 0;
    }
    else if (strcmp("e_int", type) == 0)
    {
        uint8_t res;
        uint32_t i;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* set the interrupt irq */
        g_gpio_irq = pmw3901mb_interrupt_irq_handler;
        
        /* interrupt init */
        res = pmw3901mb_interrupt_init(a_callback);
        if (res != 0)
        {
            (void)pmw3901mb_interrupt_deinit();
            
            return 1;
        }
        
        /* loop */
        for (i = 0; i < times; i++)
        {
            /* output */
            pmw3901mb_interface_debug_print("pmw3901mb: %d/%d.\n", i + 1, times);
            
            /* check the flag */
            gs_flag = 0;
            while (gs_flag == 0)
            {
                pmw3901mb_interface_delay_ms(10);
            }
        }
        
        /* deinit */
        (void)pmw3901mb_interrupt_deinit();
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        pmw3901mb_interface_debug_print("Usage:\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-i | --information)\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-h | --help)\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-p | --port)\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-t reg | --test=reg)\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-t read | --test=read) [--height=<m>] [--times=<num>]\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-t frame | --test=frame) [--times=<num>]\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-t int | --test=int) [--times=<num>]\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-e read | --example=read) [--height=<m>] [--times=<num>]\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-e frame | --example=frame) [--times=<num>]\n");
        pmw3901mb_interface_debug_print("  pmw3901mb (-e int | --example=int) [--times=<num>]\n");
        pmw3901mb_interface_debug_print("\n");
        pmw3901mb_interface_debug_print("Options:\n");
        pmw3901mb_interface_debug_print("  -e <read | frame | int>, --example=<read | frame | int>\n");
        pmw3901mb_interface_debug_print("                              Run the driver example.\n");
        pmw3901mb_interface_debug_print("  -h, --help                  Show the help.\n");
        pmw3901mb_interface_debug_print("      --height=<m>            Set the chip height in m.([default: 1.0])\n");
        pmw3901mb_interface_debug_print("  -i, --information           Show the chip information.\n");
        pmw3901mb_interface_debug_print("  -p, --port                  Display the pin connections of the current board.\n");
        pmw3901mb_interface_debug_print("  -t <reg | read | frame | int>, --test=<reg | read | frame | int>\n");
        pmw3901mb_interface_debug_print("                              Run the driver test.\n");
        pmw3901mb_interface_debug_print("      --times=<num>           Set the running times.([default: 3])\n");
        
        return 0;
    }
    else if (strcmp("i", type) == 0)
    {
        pmw3901mb_info_t info;
        
        /* print pmw3901mb info */
        pmw3901mb_info(&info);
        pmw3901mb_interface_debug_print("pmw3901mb: chip is %s.\n", info.chip_name);
        pmw3901mb_interface_debug_print("pmw3901mb: manufacturer is %s.\n", info.manufacturer_name);
        pmw3901mb_interface_debug_print("pmw3901mb: interface is %s.\n", info.interface);
        pmw3901mb_interface_debug_print("pmw3901mb: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        pmw3901mb_interface_debug_print("pmw3901mb: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        pmw3901mb_interface_debug_print("pmw3901mb: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        pmw3901mb_interface_debug_print("pmw3901mb: max current is %0.2fmA.\n", info.max_current_ma);
        pmw3901mb_interface_debug_print("pmw3901mb: max temperature is %0.1fC.\n", info.temperature_max);
        pmw3901mb_interface_debug_print("pmw3901mb: min temperature is %0.1fC.\n", info.temperature_min);
        
        return 0;
    }
    else if (strcmp("p", type) == 0)
    {
        /* print pin connection */
        pmw3901mb_interface_debug_print("pmw3901mb: SCK connected to GPIO11(BCM).\n");
        pmw3901mb_interface_debug_print("pmw3901mb: MISO connected to GPIO9(BCM).\n");
        pmw3901mb_interface_debug_print("pmw3901mb: MOSI connected to GPIO10(BCM).\n");
        pmw3901mb_interface_debug_print("pmw3901mb: CS connected to GPIO8(BCM).\n");
        pmw3901mb_interface_debug_print("pmw3901mb: RESET connected to GPIO27(BCM).\n");
        pmw3901mb_interface_debug_print("pmw3901mb: INT connected to GPIO17(BCM).\n");
        
        return 0;
    }
    else
    {
        return 5;
    }
}

/**
 * @brief     main function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 * @note      none
 */
int main(uint8_t argc, char **argv)
{
    uint8_t res;

    res = pmw3901mb(argc, argv);
    if (res == 0)
    {
        /* run success */
    }
    else if (res == 1)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: run failed.\n");
    }
    else if (res == 5)
    {
        pmw3901mb_interface_debug_print("pmw3901mb: param is invalid.\n");
    }
    else
    {
        pmw3901mb_interface_debug_print("pmw3901mb: unknown status code.\n");
    }

    return 0;
}
