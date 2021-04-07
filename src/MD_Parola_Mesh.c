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
 * \brief Implements mesh effect
 */

#if ENA_MISC

void MD_PZone_effectMesh(MD_PZone_t *z,bool bIn)
// Text enters with alternating up/down columns
{
  bool bUp = true;

  if (bIn)  // incoming
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I MESH");
      z->_nextPos = 0;
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I MESH");

      MD_PZone_zoneClear(z);
      MD_PZone_commonPrint(z);

      for (uint8_t c = ZONE_START_COL(z->_zoneStart); c <= ZONE_END_COL(z->_zoneEnd); c++)
      {
        // scroll the whole display so that the message appears to be animated
        // Note: Directions are reversed because we start with the message in the
        // middle position thru commonPrint() and to see it animated move DOWN we
        // need to scroll it UP, and vice versa.
        uint8_t col = MD_MAX72XX_getColumn1(z->_MX,c);

        col = (bUp ? col >> (COL_SIZE - 1 - z->_nextPos) : col << (COL_SIZE - 1 - z->_nextPos));
        MD_MAX72XX_setColumn2(z->_MX,c, col);
        bUp = !bUp;
      }

      // check if we have finished
      z->_nextPos++;
      if (z->_nextPos == COL_SIZE) z->_fsmState = PAUSE;
      break;

    default:
      PRINT_STATE("I MESH");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (z->_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O MESH");
      z->_nextPos = 1;
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O MESH");

      for (uint8_t c = ZONE_START_COL(z->_zoneStart); c <= ZONE_END_COL(z->_zoneEnd); c++)
      {
        uint8_t col = MD_MAX72XX_getColumn1(z->_MX,c);

        col = (bUp ? col << z->_nextPos : col >> z->_nextPos);
        MD_MAX72XX_setColumn2(z->_MX,c, col);
        bUp = !bUp;
      }

      // check if we have finished
      z->_nextPos++;
      if (z->_nextPos == COL_SIZE) z->_fsmState = END;
      break;

    default:
      PRINT_STATE("O MESH");
      z->_fsmState = END;
      break;
    }
  }
}

#endif