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
 * \brief Implements slice effect
 */

#if ENA_MISC

void MD_PZone_effectSlice(MD_PZone_t *z,bool bIn)
{
  if (bIn)
  {
    switch(z->_fsmState)
    {
    case INITIALISE:
    case GET_FIRST_CHAR:
      PRINT_STATE("I SLICE");

      if (!MD_PZone_getFirstChar(z,&z->_charCols))
      {
        z->_fsmState = END;
        break;
      }
      MD_PZone_zoneClear(z);
      z->_countCols = 0;
      z->_nextPos = ZONE_START_COL(z->_zoneStart);
      z->_endPos = z->_limitLeft;

      FSMPRINT(" - Start ", _nextPos);
      FSMPRINT(", End ", _endPos);

      z->_fsmState = PUT_CHAR;
      break;

    case GET_NEXT_CHAR: // Load the next character from the font table
      PRINT_STATE("I SLICE");
      // Have we reached the end of the characters string?
      do
      {
        if (!MD_PZone_getNextChar(z,&z->_charCols))
          z->_fsmState = PAUSE;
      } while (z->_charCols == 0 && z->_fsmState != PAUSE);

      if (z->_fsmState == PAUSE) break;

      z->_countCols = 0;
      z->_fsmState = PUT_CHAR;
      // !! fall through to next state to start displaying

    case PUT_CHAR:  // display the next part of the character
      PRINT_STATE("I SLICE");
      FSMPRINT(" - Next ", _endPos);
      FSMPRINT(", anim ", _nextPos);

      // if the text is too long for the zone, stop when we are at the last column of the zone
      if (z->_nextPos == z->_endPos)
      {
        MD_MAX72XX_setColumn2(z->_MX,z->_nextPos, DATA_BAR(z->_cBuf[z->_countCols]));
        z->_fsmState = PAUSE;
        break;
      }

      if (z->_cBuf[z->_countCols] == 0) // empty column ?
      {
        z->_nextPos = z->_endPos; // pretend we just animated it!
      }
      else  // something to animate
      {
        // clear the column and animate the next one
        if (z->_nextPos != z->_endPos) MD_MAX72XX_setColumn2(z->_MX,z->_nextPos, EMPTY_BAR);
        z->_nextPos++;
        MD_MAX72XX_setColumn2(z->_MX,z->_nextPos, DATA_BAR(z->_cBuf[z->_countCols]));
      }

      // set up for the next time
      if (z->_nextPos == z->_endPos)
      {
        z->_nextPos = ZONE_START_COL(z->_zoneStart);
        z->_countCols++;
        z->_endPos--;
      }
      if (z->_countCols == z->_charCols) z->_fsmState = GET_NEXT_CHAR;
      break;

    default:
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch(z->_fsmState)
    {
    case PAUSE:
      PRINT_STATE("O SLICE");
      z->_nextPos = z->_endPos = z->_limitLeft;
      z->_fsmState = PUT_CHAR;
      // fall through

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O SLICE");
      FSMPRINT(" - Next ", _endPos);
      FSMPRINT(", anim ", _nextPos);

      while(MD_MAX72XX_getColumn1(z->_MX,z->_nextPos) == EMPTY_BAR && z->_endPos >= z->_limitRight)
        z->_nextPos = z->_endPos--; // pretend we just animated it!

      if (z->_endPos + 1 < z->_limitRight)
          z->_fsmState = END;  //reached the end
      else
      {
        // Move the column over to the left and blank out previous position
        if (z->_nextPos < ZONE_END_COL(z->_zoneEnd))
            MD_MAX72XX_setColumn2(z->_MX,z->_nextPos + 1, MD_MAX72XX_getColumn1(z->_MX,z->_nextPos));
        MD_MAX72XX_setColumn2(z->_MX,z->_nextPos, EMPTY_BAR);
        z->_nextPos++;

        // set up for the next time
        if (z->_nextPos == ZONE_END_COL(z->_zoneEnd) + 1)
            z->_nextPos = z->_endPos--;
      }
      break;

    default:
        z->_fsmState = END;
    }
  }
}

#endif