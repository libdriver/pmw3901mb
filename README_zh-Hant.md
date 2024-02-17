[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver PMW3901MB

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/pmw3901mb/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

PMW3901MB 是 PixArt Imaging 最新的光學導航芯片，採用遠場光學技術設計，可實現空中導航。它採用 28 引腳板載芯片 (COB) 封裝，提供 X-Y 運動信息，工作範圍從 80 毫米到無窮遠。它最適合運動檢測的遠場應用。

LibDriver PMW3901MB 是LibDriver推出的PMW3901MB 的全功能驅動，該驅動提供原始數據幀讀取，導航數據讀取，中斷讀取等功能並且它符合MISRA標準。

### 目錄

  - [說明](#說明)
  - [安裝](#安裝)
  - [使用](#使用)
    - [example basic](#example-basic)
    - [example frame](#example-frame)
    - [example interrupt](#example-interrupt)
  - [文檔](#文檔)
  - [貢獻](#貢獻)
  - [版權](#版權)
  - [聯繫我們](#聯繫我們)

### 說明

/src目錄包含了LibDriver PMW3901MB的源文件。

/interface目錄包含了LibDriver PMW3901MB與平台無關的SPI總線模板。

/test目錄包含了LibDriver PMW3901MB驅動測試程序，該程序可以簡單的測試芯片必要功能。

/example目錄包含了LibDriver PMW3901MB編程範例。

/doc目錄包含了LibDriver PMW3901MB離線文檔。

/datasheet目錄包含了PMW3901MB數據手冊。

/project目錄包含了常用Linux與單片機開發板的工程樣例。所有工程均採用shell腳本作為調試方法，詳細內容可參考每個工程裡面的README.md。

/misra目錄包含了LibDriver MISRA程式碼掃描結果。

### 安裝

參考/interface目錄下與平台無關的SPI總線模板，完成指定平台的SPI總線驅動。

將/src目錄，您使用平臺的介面驅動和您開發的驅動加入工程，如果您想要使用默認的範例驅動，可以將/example目錄加入您的工程。

### 使用

您可以參考/example目錄下的程式設計範例完成適合您的驅動，如果您想要使用默認的程式設計範例，以下是它們的使用方法。

#### example basic

```C
#include "driver_pmw3901mb_basic.h"

uint8_t res;
float height;
float delta_x;
float delta_y;
uint32_t i, times;
pmw3901mb_motion_t motion;

height = 1.0f;
times = 3;
res = pmw3901mb_basic_init();
if (res ！= 0)
{
    return 1;
}

...

for (i = 0; i < times; i++)
{
    read:

    res = pmw3901mb_basic_read(height, &motion, (float *)&delta_x, (float *)&delta_y);
    if (res ！= 0)
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
        pmw3901mb_interface_debug_print("pmw3901mb: surface quality is 0x%04X.\n\n", motion.surface_quality);
    }
    else
    {
        pmw3901mb_interface_delay_ms(500);

        goto read;
    }

    /* delay 1000 ms */
    pmw3901mb_interface_delay_ms(1000);
    
    ...
}

...
    
(void)pmw3901mb_basic_deinit();

return 0;
```

#### example frame

```C
#include "driver_pmw3901mb_frame.h"

uint8_t res;
uint32_t k, times;
uint32_t i, j;
static uint8_t gs_frame[35][35];

times = 3;
res = pmw3901mb_frame_init();
if (res != 0)
{
    return 1;
}

...

for (k = 0; k < times; k++)
{
    res = pmw3901mb_frame_read(gs_frame);
    if (res != 0)
    {
        (void)pmw3901mb_frame_deinit();

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
    
    ...
}

...
    
(void)pmw3901mb_frame_deinit();

return 0;
```

#### example interrupt

```C
#include "driver_pmw3901mb_interrupt.h"

uint8_t (*g_gpio_irq)(float height_m) = NULL;
static uint8_t gs_flag;
uint8_t res;
uint32_t i, times;

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
        pmw3901mb_interface_debug_print("pmw3901mb: surface quality is 0x%04X.\n\n", motion->surface_quality);
        
        /* set flag */
        gs_flag = 1;
    }
}

...
    
times = 3;
res = gpio_interrupt_init();
if (res != 0)
{
    return 1;
}
g_gpio_irq = pmw3901mb_interrupt_irq_handler;

...

res = pmw3901mb_interrupt_init(_callback);
if (res != 0)
{
    (void)pmw3901mb_interrupt_deinit();

    return 1;
}

...

for (i = 0; i < times; i++)
{
    pmw3901mb_interface_debug_print("pmw3901mb: %d/%d.\n", i + 1, times);

    gs_flag = 0;
    while (gs_flag == 0)
    {
        pmw3901mb_interface_delay_ms(10);
    }
    
    ...
}

...
    
(void)pmw3901mb_interrupt_deinit();
(void)gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

### 文檔

在線文檔: [https://www.libdriver.com/docs/pmw3901mb/index.html](https://www.libdriver.com/docs/pmw3901mb/index.html)。

離線文檔: /doc/html/index.html。

### 貢獻

請參攷CONTRIBUTING.md。

### 版權

版權 (c) 2015 - 現在 LibDriver 版權所有

MIT 許可證（MIT）

特此免費授予任何獲得本軟件副本和相關文檔文件（下稱“軟件”）的人不受限制地處置該軟件的權利，包括不受限制地使用、複製、修改、合併、發布、分發、轉授許可和/或出售該軟件副本，以及再授權被配發了本軟件的人如上的權利，須在下列條件下：

上述版權聲明和本許可聲明應包含在該軟件的所有副本或實質成分中。

本軟件是“如此”提供的，沒有任何形式的明示或暗示的保證，包括但不限於對適銷性、特定用途的適用性和不侵權的保證。在任何情況下，作者或版權持有人都不對任何索賠、損害或其他責任負責，無論這些追責來自合同、侵權或其它行為中，還是產生於、源於或有關於本軟件以及本軟件的使用或其它處置。

### 聯繫我們

請聯繫lishifenging@outlook.com。