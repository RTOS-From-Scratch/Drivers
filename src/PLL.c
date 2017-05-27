/*
 * PLL.c
 *
 *  Created on: Jun 20, 2016
 *      Author: leo-0
 */

#include "PLL.h"
#include <Misc/src/definitions.h>
#include "tm4c123gh6pm.h"

/* PLL (XTAL) frequency
0x00-0x5  reserved
  0x06    4 MHz
  0x07    4.096 MHz
  0x08    4.9152 MHz
  0x09    5 MHz (USB)
  0x0A    5.12 MHz
  0x0B    6 MHz (USB)
  0x0C    6.144 MHz
  0x0D    7.3728 MHz
  0x0E    8 MHz (USB)
  0x0F    8.192 MHz
  0x10    10.0 MHz (USB)
  0x11    12.0 MHz (USB)
  0x12    12.288 MHz
  0x13    13.56 MHz
  0x14    14.31818 MHz
  0x15    16.0 MHz (USB)
  0x16    16.384 MHz
  0x17    18.0 MHz (USB)
  0x18    20.0 MHz (USB)
  0x19    24.0 MHz (USB)
  0x1A    25.0 MHz (USB)
*/

/* system divisor
SYSDIV2  Divisor  Clock (MHz)
 0        1       reserved
 1        2       reserved
 2        3       reserved
 3        4       reserved
 4        5       80.000
 5        6       66.667
 6        7       reserved
 7        8       50.000
 8        9       44.444
 9        10      40.000
 10       11      36.364
 11       12      33.333
 12       13      30.769
 13       14      28.571
 14       15      26.667
 15       16      25.000
 16       17      23.529
 17       18      22.222
 18       19      21.053
 19       20      20.000
 20       21      19.048
 21       22      18.182
 22       23      17.391
 23       24      16.667
 24       25      16.000
 25       26      15.385
 26       27      14.815
 27       28      14.286
 28       29      13.793
 29       30      13.333
 30       31      12.903
 31       32      12.500
 32       33      12.121
 33       34      11.765
 34       35      11.429
 35       36      11.111
 36       37      10.811
 37       38      10.526
 38       39      10.256
 39       40      10.000
 40       41      9.756
 41       42      9.524
 42       43      9.302
 43       44      9.091
 44       45      8.889
 45       46      8.696
 46       47      8.511
 47       48      8.333
 48       49      8.163
 49       50      8.000
 50       51      7.843
 51       52      7.692
 52       53      7.547
 53       54      7.407
 54       55      7.273
 55       56      7.143
 56       57      7.018
 57       58      6.897
 58       59      6.780
 59       60      6.667
 60       61      6.557
 61       62      6.452
 62       63      6.349
 63       64      6.250
 64       65      6.154
 65       66      6.061
 66       67      5.970
 67       68      5.882
 68       69      5.797
 69       70      5.714
 70       71      5.634
 71       72      5.556
 72       73      5.479
 73       74      5.405
 74       75      5.333
 75       76      5.263
 76       77      5.195
 77       78      5.128
 78       79      5.063
 79       80      5.000
 80       81      4.938
 81       82      4.878
 82       83      4.819
 83       84      4.762
 84       85      4.706
 85       86      4.651
 86       87      4.598
 87       88      4.545
 88       89      4.494
 89       90      4.444
 90       91      4.396
 91       92      4.348
 92       93      4.301
 93       94      4.255
 94       95      4.211
 95       96      4.167
 96       97      4.124
 97       98      4.082
 98       99      4.040
 99       100     4.000
 100      101     3.960
 101      102     3.922
 102      103     3.883
 103      104     3.846
 104      105     3.810
 105      106     3.774
 106      107     3.738
 107      108     3.704
 108      109     3.670
 109      110     3.636
 110      111     3.604
 111      112     3.571
 112      113     3.540
 113      114     3.509
 114      115     3.478
 115      116     3.448
 116      117     3.419
 117      118     3.390
 118      119     3.361
 119      120     3.333
 120      121     3.306
 121      122     3.279
 122      123     3.252
 123      124     3.226
 124      125     3.200
 125      126     3.175
 126      127     3.150
 127      128     3.125
*/

