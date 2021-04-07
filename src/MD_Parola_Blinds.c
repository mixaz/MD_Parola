/*
MD_Parola - Library for modular scrolling text and Effects

See header file for comments

Copyright (C) 2013 Marco Colli. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <MD_Parola.h>
#include <MD_Parola_lib.h>
/**
 * \file
 * \brief Implements blinds effect
 */

#if ENA_MISC

const uint8_t BLINDS_SIZE = 4; ///< The width of the blinds in pixels

void MD_PZone_effectBlinds(MD_PZone_t *z,bool bIn)
// Transfer between messages with blinds effects
{
  switch (z->_fsmState)
  {
  case INITIALISE:  // bIn = true
  case PAUSE:       // bIn = false
    PRINT_STATE("IO BLIND");
    z->_nextPos = 0;
    z->_fsmState = GET_FIRST_CHAR;
    // fall through

  case GET_FIRST_CHAR:  // blinds closing
    PRINT_STATE("IO BLIND");

    z->_nextPos++;
    for (int16_t i = ZONE_START_COL(z->_zoneStart); i <= ZONE_END_COL(z->_zoneEnd); i++)
    {
      if (i % BLINDS_SIZE < z->_nextPos)
        MD_MAX72XX_setColumn2(z->_MX,i, LIGHT_BAR);
    }

    if (z->_nextPos == BLINDS_SIZE)
    {
      z->_nextPos = BLINDS_SIZE;
      z->_fsmState = GET_NEXT_CHAR;
    }
    break;

  case GET_NEXT_CHAR:   // blinds opening
    PRINT_STATE("IO BLIND");
    MD_PZone_zoneClear(z);
    if (bIn) MD_PZone_commonPrint(z); // only do this when putting the message up

    z->_nextPos--;
    for (int16_t i = ZONE_START_COL(z->_zoneStart); i <= ZONE_END_COL(z->_zoneEnd); i++)
    {
      if (i % BLINDS_SIZE < z->_nextPos)
        MD_MAX72XX_setColumn2(z->_MX,i, LIGHT_BAR);
    }

    if (z->_nextPos == 0)
      z->_fsmState = PUT_CHAR;
    break;

  case PUT_CHAR:
    PRINT_STATE("IO BLIND");
    MD_PZone_zoneClear(z);
    if (bIn) MD_PZone_commonPrint(z);
    z->_fsmState = (bIn ? PAUSE : END);
    break;

  default:
    PRINT_STATE("IO BLIND");
    z->_fsmState = (bIn ? PAUSE : END);
  }
}

#endif
