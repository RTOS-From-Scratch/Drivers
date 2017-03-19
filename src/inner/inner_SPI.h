#ifndef INNER_SPI_H_
#define INNER_SPI_H_

// this is the configuration of the clk rate at `SCR` and `SSICR0` register
#define __SPI_SPEED_CLK_RATE            8
#define __SPI_SPEED_FULL_CLK_RATE       __SPI_SPEED_CLK_RATE << 0
#define __SPI_SPEED_HALF_CLK_RATE       __SPI_SPEED_CLK_RATE << 1
#define __SPI_SPEED_QUARTER_CLK_RATE    __SPI_SPEED_CLK_RATE << 3
#define __SPI_SPEED_EIGHTH_CLK_RATE     __SPI_SPEED_CLK_RATE << 7

#define __SPI_SPEED_CONST_CLK_PRESCALE  2

#endif // INNER_SPI_H_
