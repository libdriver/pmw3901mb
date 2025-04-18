[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver PMW3901MB

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/pmw3901mb/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

PMW3901MB는 공중 탐색을 가능하게 하는 원거리 광학 기술로 설계된 PixArt Imaging의 최신 광학 탐색 칩입니다. 이 제품은 80mm에서 무한대의 넓은 작동 범위로 X-Y 모션 정보를 제공하는 28핀 COB(칩 온보드) 패키지에 들어 있습니다. 모션 감지를 위한 원거리 애플리케이션에 가장 적합합니다.

LibDriver PMW3901MB는 LibDriver에서 출시한 PMW3901MB의 전체 기능 드라이버입니다. 프레임 읽기, 탐색 읽기, 인터럽트 읽기 및 LibDriver가 MISRA와 호환되는 기타 기능을 제공합니다.

### 콘텐츠

  - [설명](#설명)
  - [설치](#설치)
  - [사용](#사용)
    - [example basic](#example-basic)
    - [example frame](#example-frame)
    - [example interrupt](#example-interrupt)
  - [문서](#문서)
  - [기고](#기고)
  - [저작권](#저작권)
  - [문의하기](#문의하기)

### 설명

/src 디렉토리에는 LibDriver PMW3901MB의 소스 파일이 포함되어 있습니다.

/interface 디렉토리에는 LibDriver PMW3901MB용 플랫폼 독립적인 SPI버스 템플릿이 포함되어 있습니다.

/test 디렉토리에는 LibDriver PMW3901MB드라이버 테스트 프로그램이 포함되어 있어 칩의 필요한 기능을 간단히 테스트할 수 있습니다.

/example 디렉토리에는 LibDriver PMW3901MB프로그래밍 예제가 포함되어 있습니다.

/doc 디렉토리에는 LibDriver PMW3901MB오프라인 문서가 포함되어 있습니다.

/datasheet 디렉토리에는 PMW3901MB데이터시트가 있습니다.

/project 디렉토리에는 일반적으로 사용되는 Linux 및 마이크로컨트롤러 개발 보드의 프로젝트 샘플이 포함되어 있습니다. 모든 프로젝트는 디버깅 방법으로 셸 스크립트를 사용하며, 자세한 내용은 각 프로젝트의 README.md를 참조하십시오.

/misra 에는 LibDriver misra 코드 검색 결과가 포함됩니다.

### 설치

/interface 디렉토리에서 플랫폼 독립적인SPI버스 템플릿을 참조하여 지정된 플랫폼에 대한 SPI버스 드라이버를 완성하십시오.

/src 디렉터리, 플랫폼용 인터페이스 드라이버 및 자체 드라이버를 프로젝트에 추가합니다. 기본 예제 드라이버를 사용하려면 /example 디렉터리를 프로젝트에 추가합니다.

### 사용

/example 디렉터리의 예제를 참조하여 자신만의 드라이버를 완성할 수 있습니다. 기본 프로그래밍 예제를 사용하려는 경우 사용 방법은 다음과 같습니다.

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

```c
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

### 문서

온라인 문서: [https://www.libdriver.com/docs/pmw3901mb/index.html](https://www.libdriver.com/docs/pmw3901mb/index.html).

오프라인 문서: /doc/html/index.html.

### 기고

CONTRIBUTING.md 를 참조하십시오.

### 저작권

저작권 (c) 2015 - 지금 LibDriver 판권 소유

MIT 라이선스(MIT)

이 소프트웨어 및 관련 문서 파일("소프트웨어")의 사본을 얻은 모든 사람은 이에 따라 무제한 사용, 복제, 수정, 통합, 출판, 배포, 2차 라이선스를 포함하여 소프트웨어를 처분할 수 있는 권리가 부여됩니다. 소프트웨어의 사본에 대한 라이선스 및/또는 판매, 그리고 소프트웨어가 위와 같이 배포된 사람의 권리에 대한 2차 라이선스는 다음 조건에 따릅니다.

위의 저작권 표시 및 이 허가 표시는 이 소프트웨어의 모든 사본 또는 내용에 포함됩니다.

이 소프트웨어는 상품성, 특정 목적에의 적합성 및 비침해에 대한 보증을 포함하되 이에 국한되지 않는 어떠한 종류의 명시적 또는 묵시적 보증 없이 "있는 그대로" 제공됩니다. 어떤 경우에도 저자 또는 저작권 소유자는 계약, 불법 행위 또는 기타 방식에 관계없이 소프트웨어 및 기타 소프트웨어 사용으로 인해 발생하거나 이와 관련하여 발생하는 청구, 손해 또는 기타 책임에 대해 책임을 지지 않습니다.

### 문의하기

연락주세요lishifenging@outlook.com.