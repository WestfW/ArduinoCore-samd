/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif
void pinMode( uint32_t ulPin, uint32_t ulMode )
{
  PortGroup *pPort = digitalPinToPort(ulPin);

  // Handle the case the pin isn't usable as PIO
  if ( pPort == NOT_A_PORT )
  {
    return ;
  }

  uint32_t pin = g_APinDescription[ulPin].ulPin;
  uint32_t pinMask = (1ul << pin);

  // enable continuous pin scanning on all pins, so that we can read from
  // the high speed PORT_IOBUS without getting stale data.
  pPort->CTRL.reg |= pinMask;

  // Set pin mode according to chapter '22.6.3 I/O Pin Configuration'
  switch ( ulMode )
  {
    case INPUT:
      // Set pin to input mode
      pPort->PINCFG[pin].reg=(uint8_t)(PORT_PINCFG_INEN) ;
      pPort->DIRCLR.reg = pinMask;
    break ;

    case INPUT_PULLUP:
      // Set pin to input mode with pull-up resistor enabled
      pPort->PINCFG[pin].reg=(uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ;
      pPort->DIRCLR.reg = pinMask;

      // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.7 Data Output Value Set')
      pPort->OUTSET.reg = pinMask;
    break ;

    case INPUT_PULLDOWN:
      // Set pin to input mode with pull-down resistor enabled
      pPort->PINCFG[pin].reg=(uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ;
      pPort->DIRCLR.reg = pinMask;

      // Enable pull level (cf '22.6.3.2 Input Configuration' and '22.8.6 Data Output Value Clear')
      pPort->OUTCLR.reg = pinMask;
    break ;

    case OUTPUT:
      // enable input, to support reading back values, with pullups disabled
      pPort->PINCFG[pin].reg=(uint8_t)(PORT_PINCFG_INEN) ;

      // Set pin to output mode
      pPort->DIRSET.reg = pinMask;
    break ;

    default:
      // do nothing
    break ;
  }
}

void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
  PortGroup *pPort = digitalPinToPort(ulPin);

  // Handle the case the pin isn't usable as PIO
  if ( pPort == NOT_A_PORT )
  {
    return ;
  }

  uint32_t pin = g_APinDescription[ulPin].ulPin;
  uint32_t pinMask = (1ul << pin);

  if ( (pPort->DIRSET.reg & pinMask) == 0 ) {
    // the pin is not an output, disable pull-up if val is LOW, otherwise enable pull-up
    pPort->PINCFG[pin].bit.PULLEN = ((ulVal == LOW) ? 0 : 1) ;
  }
  
  switch ( ulVal )
  {
    case LOW:
      pPort->OUTCLR.reg = pinMask;
    break ;

    default:
      pPort->OUTSET.reg = pinMask;
    break ;
  }

  return ;
}

int digitalRead( uint32_t ulPin )
{
  // Handle the case the pin isn't usable as PIO
  
  PortGroup *pPort = digitalPinToPort(ulPin);

  if ( pPort == NOT_A_PORT )
  {
    return LOW ;
  }

  if ( (pPort->IN.reg & (1ul << g_APinDescription[ulPin].ulPin)) != 0 )
  {
    return HIGH ;
  }

  return LOW ;
}

#ifdef __cplusplus
}
#endif

