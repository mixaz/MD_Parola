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
 * \brief Implements wipe effect
 */

#if ENA_WIPE

void MD_PZone_effectWipe(MD_PZone_t *z,bool bLightBar, bool bIn)
// Wipe the message over with a new one
// Print up the whole message and then remove the parts we
// don't need in order to do the animation.
{
  if (bIn)  // incoming
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I WIPE");
      MD_PZone_setInitialEffectConditions(z);
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I WIPE");
      if (z->_fsmState == PAUSE)
        z->_fsmState = PUT_CHAR;

      MD_PZone_commonPrint(z);

      // blank out the part of the display we don't need
      FSMPRINT(" - Clear ", _nextPos);
      FSMPRINT(" to ", _endPos);
      FSMPRINT(" step ", _posOffset);
      for (int16_t i = z->_nextPos; i != z->_endPos + z->_posOffset; i += z->_posOffset)
        MD_MAX72XX_setColumn2(z->_MX,i, EMPTY_BAR);

      if (bLightBar && (z->_nextPos != z->_endPos + z->_posOffset)) MD_MAX72XX_setColumn2(z->_MX,z->_nextPos, LIGHT_BAR);

      // check if we have finished
      if (z->_nextPos == z->_endPos + z->_posOffset) z->_fsmState = PAUSE;

      z->_nextPos += z->_posOffset; // for the next time around
      break;

    default:
      PRINT_STATE("I WIPE");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (z->_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O WIPE");
      MD_PZone_setInitialEffectConditions(z);
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O WIPE");
      MD_PZone_commonPrint(z);

      // blank out the part of the display we don't need
      FSMPRINT(" - Clear ", _nextPos);
      FSMPRINT(" to ", _endPos);
      FSMPRINT(" step ", _posOffset);
      for (int16_t i = z->_startPos; i != z->_nextPos + z->_posOffset; i += z->_posOffset)
        MD_MAX72XX_setColumn2(z->_MX,i, EMPTY_BAR);

      if (bLightBar && (z->_nextPos != z->_endPos + z->_posOffset)) MD_MAX72XX_setColumn2(z->_MX,z->_nextPos, LIGHT_BAR);

      // check if we have finished
      if (z->_nextPos == z->_endPos + z->_posOffset) z->_fsmState = END;

      z->_nextPos += z->_posOffset; // for the next time around
      break;

    default:
      PRINT_STATE("O WIPE");
      z->_fsmState = END;
      break;
    }
  }
}

#endif