#/*
# * Description : PCTL Port control configuration
# * Usage       :
# * Author      : Baron Leonardo <mohamedayman.fcis@zoho.com>
# * License     : Copyright (C) 2016 RTOS From Scratch
# *
# *
# * This program is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# */

#ifndef PCTL_H_
#define PCTL_H_

#include <stdint.h>
#include "../IO.h"
#include "inner_IO.h"
#include "tm4c123gh6pm.h"

static const uint32_t PCTL_UART_Rx[] = {
    GPIO_PCTL_PA0_U0RX,         // U0
    GPIO_PCTL_PC4_U1RX,         // U1_PORTC
    GPIO_PCTL_PD6_U2RX,         // U2
    GPIO_PCTL_PC6_U3RX,         // U3
    GPIO_PCTL_PC4_U4RX,         // U4
    GPIO_PCTL_PE4_U5RX,         // U5
    GPIO_PCTL_PD4_U6RX,         // U6
    GPIO_PCTL_PE0_U7RX,         // U7
    GPIO_PCTL_PB0_U1RX,         // U1_PORTB
};

// set the Port Control encode
void PCTL_set( uint32_t PCTL_module_encode, PORTS port );
// clear the Port Control encode
void PCTL_clear( uint32_t PCTL_module_encode, PORTS port );

#endif // PCTL_H_
