[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver PMW3901MB

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/pmw3901mb/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE) 

Der PMW3901MB ist der neueste optische Navigationschip von PixArt Imaging, der mit Fernfeld-Optiktechnologie entwickelt wurde, die die Navigation in der Luft ermöglicht. Es ist in einem 28-poligen Chip-on-Board (COB)-Gehäuse untergebracht, das X-Y-Bewegungsinformationen mit einem großen Arbeitsbereich von 80 mm bis unendlich liefert. Es eignet sich am besten für Fernfeldanwendungen zur Bewegungserkennung.

LibDriver PMW3901MB ist der voll funktionsfähige Treiber von pmw3901mb, der von LibDriver gestartet wurde. Es bietet Frame-Lesen, Navigations-Lesen, Interrupt-Lesen und andere Funktionen. LibDriver ist MISRA-kompatibel.

### Inhaltsverzeichnis

  - [Anweisung](#Anweisung)
  - [Installieren](#Installieren)
  - [Nutzung](#Nutzung)
    - [example basic](#example-basic)
    - [example frame](#example-frame)
    - [example interrupt](#example-interrupt)
  - [Dokument](#Dokument)
  - [Beitrag](#Beitrag)
  - [Lizenz](#Lizenz)
  - [Kontaktieren Sie uns](#Kontaktieren-Sie-uns)

### Anweisung

/src enthält LibDriver PMW3901MB-Quelldateien.

/interface enthält die plattformunabhängige Vorlage LibDriver PMW3901MB SPI.

/test enthält den Testcode des LibDriver PMW3901MB-Treibers und dieser Code kann die erforderliche Funktion des Chips einfach testen.

/example enthält LibDriver PMW3901MB-Beispielcode.

/doc enthält das LibDriver PMW3901MB-Offlinedokument.

/Datenblatt enthält PMW3901MB-Datenblatt.

/project enthält den allgemeinen Beispielcode für Linux- und MCU-Entwicklungsboards. Alle Projekte verwenden das Shell-Skript, um den Treiber zu debuggen, und die detaillierten Anweisungen finden Sie in der README.md jedes Projekts.

/misra enthält die Ergebnisse des LibDriver MISRA Code Scans.

### Installieren

Verweisen Sie auf eine plattformunabhängige SPI-Schnittstellenvorlage und stellen Sie Ihren Plattform-SPI-Treiber fertig.

Fügen Sie das Verzeichnis /src, den Schnittstellentreiber für Ihre Plattform und Ihre eigenen Treiber zu Ihrem Projekt hinzu. Wenn Sie die Standardbeispieltreiber verwenden möchten, fügen Sie das Verzeichnis /example zu Ihrem Projekt hinzu.

### Nutzung

Sie können auf die Beispiele im Verzeichnis /example zurückgreifen, um Ihren eigenen Treiber zu vervollständigen. Wenn Sie die Standardprogrammierbeispiele verwenden möchten, erfahren Sie hier, wie Sie diese verwenden.

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

### Dokument

Online-Dokumente: [https://www.libdriver.com/docs/pmw3901mb/index.html](https://www.libdriver.com/docs/pmw3901mb/index.html).

Offline-Dokumente: /doc/html/index.html.

### Beitrag

Bitte beachten Sie CONTRIBUTING.md.

### Lizenz

Urheberrechte © (c) 2015 - Gegenwart LibDriver Alle Rechte vorbehalten



Die MIT-Lizenz (MIT)



Hiermit wird jeder Person kostenlos die Erlaubnis erteilt, eine Kopie zu erhalten

dieser Software und zugehörigen Dokumentationsdateien (die „Software“) zu behandeln

in der Software ohne Einschränkung, einschließlich, aber nicht beschränkt auf die Rechte

zu verwenden, zu kopieren, zu modifizieren, zusammenzuführen, zu veröffentlichen, zu verteilen, unterzulizenzieren und/oder zu verkaufen

Kopien der Software und Personen, denen die Software gehört, zu gestatten

dazu eingerichtet werden, unter folgenden Bedingungen:



Der obige Urheberrechtshinweis und dieser Genehmigungshinweis müssen in allen enthalten sein

Kopien oder wesentliche Teile der Software.



DIE SOFTWARE WIRD "WIE BESEHEN" BEREITGESTELLT, OHNE JEGLICHE GEWÄHRLEISTUNG, AUSDRÜCKLICH ODER

STILLSCHWEIGEND, EINSCHLIESSLICH, ABER NICHT BESCHRÄNKT AUF DIE GEWÄHRLEISTUNG DER MARKTGÄNGIGKEIT,

EIGNUNG FÜR EINEN BESTIMMTEN ZWECK UND NICHTVERLETZUNG VON RECHTEN DRITTER. IN KEINEM FALL DARF DAS

AUTOREN ODER URHEBERRECHTSINHABER HAFTEN FÜR JEGLICHE ANSPRÜCHE, SCHÄDEN ODER ANDERE

HAFTUNG, OB AUS VERTRAG, DELIKT ODER ANDERWEITIG, ENTSTEHEND AUS,

AUS ODER IM ZUSAMMENHANG MIT DER SOFTWARE ODER DER VERWENDUNG ODER ANDEREN HANDLUNGEN MIT DER

SOFTWARE.

### Kontaktieren Sie uns

Bitte senden Sie eine E-Mail an lishifenging@outlook.com.