[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver PMW3901MB

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/pmw3901mb/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

PMW3901MBは、空中でのナビゲーションを可能にする遠方界光学技術で設計されたPixArtImagingの最新の光学ナビゲーションチップです。 28ピンのチップオンボード（COB）パッケージに収められており、80mmから無限大までの広い動作範囲でX-Yモーション情報を提供します。 動き検出の遠方界アプリケーションに最適です。

LibDriver PMW3901MBは、LibDriverによって起動されたpmw3901mbの全機能ドライバーです。 フレーム読み取り、ナビゲーション読み取り、割り込み読み取り、およびその他の機能を提供します。LibDriverはMISRAに準拠しています。

### 目次

  - [説明](#説明)
  - [インストール](#インストール)
  - [使用](#使用)
    - [example basic](#example-basic)
    - [example frame](#example-frame)
    - [example interrupt](#example-interrupt)
  - [ドキュメント](#ドキュメント)
  - [貢献](#貢献)
  - [著作権](#著作権)
  - [連絡して](#連絡して)

### 説明

/ srcディレクトリには、LibDriver PMW3901MBのソースファイルが含まれています。

/ interfaceディレクトリには、LibDriver PMW3901MB用のプラットフォームに依存しないSPIバステンプレートが含まれています。

/ testディレクトリには、チップの必要な機能を簡単にテストできるLibDriver PMW3901MBドライバーテストプログラムが含まれています。

/ exampleディレクトリには、LibDriver PMW3901MBプログラミング例が含まれています。

/ docディレクトリには、LibDriver PMW3901MBオフラインドキュメントが含まれています。

/ datasheetディレクトリには、PMW3901MBデータシートが含まれています。

/ projectディレクトリには、一般的に使用されるLinuxおよびマイクロコントローラー開発ボードのプロジェクトサンプルが含まれています。 すべてのプロジェクトは、デバッグ方法としてシェルスクリプトを使用しています。詳細については、各プロジェクトのREADME.mdを参照してください。

/ misraはLibDriver misraコードスキャン結果を含む。

### インストール

/ interfaceディレクトリにあるプラットフォームに依存しないSPIバステンプレートを参照して、指定したプラットフォームのSPIバスドライバを完成させます。

/src ディレクトリ、プラットフォームのインターフェイス ドライバー、および独自のドライバーをプロジェクトに追加します。デフォルトのサンプル ドライバーを使用する場合は、/example ディレクトリをプロジェクトに追加します。

### 使用

/example ディレクトリ内のサンプルを参照して、独自のドライバーを完成させることができます。 デフォルトのプログラミング例を使用したい場合の使用方法は次のとおりです。

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

### ドキュメント

オンラインドキュメント: [https://www.libdriver.com/docs/pmw3901mb/index.html](https://www.libdriver.com/docs/pmw3901mb/index.html)。

オフラインドキュメント: /doc/html/index.html。

### 貢献

CONTRIBUTING.mdを参照してください。

### 著作権

著作権（c）2015-今 LibDriver 全著作権所有

MITライセンス（MIT）

このソフトウェアおよび関連するドキュメントファイル（「ソフトウェア」）のコピーを取得した人は、無制限の使用、複製、変更、組み込み、公開、配布、サブライセンスを含む、ソフトウェアを処分する権利を制限なく付与されます。ソフトウェアのライセンスおよび/またはコピーの販売、および上記のようにソフトウェアが配布された人の権利のサブライセンスは、次の条件に従うものとします。

上記の著作権表示およびこの許可通知は、このソフトウェアのすべてのコピーまたは実体に含まれるものとします。

このソフトウェアは「現状有姿」で提供され、商品性、特定目的への適合性、および非侵害の保証を含むがこれらに限定されない、明示または黙示を問わず、いかなる種類の保証もありません。 いかなる場合も、作者または著作権所有者は、契約、不法行為、またはその他の方法で、本ソフトウェアおよび本ソフトウェアの使用またはその他の廃棄に起因または関連して、請求、損害、またはその他の責任を負わないものとします。

### 連絡して

お問い合わせくださいlishifenging@outlook.com。