typedef enum XTAL {
    XTAL_MHZ_4    = SYSCTL_RCC_XTAL_4MHZ,
    XTAL_MHZ_4_09 = SYSCTL_RCC_XTAL_4_09MHZ,
    XTAL_MHZ_4_91 = SYSCTL_RCC_XTAL_4_91MHZ,
    XTAL_MHZ_5    = SYSCTL_RCC_XTAL_5MHZ,
    XTAL_MHZ_5_12 = SYSCTL_RCC_XTAL_5_12MHZ,
    XTAL_MHZ_6    = SYSCTL_RCC_XTAL_6MHZ,
    XTAL_MHZ_6_14 = SYSCTL_RCC_XTAL_6_14MHZ,
    XTAL_MHZ_7_37 = SYSCTL_RCC_XTAL_7_37MHZ,
    XTAL_MHZ_8    = SYSCTL_RCC_XTAL_8MHZ,
    XTAL_MHZ_8_19 = SYSCTL_RCC_XTAL_8_19MHZ,
    XTAL_MHZ_10   = SYSCTL_RCC_XTAL_10MHZ,
    XTAL_MHZ_12   = SYSCTL_RCC_XTAL_12MHZ,
    XTAL_MHZ_12_2 = SYSCTL_RCC_XTAL_12_2MHZ,
    XTAL_MHZ_13_5 = SYSCTL_RCC_XTAL_13_5MHZ,
    XTAL_MHZ_14_3 = SYSCTL_RCC_XTAL_14_3MHZ,
    XTAL_MHZ_16   = SYSCTL_RCC_XTAL_16MHZ,
    XTAL_MHZ_16_3 = SYSCTL_RCC_XTAL_16_3MHZ,
    XTAL_MHZ_18   = SYSCTL_RCC_XTAL_18MHZ,
    XTAL_MHZ_20   = SYSCTL_RCC_XTAL_20MHZ,
    XTAL_MHZ_24   = SYSCTL_RCC_XTAL_24MHZ,
    XTAL_MHZ_25 = SYSCTL_RCC_XTAL_25MHZ,
} XTAL;

static void __PLL_setClockSpeedManual( uint32_t XTAL, byte SYSDIV2 );
static int __sysClkSpeedInMHz = SYS_CLK_SPEED_IN_MHZ;

void PLL_setClockSpeed( enum ClockSpeed clk )
{
    __sysClkSpeedInMHz = clk;
    byte sysDiv = (400 / clk) - 1;
    __PLL_setClockSpeedManual( XTAL_MHZ_16, sysDiv );
}

void __PLL_setClockSpeedManual( uint32_t XTAL, byte SYSDIV2 )
{
    /* 1- RCC2 - enable RCC2
     * 2- RCC2 - use PLL ( external oscillator ) or internall oscillator
     * 3- RCC  - clear XTAL in RCC
     * 4- RCC  - set XTAL to the specific oscillator speed (ex. 16 MHZ)
     * 5- choose the source of oscillation OSCSRC2
     * 6-
     */

    /* USERCC2 - clock configure 2 - activate RCC2
     * bit-31
     */
    SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;

    /* BYPASS2 - multiplexer 2x1 switcher
    * 0 -> use PLL
    * 1 -> use internall oscillator
    * bit-11
    */
    SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;

    /* specify the crystal frequency in the four XTAL bits
    * bits 10-6
    * XTAL is in RCC not RCC2
    * first : clear the required bits
    * second: set XTALL
    */
    SYSCTL_RCC_R = ( SYSCTL_RCC_R & ~SYSCTL_RCC_XTAL_M ) | XTAL;

    /* OSCSRC2 - multiplexer 4x1 switcher
     * choose the main oscillator
     * bits 6-4
     * 00  -> main oscillator
     * 01  -> intenal oscillator
     * 10  -> intenal oscillator / 4
     * 11  -> low freq internal oscillator
     * 111 -> activate 32.768 KHz internall or external oscillator
     */
     SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~SYSCTL_RCC2_OSCSRC2_M) + SYSCTL_RCC2_OSCSRC2_MO;

    /* PWRDN2 - multiplexer 2x1 switcher
     * activate PLL
     * bit-13
     * 1 -> PLL is off
     * 0 -> PLL is on
     */
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;

    SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;

    /* SYSDIV2 - System Clock Divisor
     * 400 MHz / (SYSDIV2 + 1)
     * bits 28-22
     * NOTE: LSB -> least signficent bit
     * first  : clear the required bits
     * second : set SYSDIV2
     */
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + ( SYSDIV2 << 22 );


    /* PLLRIS register - Raw Interrupt Status -
     * wait for PLL to stabilize
     * bit-6
     * 1 -> means that PLL is stabilized and is ready to use
     */
    while( (SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) == 0x0 );

    /* BYPASS2 - multiplexer 2x1 switcher
     * 0 -> use PLL
     * 1 -> use internall oscillator
     * bit-11
     */
    SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
}

ClockSpeed PLL_getClockSpeed()
{
    return __sysClkSpeedInMHz;
}
