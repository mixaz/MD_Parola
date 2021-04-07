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
 * \brief Implements horizontal scrolling effect
 */

#define START_POSITION (bLeft) ? ZONE_START_COL(z->_zoneStart) : ZONE_END_COL(z->_zoneEnd) ///< Start position depends on the scrolling direction

void MD_PZone_effectHScroll(MD_PZone_t *z,bool bLeft, bool bIn)
{
  if (bIn)
  {
    switch(z->_fsmState)
    {
    case INITIALISE:
    case GET_FIRST_CHAR: // Load the first character from the font table
      PRINT_STATE("I HSCROLL");

      if (!MD_PZone_getFirstChar(z,&z->_charCols))
      {
        z->_fsmState = END;
        break;
      }
      z->_countCols = 0;
      z->_fsmState = PUT_CHAR;
      break;

    case GET_NEXT_CHAR: // Load the next character from the font table
      PRINT_STATE("I HSCROLL");

      // Have we reached the end of the characters string?
      do
      {
        if (!MD_PZone_getNextChar(z,&z->_charCols))
          z->_fsmState = PAUSE;
      } while (z->_charCols == 0 && z->_fsmState != PAUSE);

      if (z->_fsmState == PAUSE) break;

      z->_countCols = 0;
      z->_fsmState = PUT_CHAR;
      FSMPRINTS(", fall through");
      // !! fall through to next state to start displaying

    case PUT_CHAR:  // display the next part of the character
      PRINT_STATE("I HSCROLL");

      if (z->_charCols != 0)
      {
        MD_MAX72XX_transform(z->_MX,z->_zoneStart, z->_zoneEnd, bLeft ? TSL : TSR);
        MD_MAX72XX_setColumn2(z->_MX,START_POSITION, DATA_BAR(z->_cBuf[z->_countCols++]));
        FSMPRINTS(", scroll");
      }

      // end of this buffer - we may need to get another one
      if (z->_countCols == z->_charCols)
      {
        if (!z->_endOfText)
          z->_fsmState = GET_NEXT_CHAR;
        else
        {
          // work out the number of filler columns
          z->_countCols = (bLeft ? z->_limitLeft - z->_textLen - ZONE_START_COL(z->_zoneStart) + 1 : ZONE_END_COL(z->_zoneEnd) - z->_limitLeft);
          FSMPRINT(", filler count ", _countCols);
          z->_fsmState = (z->_countCols <= 0) ? PAUSE : PUT_FILLER;
        }
      }
    break;

    case PUT_FILLER:    // keep sending out blank columns until aligned
      PRINT_STATE("I HSCROLL");

      MD_MAX72XX_transform(z->_MX,z->_zoneStart, z->_zoneEnd, bLeft ? TSL : TSR);
      MD_MAX72XX_setColumn2(z->_MX,START_POSITION, EMPTY_BAR);
      FSMPRINTS(", fill");

      if (--z->_countCols == 0)
          z->_fsmState = PAUSE;
    break;

    default:
      z->_fsmState = PAUSE;
    break;
    }
  }
  else  // exiting
  {
    switch(z->_fsmState)
    {
    case PAUSE:
      PRINT_STATE("O HSCROLL");
      z->_fsmState = PUT_FILLER;
      FSMPRINTS(" falling thru");
      // fall through

    case PUT_FILLER:
      PRINT_STATE("O HSCROLL");
      MD_MAX72XX_transform(z->_MX,z->_zoneStart, z->_zoneEnd, bLeft ? TSL : TSR);
      MD_MAX72XX_setColumn2(z->_MX,START_POSITION, EMPTY_BAR);

      // check if enough scrolled off to say that new message should start
      // how we count depends on the direction for scrolling
      {
        uint16_t  spaceCount = 0;
        uint16_t  maxCount = (z->_zoneEnd - z->_zoneStart + 1) * COL_SIZE;

        if ((z->_scrollDistance != 0) && (maxCount > z->_scrollDistance)) maxCount = z->_scrollDistance;

        if (bLeft)
        {
          for (int16_t i = ZONE_START_COL(z->_zoneStart);
              (i <= ZONE_END_COL(z->_zoneEnd)) && (MD_MAX72XX_getColumn1(z->_MX,i) == EMPTY_BAR);
               i++, spaceCount++);
        }
        else
        {
          for (int16_t i = ZONE_END_COL(z->_zoneEnd);
              (i >= ZONE_START_COL(z->_zoneStart)) && (MD_MAX72XX_getColumn1(z->_MX,i) == EMPTY_BAR);
              i--, spaceCount++);
        }

        if (maxCount <= spaceCount) z->_fsmState = END;  // enough of a space between messages, end this FSM
      }
      break;

    default:
        z->_fsmState = END;
      break;
    }
  }
}
