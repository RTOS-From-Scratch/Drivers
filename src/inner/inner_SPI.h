#ifndef INNER_SPI_H_
#define INNER_SPI_H_

#define __SPI0_CLK            A2
#define __SPI1_PORTF_CLK      F2
#define __SPI1_PORTD_CLK      D0
#define __SPI2_CLK            B4
#define __SPI3_CLK            D0

// this is the configuration of the clk rate at `SCR` at `SSICR0` register
#define __SPI_SPEED_CLK_RATE            8
#define __SPI_SPEED_FULL_CLK_RATE       __SPI_SPEED_CLK_RATE << 0
#define __SPI_SPEED_HALF_CLK_RATE       __SPI_SPEED_CLK_RATE << 1
#define __SPI_SPEED_QUARTER_CLK_RATE    __SPI_SPEED_CLK_RATE << 3
#define __SPI_SPEED_EIGHTH_CLK_RATE     __SPI_SPEED_CLK_RATE << 7

#define __SPI_SPEED_CONST_CLK_PRESCALE  2

#endif // INNER_SPI_H_
