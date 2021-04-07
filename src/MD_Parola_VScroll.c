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
 * \brief Implements vertical scroll effect
 */

void MD_PZone_effectVScroll(MD_PZone_t *z,bool bUp, bool bIn)
// Scroll the display horizontally up or down, depending on the selected effect
{
  if (bIn)  // incoming
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I VSCROLL");
      z->_nextPos = COL_SIZE-1;
      MD_MAX72XX_control2(z->_MX,z->_zoneStart, z->_zoneEnd, WRAPAROUND, OFF);
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I VSCROLL");

      MD_PZone_zoneClear(z);
      MD_PZone_commonPrint(z);

      // scroll each column of the zone so that the message appears to be animated
      // Note: Directions are reversed here because we start with the message in the
      // middle position thru commonPrint() and to see it animated moving DOWN we
      // need to scroll it UP, and vice versa.
      for (int16_t j = ZONE_START_COL(z->_zoneStart); j <= ZONE_END_COL(z->_zoneEnd); j++)   // for each column
      {
        uint8_t c = MD_MAX72XX_getColumn1(z->_MX,j);

        for (int8_t i = z->_nextPos; i > 0; i--)
        {
          c = (bUp ? c << 1 : c >> 1);
          if (z->_inverted) c |= (bUp ? 0x01 : 0x80);
        }

        MD_MAX72XX_setColumn2(z->_MX,j, c);
      }

      // check if we have finished
      if (z->_nextPos == 0)
          z->_fsmState = PAUSE;

      z->_nextPos--;
      break;

    default:
      PRINT_STATE("I VSCROLL");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (z->_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O VSCROLL");
      z->_nextPos = 0;
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O VSCROLL");

      for (int16_t j = ZONE_START_COL(z->_zoneStart); j <= ZONE_END_COL(z->_zoneEnd); j++)   // for each column
      {
        uint8_t c = MD_MAX72XX_getColumn1(z->_MX,j);

        c = (bUp ? c >> 1 : c << 1);
        if (z->_inverted) c |= (bUp ? 0x80 : 0x01);

        MD_MAX72XX_setColumn2(z->_MX,j, c);
      }

      // check if we have finished
      if (z->_nextPos == COL_SIZE - 1)
          z->_fsmState = END;

      z->_nextPos++;
      break;

    default:
      PRINT_STATE("O VSCROLL");
      z->_fsmState = END;
      break;
    }
  }
}
