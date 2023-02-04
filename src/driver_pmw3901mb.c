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
 * @file      driver_pmw3901mb.c
 * @brief     driver pmw3901mb source file
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

#include "driver_pmw3901mb.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "PixArt Imaging PMW3901MB"        /**< chip name */
#define MANUFACTURER_NAME         "PixArt Imaging"                  /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        1.8f                              /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        3.6f                              /**< chip max supply voltage */
#define MAX_CURRENT               70.0f                             /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                            /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                             /**< chip max operating temperature */
#define DRIVER_VERSION            1000                              /**< driver version */

/**
 * @brief chip register definition
 */
#define PMW3901MB_REG_PRODUCT_ID                  0x00        /**< product id register */
#define PMW3901MB_REG_REVISION_ID                 0x01        /**< revision id register */
#define PMW3901MB_REG_MOTION                      0x02        /**< motion register */
#define PMW3901MB_REG_DELTA_X_L                   0x03        /**< delta_x_l  register */
#define PMW3901MB_REG_DELTA_X_H                   0x04        /**< delta_x_h register */
#define PMW3901MB_REG_DELTA_Y_L                   0x05        /**< delta_y_l register */
#define PMW3901MB_REG_DELTA_Y_H                   0x06        /**< delta_y_h register */
#define PMW3901MB_REG_SQUAL                       0x07        /**< squal register */
#define PMW3901MB_REG_RAW_DATA_SUM                0x08        /**< raw data sum register */
#define PMW3901MB_REG_MAXIMUM_RAW_DATA            0x09        /**< maximum raw data register */
#define PMW3901MB_REG_MINIMUM_RAW_DATA            0x0A        /**< minimum raw data register */
#define PMW3901MB_REG_SHUTTER_LOWER               0x0B        /**< shutter lower register */
#define PMW3901MB_REG_SHUTTER_UPPER               0x0C        /**< shutter upper register */
#define PMW3901MB_REG_OBSERVATION                 0x15        /**< observation register */
#define PMW3901MB_REG_MOTION_BURST                0x16        /**< motion burst register */
#define PMW3901MB_REG_POWER_UP_RESET              0x3A        /**< power up reset register */
#define PMW3901MB_REG_SHUTDOWN                    0x3B        /**< shutdown register */
#define PMW3901MB_REG_RAW_DATA_GRAB               0x58        /**< raw data grab register */
#define PMW3901MB_REG_RAW_DATA_GRAB_STATUS        0x59        /**< raw data grab status register */
#define PMW3901MB_REG_INVERSE_PRODUCT_ID          0x5F        /**< inverse product id register */

/**
 * @brief      read bytes
 * @param[in]  *handle points to a pmw3901mb handle structure
 * @param[in]  reg is the register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the data length
 * @return     status code
 *             - 0 success
 *             - 1 spi read failed
 * @note       none
 */
static uint8_t a_pmw3901mb_spi_read(pmw3901mb_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->spi_read(reg, buf, len) != 0)        /* spi read */
    {
        return 1;                                    /* return error */
    }
    else
    {
        return 0;                                    /* success return 0 */
    }
}

/**
 * @brief     write bytes
 * @param[in] *handle points to a pmw3901mb handle structure
 * @param[in] reg is the register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the data length
 * @return    status code
 *            - 0 success
 *            - 1 spi write failed
 * @note      none
 */
