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

#include "PCTL.h"

void PCTL_set( uint32_t PCTL_module_encode, PORTS port )
{
    REG_VALUE(__PORTS_ADDR[port] + __IO_PCTL) |= PCTL_module_encode;
}

void PCTL_clear( uint32_t PCTL_module_encode, PORTS port )
{
    REG_VALUE(__PORTS_ADDR[port] + __IO_PCTL) &= ~(PCTL_module_encode);
}
