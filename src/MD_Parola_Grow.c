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
 * \brief Implements grow effects
 */

#if ENA_GROW

void MD_PZone_effectGrow(MD_PZone_t *z,bool bUp, bool bIn)
// Scan the message over with a new one
// Print up the whole message and then remove the parts we
// don't need in order to do the animation.
{
  if (bIn)  // incoming
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I GROW");
      MD_PZone_setInitialEffectConditions(z);
      z->_nextPos = (bUp ? 0xff : 1); // this is the bit mask
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I GROW");

      MD_PZone_commonPrint(z);
      // check if we have finished
      if (z->_nextPos == (bUp ? 0 : 0xff)) // all bits covered
      {
        z->_fsmState = PAUSE;
        break;
      }

      // blank out the part of the display we don't need
      FSMPRINT("Keep bits ", _nextPos);
      for (int16_t i = z->_startPos; i != z->_endPos + z->_posOffset; i += z->_posOffset)
      {
        uint8_t c = DATA_BAR(MD_MAX72XX_getColumn1(z->_MX,i)) & (bUp ? ~z->_nextPos : z->_nextPos);

        MD_MAX72XX_setColumn2(z->_MX,i, DATA_BAR(c));
      }

      // for the next time around
      if (bUp)
        z->_nextPos >>= 1;
      else
        z->_nextPos = (z->_nextPos << 1) | 1;
      break;

    default:
      PRINT_STATE("I GROW");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (z->_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O GROW");
      MD_PZone_setInitialEffectConditions(z);
      z->_nextPos = (bUp ? 1 : 0xff);  // this is the bit mask
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O GROW");
      MD_PZone_commonPrint(z);

      // blank out the part of the display we don't need
      FSMPRINT(" Keep bits ", _nextPos);
      for (int16_t i =z->_startPos; i != z->_endPos + z->_posOffset; i += z->_posOffset)
      {
        uint8_t c = DATA_BAR(MD_MAX72XX_getColumn1(z->_MX,i)) & (bUp ? ~z->_nextPos : z->_nextPos);

        MD_MAX72XX_setColumn2(z->_MX,i, DATA_BAR(c));
      }

      // check if we have finished
      if (z->_nextPos == (bUp ? 0xff : 0x0)) // all bits covered
        z->_fsmState = END;

      // for the next time around
      if (bUp)
        z->_nextPos = (z->_nextPos << 1) | 1;
      else
        z->_nextPos >>= 1;
      break;

    default:
      PRINT_STATE("O GROW");
      z->_fsmState = END;
      break;
    }
  }
}

#endif