static uint8_t a_pmw3901mb_spi_write(pmw3901mb_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle->spi_write(0x80 | reg, buf, len) != 0)        /* spi write */
    {
        return 1;                                            /* return error */
    }
    else
    {
        return 0;                                            /* success return 0 */
    }
}

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
uint8_t pmw3901mb_init(pmw3901mb_handle_t *handle)
{
    uint8_t id;
  
    if (handle == NULL)                                                                  /* check handle */
    {
        return 2;                                                                        /* return error */
    }
    if (handle->debug_print == NULL)                                                     /* check debug_print */
    {
        return 3;                                                                        /* return error */
    }
    if (handle->spi_init == NULL)                                                        /* check spi_init */
    {
        handle->debug_print("pmw3901mb: spi_init is null.\n");                           /* spi_init is null */
       
        return 3;                                                                        /* return error */
    }
    if (handle->spi_deinit == NULL)                                                      /* check spi_deinit */
    {
        handle->debug_print("pmw3901mb: spi_deinit is null.\n");                         /* spi_deinit is null */
       
        return 3;                                                                        /* return error */
    }
    if (handle->spi_read == NULL)                                                        /* check spi_read */
    {
        handle->debug_print("pmw3901mb: spi_read is null.\n");                           /* spi_read is null */
       
        return 3;                                                                        /* return error */
    }
    if (handle->spi_write == NULL)                                                       /* check spi_write */
    {
        handle->debug_print("pmw3901mb: spi_write is null.\n");                          /* spi_write is null */
       
        return 3;                                                                        /* return error */
    }
    if (handle->reset_gpio_init == NULL)                                                 /* check reset_gpio_init */
    {
        handle->debug_print("pmw3901mb: reset_gpio_init is null.\n");                    /* reset_gpio_init is null */
       
        return 3;                                                                        /* return error */
    }
    if (handle->reset_gpio_deinit == NULL)                                               /* check reset_gpio_deinit */
    {
        handle->debug_print("pmw3901mb: reset_gpio_deinit is null.\n");                  /* reset_gpio_deinit is null */
       
        return 3;                                                                        /* return error */
    }
    if (handle->reset_gpio_write == NULL)                                                /* check reset_gpio_write */
    {
        handle->debug_print("pmw3901mb: reset_gpio_write is null.\n");                   /* reset_gpio_write is null */
       
        return 3;                                                                        /* return error */
    }
    if (handle->delay_ms == NULL)                                                        /* check delay_ms */
    {
        handle->debug_print("pmw3901mb: delay_ms is null.\n");                           /* delay_ms is null */
       
        return 3;                                                                        /* return error */
    }
    
    if (handle->spi_init() != 0)                                                         /* initialize spi bus */
    {
        handle->debug_print("pmw3901mb: spi init failed.\n");                            /* spi init failed */
       
        return 1;                                                                        /* return error */
    }
    if (handle->reset_gpio_init() != 0)                                                  /* initialize gpio */
    {
        handle->debug_print("pmw3901mb: gpio init failed.\n");                           /* gpio init failed */
        (void)handle->spi_deinit();                                                      /* spi deinit */
        
        return 1;                                                                        /* return error */
    }
    if (handle->reset_gpio_write(0) != 0)                                                /* write 0 */
    {
        handle->debug_print("pmw3901mb: reset failed.\n");                               /* reset failed */
        (void)handle->spi_deinit();                                                      /* spi deinit */
        (void)handle->reset_gpio_deinit();                                               /* reset gpio deinit */
        
        return 5;                                                                        /* return error */
    }
    handle->delay_ms(10);                                                                /* delay 10 ms */
    if (handle->reset_gpio_write(1) != 0)                                                /* write 1 */
    {
        handle->debug_print("pmw3901mb: reset failed.\n");                               /* reset failed */
        (void)handle->spi_deinit();                                                      /* spi deinit */
        (void)handle->reset_gpio_deinit();                                               /* reset gpio deinit */
        
        return 5;                                                                        /* return error */
    }
    handle->delay_ms(10);                                                                /* delay 10 ms */
    if (a_pmw3901mb_spi_read(handle, PMW3901MB_REG_PRODUCT_ID, (uint8_t *)&id, 1) != 0)  /* get product id */
    {
        handle->debug_print("pmw3901mb: get product id failed.\n");                      /* get product id failed */
        (void)handle->spi_deinit();                                                      /* spi deinit */
        (void)handle->reset_gpio_deinit();                                               /* reset gpio deinit */
       
        return 4;                                                                        /* return error */
    }
    if (id != 0x49)                                                                      /* check id */
    {
        handle->debug_print("pmw3901mb: id is invalid.\n");                              /* id is invalid */
        (void)handle->spi_deinit();                                                      /* spi deinit */
        (void)handle->reset_gpio_deinit();                                               /* reset gpio deinit */
       
        return 4;                                                                        /* return error */
    }
    handle->inited = 1;                                                                  /* flag finish initialization */
    
    return 0;                                                                            /* success return 0 */
}

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
uint8_t pmw3901mb_deinit(pmw3901mb_handle_t *handle)
{
    uint8_t res;
    uint8_t cmd;
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    cmd = 0xB6;                                                                            /* shutdown command */
    res = a_pmw3901mb_spi_write(handle, PMW3901MB_REG_SHUTDOWN, (uint8_t *)&cmd, 1);       /* set shutdown reset */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: power down failed.\n");                            /* power down failed */
       
        return 4;                                                                          /* return error */
    }
    res = handle->spi_deinit();                                                            /* spi deinit */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: spi deinit failed.\n");                            /* spi deinit failed */
       
        return 1;                                                                          /* return error */
    }
    res = handle->reset_gpio_deinit();                                                     /* reset gpio deinit */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: gpio deinit failed.\n");                           /* gpio deinit failed */
       
        return 1;                                                                          /* return error */
    }
    handle->inited = 0;                                                                    /* flag close */
    
    return 0;                                                                              /* success return 0 */
}

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
uint8_t pmw3901mb_power_up(pmw3901mb_handle_t *handle)
{
    uint8_t res;
    uint8_t cmd;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    res = handle->reset_gpio_write(0);                                                           /* write 0 */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: set gpio failed.\n");                                    /* set gpio failed */
       
        return 1;                                                                                /* return error */
    }
    handle->delay_ms(10);                                                                        /* delay 10 ms */
    res = handle->reset_gpio_write(1);                                                           /* write 1 */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: set gpio failed.\n");                                    /* set gpio failed */
       
        return 1;                                                                                /* return error */
    }
    handle->delay_ms(10);                                                                        /* delay 10 ms */
    cmd = 0x5A;                                                                                  /* power up command */
    res = a_pmw3901mb_spi_write(handle, PMW3901MB_REG_POWER_UP_RESET, (uint8_t *)&cmd, 1);       /* set power up reset */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: set power up reset failed.\n");                          /* set power up reset failed */
       
        return 1;                                                                                /* return error */
    }
    handle->delay_ms(10);                                                                        /* delay 10 ms */
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_MOTION, (uint8_t *)&cmd, 1);                /* get command */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: get command failed.\n");                                 /* get command failed */
       
        return 1;                                                                                /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_DELTA_X_L, (uint8_t *)&cmd, 1);             /* get command */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: get command failed.\n");                                 /* get command failed */
       
        return 1;                                                                                /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_DELTA_X_H, (uint8_t *)&cmd, 1);             /* get command */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: get command failed.\n");                                 /* get command failed */
       
        return 1;                                                                                /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_DELTA_Y_L, (uint8_t *)&cmd, 1);             /* get command */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: get command failed.\n");                                 /* get command failed */
       
        return 1;                                                                                /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_DELTA_Y_H, (uint8_t *)&cmd, 1);             /* get command */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: get command failed.\n");                                 /* get command failed */
       
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t pmw3901mb_burst_read(pmw3901mb_handle_t *handle, pmw3901mb_motion_t *motion)
{
    uint8_t res;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_MOTION_BURST, (uint8_t *)motion->raw, 12);       /* burst read */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("pmw3901mb: burst read failed.\n");                                       /* burst read failed */
       
        return 1;                                                                                     /* return error */
    }
    if ((motion->raw[0] & (1 << 7)) != 0)                                                             /* check motion flag */
    {
        if ((motion->raw[6] < 0x19) || (motion->raw[10] == 0x1F))                                     /* check data */
        {
            motion->is_valid = 2;                                                                     /* set invalid */
            
            return 0;                                                                                 /* success return 0 */
        }
        motion->delta_x = (int16_t)(((uint16_t)motion->raw[3] << 8) | motion->raw[2]);                /* set delta_x */
        motion->delta_y = (int16_t)(((uint16_t)motion->raw[5] << 8) | motion->raw[4]);                /* set delta_y */
        motion->observation = motion->raw[1] & 0x3F;                                                  /* set observation */
        motion->raw_average = motion->raw[7];                                                         /* set raw average */
        motion->raw_max = motion->raw[8];                                                             /* set raw max */
        motion->raw_min = motion->raw[9];                                                             /* set raw min */
        motion->shutter = (((((uint16_t)motion->raw[10] & 0x1F) << 8)) | motion->raw[11]);            /* set shutter */
        motion->surface_quality = motion->raw[6] * 4;                                                 /* set surface quality */
        
        motion->is_valid = 1;                                                                         /* set valid */
    }
    else
    {
        motion->is_valid = 0;                                                                         /* set invalid */
    }
    
    return 0;                                                                                         /* success return 0 */
}

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
uint8_t pmw3901mb_start_frame_capture(pmw3901mb_handle_t *handle)
{
    uint8_t res;
    uint8_t cmd;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    cmd = 0x07;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x1D;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x41, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4C, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x08;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x38;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x6A, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x04;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x55, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x40, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x11;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4D, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t pmw3901mb_stop_frame_capture(pmw3901mb_handle_t *handle)
{
    uint8_t res;
    uint8_t cmd;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x11;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4D, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x40, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x55, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x08;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x18;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x6A, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x07;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x0D;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x41, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4C, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t pmw3901mb_get_frame(pmw3901mb_handle_t *handle, uint8_t frame[35][35])
{
    uint8_t res;
    uint8_t cmd;
    uint8_t i, j;
    uint32_t retry_times;
    
    if (handle == NULL)                                                                                    /* check handle */
    {
        return 2;                                                                                          /* return error */
    }
    if (handle->inited != 1)                                                                               /* check handle initialization */
    {
        return 3;                                                                                          /* return error */
    }
    
    cmd = 0x00;                                                                                            /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x70, (uint8_t *)&cmd, 1);                                         /* sent the command */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");                                      /* sent the command failed */
       
        return 1;                                                                                          /* return error */
    }
    cmd = 0xFF;                                                                                            /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x58, (uint8_t *)&cmd, 1);                                         /* sent the command */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");                                      /* sent the command failed */
       
        return 1;                                                                                          /* return error */
    }
    
    retry_times = 10;                                                                                      /* set retry times */
    
    start:
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_RAW_DATA_GRAB_STATUS, (uint8_t *)&cmd, 1);            /* read grab status */
    if (res != 0)                                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: read grab status failed.\n");                                      /* read grab status failed */
       
        return 1;                                                                                          /* return error */
    }
    if ((cmd & (1 << 7)) && (cmd & (1 << 6)))
    {
        for (i = 0; i < 35; i++)                                                                           /* 35 times */
        {
            for (j = 0; j < 35; j++)                                                                       /* 35 times */
            {
                retry_times = 10;                                                                          /* set retry times */
                
                step1:
                
                res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_RAW_DATA_GRAB, (uint8_t *)&cmd, 1);       /* read grab data */
                if (res != 0)                                                                              /* check result */
                {
                    handle->debug_print("pmw3901mb: read grab data failed.\n");                            /* read grab data failed */
                   
                    return 1;                                                                              /* return error */
                }
                if ((cmd & (1 << 6)) != 0)                                                                 /* check flag */
                {
                    frame[i][j] = 0;                                                                       /* set 0 */
                    frame[i][j] |= (cmd & 0x3F) << 2;                                                      /* upper 6 bits */
                }
                else
                {
                    retry_times--;                                                                         /* retry times-- */
                    if (retry_times != 0)                                                                  /* check retry times */
                    {
                        handle->delay_ms(10);                                                              /* delay 10 ms */
                        
                        goto step1;                                                                        /* goto step1 */
                    }
                    else
                    {
                        handle->debug_print("pmw3901mb: read timeout.\n");                                 /* read timeout */
                       
                        return 4;                                                                          /* return error */
                    }
                }
                retry_times = 10;                                                                          /* set 10 times */
                
                step2:
                
                res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_RAW_DATA_GRAB, (uint8_t *)&cmd, 1);       /* read grab data */
                if (res != 0)                                                                              /* check result */
                {
                    handle->debug_print("pmw3901mb: read grab data failed.\n");                            /* read grab data failed */
                   
                    return 1;                                                                              /* return error */
                }
                if ((cmd & (2 << 6)) != 0)                                                                 /* check flag */
                {
                    cmd = (cmd >> 2) & 0x3;                                                                /* get 2 bits */
                    frame[i][j] = frame[i][j] | cmd;                                                       /* set the frame */
                }
                else
                {
                    retry_times--;                                                                         /* retry times-- */
                    if (retry_times != 0)                                                                  /* check retry times */
                    {
                        handle->delay_ms(10);                                                              /* delay 10 ms */
                        
                        goto step2;                                                                        /* goto step2 */
                    }
                    else
                    {
                        handle->debug_print("pmw3901mb: read timeout.\n");                                 /* read timeout */
                       
                        return 4;                                                                          /* return error */
                    }
                }
            }
        }
        
        return 0;                                                                                          /* success return 0 */
    }
    else
    {
        retry_times--;                                                                                     /* retry times-- */
        if (retry_times != 0)
        {
            handle->delay_ms(10);                                                                          /* delay 10 ms */
            
            goto start;                                                                                    /* goto start */
        }
        else
        {
            handle->debug_print("pmw3901mb: read timeout.\n");                                             /* read timeout */
           
            return 4;                                                                                      /* return error */
        }
    }
}

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
uint8_t pmw3901mb_set_optimum_performance(pmw3901mb_handle_t *handle)
{
    uint8_t res;
    uint8_t cmd;
    uint8_t retry_times;
    
    if (handle == NULL)                                                      /* check handle */
    {
        return 2;                                                            /* return error */
    }
    if (handle->inited != 1)                                                 /* check handle initialization */
    {
        return 3;                                                            /* return error */
    }
    
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x01;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x55, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x07;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x50, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x0E;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    
    retry_times = 3;                                                         /* set retry times 3 */
    
    start:
    
    cmd = 0x10;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x43, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, 0x47, (uint8_t *)&cmd, 1);            /* read */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: read failed.\n");                    /* read failed */
       
        return 1;                                                            /* return error */
    }
    if (cmd != 0x08)                                                         /* check the result */
    {
        if (retry_times != 0)                                                /* check retry times */
        {
            retry_times--;                                                   /* retry times-- */
            handle->delay_ms(100);                                           /* delay 100 ms */
            
            goto start;                                                      /* goto start */
        }
        else
        {
            handle->debug_print("pmw3901mb: set failed.\n");                 /* set failed */
           
            return 1;                                                        /* return error */
        }
    }
    res = a_pmw3901mb_spi_read(handle, 0x67, (uint8_t *)&cmd, 1);            /* read */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: read failed.\n");                    /* read failed */
       
        return 1;                                                            /* return error */
    }
    if ((cmd & (1 << 7)) != 0)
    {
        cmd = 0x04;                                                          /* set the command */
        res = a_pmw3901mb_spi_write(handle, 0x48, (uint8_t *)&cmd, 1);       /* sent the command */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: sent the command failed.\n");    /* sent the command failed */
           
            return 1;                                                        /* return error */
        }
    }
    else
    {
        cmd = 0x02;                                                          /* set the command */
        res = a_pmw3901mb_spi_write(handle, 0x48, (uint8_t *)&cmd, 1);       /* sent the command */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: sent the command failed.\n");    /* sent the command failed */
           
            return 1;                                                        /* return error */
        }
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x7B;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x51, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x50, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x55, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x0E;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, 0x73, (uint8_t *)&cmd, 1);            /* read */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: read failed.\n");                    /* read failed */
       
        return 1;                                                            /* return error */
    }
    if (cmd == 0x00)                                                         /* check status */
    {
        uint8_t c1;
        uint8_t c2;
        
        res = a_pmw3901mb_spi_read(handle, 0x70, (uint8_t *)&c1, 1);         /* read */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: read failed.\n");                /* read failed */
           
            return 1;                                                        /* return error */
        }
        if (c1 <= 28)                                                        /* check c1 */
        {
            c1 = c1 + 14;                                                    /* c1 = c1 + 14 */
        }
        else
        {
            c1 = c1 + 11;                                                    /* c1 = c1 + 11 */
        }
        if (c1 > 0x3F)                                                       /* check c1 */
        {
            c1 = 0x3F;                                                       /* max 0x3F */
        }
        res = a_pmw3901mb_spi_read(handle, 0x71, (uint8_t *)&c2, 1);         /* read */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: read failed.\n");                /* read failed */
           
            return 1;                                                        /* return error */
        }
        c2 = (uint8_t)(((uint32_t)c2 * 45) / 100);                           /* get c2 */
        cmd = 0x00;                                                          /* set the command */
        res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);       /* sent the command */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: sent the command failed.\n");    /* sent the command failed */
           
            return 1;                                                        /* return error */
        }
        cmd = 0xAD;                                                          /* set the command */
        res = a_pmw3901mb_spi_write(handle, 0x61, (uint8_t *)&cmd, 1);       /* sent the command */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: sent the command failed.\n");    /* sent the command failed */
           
            return 1;                                                        /* return error */
        }
        cmd = 0x70;                                                          /* set the command */
        res = a_pmw3901mb_spi_write(handle, 0x51, (uint8_t *)&cmd, 1);       /* sent the command */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: sent the command failed.\n");    /* sent the command failed */
           
            return 1;                                                        /* return error */
        }
        cmd = 0x0E;                                                          /* set the command */
        res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);       /* sent the command */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: sent the command failed.\n");    /* sent the command failed */
           
            return 1;                                                        /* return error */
        }
        cmd = c1;                                                            /* set the command */
        res = a_pmw3901mb_spi_write(handle, 0x70, (uint8_t *)&cmd, 1);       /* sent the command */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: sent the command failed.\n");    /* sent the command failed */
           
            return 1;                                                        /* return error */
        }
        cmd = c2;                                                            /* set the command */
        res = a_pmw3901mb_spi_write(handle, 0x71, (uint8_t *)&cmd, 1);       /* sent the command */
        if (res != 0)                                                        /* check result */
        {
            handle->debug_print("pmw3901mb: sent the command failed.\n");    /* sent the command failed */
           
            return 1;                                                        /* return error */
        }
    }
    
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xAD;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x61, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x03;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x40, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x05;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xB3;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x41, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xF1;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x43, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x14;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x45, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x32;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x5B, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x34;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x5F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x08;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7B, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x06;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x1B;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x44, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xBF;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x40, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x3F;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4E, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x08;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x20;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x65, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x18;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x6A, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x09;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xAF;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x40;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x5F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x48, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x49, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x77;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x57, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x78;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x60, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x78;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x61, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x08;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x62, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x50;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x63, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x0A;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x60;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x45, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x11;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4D, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x55, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x1F;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x74, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x1F;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x75, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x78;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4A, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x78;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4B, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x08;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x44, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x50;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x45, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xFF;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x64, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x1F;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x65, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x14;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x67;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x65, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x08;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x66, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x70;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x63, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x15;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x48;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x48, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x07;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x0D;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x41, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x14;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x43, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x0E;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4B, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x0F;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x45, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x42;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x44, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4C, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x10;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x02;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x5B, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x07;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x41;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x40, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x70, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    handle->delay_ms(10);                                                    /* delay 10 ms */
    cmd = 0x44;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x32, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x07;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x40;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x40, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x06;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xF0;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x62, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x63, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x0D;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xC0;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x48, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xD5;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x6F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x00;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x7F, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xA0;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x5B, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0xA8;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x4E, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x50;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x5A, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    cmd = 0x80;                                                              /* set the command */
    res = a_pmw3901mb_spi_write(handle, 0x40, (uint8_t *)&cmd, 1);           /* sent the command */
    if (res != 0)                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: sent the command failed.\n");        /* sent the command failed */
       
        return 1;                                                            /* return error */
    }
    
    return 0;                                                                /* success return 0 */
}

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
uint8_t pmw3901mb_get_product_id(pmw3901mb_handle_t *handle, uint8_t *id)
{
    uint8_t res;
    
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_PRODUCT_ID, (uint8_t *)id, 1);       /* get product id */
    if (res != 0)                                                                         /* check result */
    {
        handle->debug_print("pmw3901mb: get product id failed.\n");                       /* get product id failed */
       
        return 1;                                                                         /* return error */
    }
    
    return 0;                                                                             /* success return 0 */
}

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
uint8_t pmw3901mb_get_inverse_product_id(pmw3901mb_handle_t *handle, uint8_t *id)
{
    uint8_t res;
    
    if (handle == NULL)                                                                           /* check handle */
    {
        return 2;                                                                                 /* return error */
    }
    if (handle->inited != 1)                                                                      /* check handle initialization */
    {
        return 3;                                                                                 /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_INVERSE_PRODUCT_ID, (uint8_t *)id, 1);       /* get inverse product id */
    if (res != 0)                                                                                 /* check result */
    {
        handle->debug_print("pmw3901mb: get inverse product id failed.\n");                       /* get inverse product id failed */
       
        return 1;                                                                                 /* return error */
    }
    
    return 0;                                                                                     /* success return 0 */
}

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
uint8_t pmw3901mb_get_revision_id(pmw3901mb_handle_t *handle, uint8_t *id)
{
    uint8_t res;
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_REVISION_ID, (uint8_t *)id, 1);       /* get revision id */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: get revision id failed.\n");                       /* get revision id failed */
       
        return 1;                                                                          /* return error */
    }
    
    return 0;                                                                              /* success return 0 */
}

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
uint8_t pmw3901mb_reset(pmw3901mb_handle_t *handle)
{
    uint8_t res;
    uint8_t cmd;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    cmd = 0x5A;                                                                                  /* power up command */
    res = a_pmw3901mb_spi_write(handle, PMW3901MB_REG_POWER_UP_RESET, (uint8_t *)&cmd, 1);       /* set power up reset */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: set power up reset failed.\n");                          /* set power up reset failed */
       
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t pmw3901mb_shutdown(pmw3901mb_handle_t *handle)
{
    uint8_t res;
    uint8_t cmd;
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    cmd = 0xB6;                                                                            /* shutdown command */
    res = a_pmw3901mb_spi_write(handle, PMW3901MB_REG_SHUTDOWN, (uint8_t *)&cmd, 1);       /* set shutdown reset */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: set shutdown failed.\n");                          /* set shutdown failed */
       
        return 1;                                                                          /* return error */
    }
    
    return 0;                                                                              /* success return 0 */
}

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
uint8_t pmw3901mb_get_motion(pmw3901mb_handle_t *handle, uint8_t *motion)
{
    uint8_t res;
    
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_MOTION, (uint8_t *)motion, 1);       /* get motion */
    if (res != 0)                                                                         /* check result */
    {
        handle->debug_print("pmw3901mb: get motion failed.\n");                           /* get motion failed */
       
        return 1;                                                                         /* return error */
    }
    
    return 0;                                                                             /* success return 0 */
}

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
uint8_t pmw3901mb_set_motion(pmw3901mb_handle_t *handle, uint8_t motion)
{
    uint8_t res;
    
    if (handle == NULL)                                                                    /* check handle */
    {
        return 2;                                                                          /* return error */
    }
    if (handle->inited != 1)                                                               /* check handle initialization */
    {
        return 3;                                                                          /* return error */
    }
    
    res = a_pmw3901mb_spi_write(handle, PMW3901MB_REG_MOTION, (uint8_t *)&motion, 1);      /* set motion */
    if (res != 0)                                                                          /* check result */
    {
        handle->debug_print("pmw3901mb: set motion failed.\n");                            /* set motion failed */
       
        return 1;                                                                          /* return error */
    }
    
    return 0;                                                                              /* success return 0 */
}

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
uint8_t pmw3901mb_get_delta_x(pmw3901mb_handle_t *handle, int16_t *delta)
{
    uint8_t res;
    uint8_t msb;
    uint8_t lsb;
    
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_DELTA_X_L, (uint8_t *)&lsb, 1);      /* get delta xl */
    if (res != 0)                                                                         /* check result */
    {
        handle->debug_print("pmw3901mb: get delta xl failed.\n");                         /* get delta xl failed */
       
        return 1;                                                                         /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_DELTA_X_H, (uint8_t *)&msb, 1);      /* get delta xh */
    if (res != 0)                                                                         /* check result */
    {
        handle->debug_print("pmw3901mb: get delta xh failed.\n");                         /* get delta xh failed */
       
        return 1;                                                                         /* return error */
    }
    *delta = (int16_t)(((uint16_t)(msb) << 8) | lsb);                                     /* get delta */
    
    return 0;                                                                             /* success return 0 */
}

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
uint8_t pmw3901mb_get_delta_y(pmw3901mb_handle_t *handle, int16_t *delta)
{
    uint8_t res;
    uint8_t msb;
    uint8_t lsb;
    
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_DELTA_Y_L, (uint8_t *)&lsb, 1);      /* get delta yl */
    if (res != 0)                                                                         /* check result */
    {
        handle->debug_print("pmw3901mb: get delta yl failed.\n");                         /* get delta yl failed */
       
        return 1;                                                                         /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_DELTA_Y_H, (uint8_t *)&msb, 1);      /* get delta yh */
    if (res != 0)                                                                         /* check result */
    {
        handle->debug_print("pmw3901mb: get delta yh failed.\n");                         /* get delta yh failed */
       
        return 1;                                                                         /* return error */
    }
    *delta = (int16_t)(((uint16_t)(msb) << 8) | lsb);                                     /* get delta */
    
    return 0;                                                                             /* success return 0 */
}

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
uint8_t pmw3901mb_get_motion_burst(pmw3901mb_handle_t *handle, uint8_t *burst, uint8_t len)
{
    uint8_t res;
    
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_MOTION_BURST, (uint8_t *)burst, len);     /* get motion burst */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("pmw3901mb: get motion burst failed.\n");                          /* get motion burst failed */
       
        return 1;                                                                              /* return error */
    }
    
    return 0;                                                                                  /* success return 0 */
}

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
uint8_t pmw3901mb_get_squal(pmw3901mb_handle_t *handle, uint8_t *squal)
{
    uint8_t res;
    
    if (handle == NULL)                                                                 /* check handle */
    {
        return 2;                                                                       /* return error */
    }
    if (handle->inited != 1)                                                            /* check handle initialization */
    {
        return 3;                                                                       /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_SQUAL, (uint8_t *)squal, 1);       /* get squal */
    if (res != 0)                                                                       /* check result */
    {
        handle->debug_print("pmw3901mb: get squal failed.\n");                          /* get squal failed */
       
        return 1;                                                                       /* return error */
    }
    
    return 0;                                                                           /* success return 0 */
}

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
uint8_t pmw3901mb_get_raw_data_sum(pmw3901mb_handle_t *handle, uint8_t *sum)
{
    uint8_t res;
    
    if (handle == NULL)                                                                      /* check handle */
    {
        return 2;                                                                            /* return error */
    }
    if (handle->inited != 1)                                                                 /* check handle initialization */
    {
        return 3;                                                                            /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_RAW_DATA_SUM, (uint8_t *)sum, 1);       /* get sum */
    if (res != 0)                                                                            /* check result */
    {
        handle->debug_print("pmw3901mb: get sum failed.\n");                                 /* get sum failed */
       
        return 1;                                                                            /* return error */
    }
    
    return 0;                                                                                /* success return 0 */
}

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
uint8_t pmw3901mb_get_max_raw_data(pmw3901mb_handle_t *handle, uint8_t *max)
{
    uint8_t res;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_MAXIMUM_RAW_DATA, (uint8_t *)max, 1);       /* get maximum raw data */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: get maximum raw data failed.\n");                        /* get maximum raw data failed */
       
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t pmw3901mb_get_min_raw_data(pmw3901mb_handle_t *handle, uint8_t *min)
{
    uint8_t res;
    
    if (handle == NULL)                                                                          /* check handle */
    {
        return 2;                                                                                /* return error */
    }
    if (handle->inited != 1)                                                                     /* check handle initialization */
    {
        return 3;                                                                                /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_MINIMUM_RAW_DATA, (uint8_t *)min, 1);       /* get minimum raw data */
    if (res != 0)                                                                                /* check result */
    {
        handle->debug_print("pmw3901mb: get minimum raw data failed.\n");                        /* get minimum raw data failed */
       
        return 1;                                                                                /* return error */
    }
    
    return 0;                                                                                    /* success return 0 */
}

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
uint8_t pmw3901mb_get_shutter(pmw3901mb_handle_t *handle, uint16_t *shutter)
{
    uint8_t res;
    uint8_t msb;
    uint8_t lsb;
    
    if (handle == NULL)                                                                        /* check handle */
    {
        return 2;                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                   /* check handle initialization */
    {
        return 3;                                                                              /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_SHUTTER_UPPER, (uint8_t *)&msb, 1);       /* get shutter upper */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("pmw3901mb: get shutter upper failed.\n");                         /* get shutter upper failed */
       
        return 1;                                                                              /* return error */
    }
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_SHUTTER_LOWER, (uint8_t *)&lsb, 1);       /* get shutter lower */
    if (res != 0)                                                                              /* check result */
    {
        handle->debug_print("pmw3901mb: get shutter lower failed.\n");                         /* get shutter lower failed */
       
        return 1;                                                                              /* return error */
    }
    *shutter = (uint16_t)(((uint16_t)(msb & 0x1F) << 8) | lsb);                                /* get shutter */
    
    return 0;                                                                                  /* success return 0 */
}

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
uint8_t pmw3901mb_get_observation(pmw3901mb_handle_t *handle, uint8_t *observation)
{
    uint8_t res;
    
    if (handle == NULL)                                                                             /* check handle */
    {
        return 2;                                                                                   /* return error */
    }
    if (handle->inited != 1)                                                                        /* check handle initialization */
    {
        return 3;                                                                                   /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_OBSERVATION, (uint8_t *)observation, 1);       /* get observation data */
    if (res != 0)                                                                                   /* check result */
    {
        handle->debug_print("pmw3901mb: get observation failed.\n");                                /* get observation failed */
       
        return 1;                                                                                   /* return error */
    }
    *observation = (*observation) & 0x3F;                                                           /* set the observation */
    
    return 0;                                                                                       /* success return 0 */
}

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
uint8_t pmw3901mb_set_observation(pmw3901mb_handle_t *handle, uint8_t observation)
{
    uint8_t res;
    
    if (handle == NULL)                                                                               /* check handle */
    {
        return 2;                                                                                     /* return error */
    }
    if (handle->inited != 1)                                                                          /* check handle initialization */
    {
        return 3;                                                                                     /* return error */
    }
    
    res = a_pmw3901mb_spi_write(handle, PMW3901MB_REG_OBSERVATION, (uint8_t *)&observation, 1);       /* set observation data */
    if (res != 0)                                                                                     /* check result */
    {
        handle->debug_print("pmw3901mb: set observation failed.\n");                                  /* set observation failed */
       
        return 1;                                                                                     /* return error */
    }
    
    return 0;                                                                                         /* success return 0 */
}

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
uint8_t pmw3901mb_get_raw_data_grab(pmw3901mb_handle_t *handle, uint8_t *grab, uint16_t len)
{
    uint8_t res;
    uint16_t i;
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    for (i = 0; i < len; i++)                                                                          /* length times */
    {
        res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_RAW_DATA_GRAB, (uint8_t *)&grab[i], 1);       /* get raw data grab data */
        if (res != 0)                                                                                  /* check result */
        {
            handle->debug_print("pmw3901mb: get raw data grab failed.\n");                             /* get raw data grab failed */
           
            return 1;                                                                                  /* return error */
        }
    }
    
    return 0;                                                                                          /* success return 0 */
}

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
uint8_t pmw3901mb_set_raw_data_grab(pmw3901mb_handle_t *handle, uint8_t *grab, uint16_t len)
{
    uint8_t res;
    uint16_t i;
    
    if (handle == NULL)                                                                                /* check handle */
    {
        return 2;                                                                                      /* return error */
    }
    if (handle->inited != 1)                                                                           /* check handle initialization */
    {
        return 3;                                                                                      /* return error */
    }
    
    for (i = 0; i < len; i++)                                                                          /* length times */
    {
        res = a_pmw3901mb_spi_write(handle, PMW3901MB_REG_RAW_DATA_GRAB, (uint8_t *)&grab[i], 1);      /* set raw data grab */
        if (res != 0)                                                                                  /* check result */
        {
            handle->debug_print("pmw3901mb: set raw data grab failed.\n");                             /* set raw data grab failed */
           
            return 1;                                                                                  /* return error */
        }
    }
    
    return 0;                                                                                          /* success return 0 */
}

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
uint8_t pmw3901mb_get_raw_data_grab_status(pmw3901mb_handle_t *handle, uint8_t *status)
{
    uint8_t res;
    
    if (handle == NULL)                                                                                 /* check handle */
    {
        return 2;                                                                                       /* return error */
    }
    if (handle->inited != 1)                                                                            /* check handle initialization */
    {
        return 3;                                                                                       /* return error */
    }
    
    res = a_pmw3901mb_spi_read(handle, PMW3901MB_REG_RAW_DATA_GRAB_STATUS, (uint8_t *)status, 1);       /* get raw data grab status */
    if (res != 0)                                                                                       /* check result */
    {
        handle->debug_print("pmw3901mb: get raw data grab status failed.\n");                           /* get raw data grab status failed */
       
        return 1;                                                                                       /* return error */
    }
    
    return 0;                                                                                           /* success return 0 */
}

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
uint8_t pmw3901mb_delta_raw_to_delta_cm(pmw3901mb_handle_t *handle, int16_t raw, float height_m, float *cm)
{
    if (handle == NULL)                                     /* check handle */
    {
        return 2;                                           /* return error */
    }
    if (handle->inited != 1)                                /* check handle initialization */
    {
        return 3;                                           /* return error */
    }
    
    *cm = (float)(raw) * height_m / 11.914f * 2.54f;        /* convert */
    
    return 0;                                               /* success return 0 */
}

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
uint8_t pmw3901mb_set_reg(pmw3901mb_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }
    if (handle->inited != 1)                                   /* check handle initialization */
    {
        return 3;                                              /* return error */
    }
    
    return a_pmw3901mb_spi_write(handle, reg, buf, len);       /* write data */
}

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
uint8_t pmw3901mb_get_reg(pmw3901mb_handle_t *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                       /* check handle */
    {
        return 2;                                             /* return error */
    }
    if (handle->inited != 1)                                  /* check handle initialization */
    {
        return 3;                                             /* return error */
    }
    
    return a_pmw3901mb_spi_read(handle, reg, buf, len);       /* read data */
}

/**
 * @brief      get chip's information
 * @param[out] *info points to a pmw3901mb info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t pmw3901mb_info(pmw3901mb_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }
    
    memset(info, 0, sizeof(pmw3901mb_info_t));                      /* initialize pmw3901mb info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "SPI", 8);                             /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */
    
    return 0;                                                       /* success return 0 */
}
