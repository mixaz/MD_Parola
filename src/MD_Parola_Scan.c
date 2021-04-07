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
 * \brief Implements scan effect
 */

#if ENA_SCAN

void MD_PZone_effectHScan(MD_PZone_t *z,bool bIn, bool bBlank)
// Scan the message end to end.
// if bBlank is true, a blank column scans the text. If false, a non-blank scans the text.
// Print up the whole message and then remove the parts we
// don't need in order to do the animation.
{
  if (bIn)  // incoming
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I SCANH");
      MD_PZone_setInitialEffectConditions(z);
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I SCANH");

      MD_PZone_commonPrint(z);
      // check if we have finished
      if (z->_nextPos == z->_endPos)
      {
        z->_fsmState = PAUSE;
        break;
      }

      // blank out the part of the display we don't need
      FSMPRINT("Scan col ", _nextPos);
      for (int16_t i = z->_startPos; i != z->_endPos + z->_posOffset; i += z->_posOffset)
      {
        if ((!bBlank && (i != z->_nextPos)) || (bBlank && (i == z->_nextPos)))
          MD_MAX72XX_setColumn2(z->_MX,i, EMPTY_BAR);
      }

      z->_nextPos += z->_posOffset; // for the next time around
      break;

    default:
      PRINT_STATE("I SCANH");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (z->_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O SCANH");
      MD_PZone_setInitialEffectConditions(z);
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O SCANH");
      MD_PZone_commonPrint(z);

      // blank out the part of the display we don't need
      FSMPRINT(" Scan col ", _nextPos);
      for (int16_t i = z->_startPos; i != z->_endPos + z->_posOffset; i += z->_posOffset)
      {
        if ((!bBlank && (i != z->_nextPos)) || (bBlank && (i == z->_nextPos)))
          MD_MAX72XX_setColumn2(z->_MX,i, EMPTY_BAR);
      }

      // check if we have finished
      if (z->_nextPos < z->_endPos) z->_fsmState = END;

      z->_nextPos += z->_posOffset; // for the next time around
      break;

    default:
      PRINT_STATE("O SCANH");
      z->_fsmState = END;
      break;
    }
  }
}

void MD_PZone_effectVScan(MD_PZone_t *z,bool bIn, bool bBlank)
// Scan the message over with a new one
// if bBlank is true, a blank column scans the text. If false, a non-blank scans the text.
// Print up the whole message and then remove the parts we
// don't need in order to do the animation.
{
  uint8_t maskCol = 0;

  if (bIn)  // incoming
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I SCANV");
      MD_PZone_setInitialEffectConditions(z);
      z->_nextPos = 0; // this is the bit number
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I SCANV");
      MD_PZone_commonPrint(z);

      // check if we have finished
      if (z->_nextPos == 8) // bits numbered 0 to 7
      {
        z->_fsmState = PAUSE;
        break;
      }

      // blank out the part of the display we don't need
      FSMPRINT("Keep bit ", _nextPos);
      maskCol = (1 << z->_nextPos);
      for (int16_t i = z->_startPos; i != z->_endPos + z->_posOffset; i += z->_posOffset)
      {
        uint8_t c = DATA_BAR(MD_MAX72XX_getColumn1(z->_MX,i) & (bBlank ? ~maskCol : maskCol));

        MD_MAX72XX_setColumn2(z->_MX,i, DATA_BAR(c));
      }

      z->_nextPos++; // for the next time around
      break;

    default:
      PRINT_STATE("I SCANV");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (z->_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O SCANV");
      MD_PZone_setInitialEffectConditions(z);
      z->_nextPos = 7; // the bit number
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O SCANV");

      MD_PZone_commonPrint(z);

      // blank out the part of the display we don't need
      FSMPRINT(" Keep bit ", _nextPos);
      if (z->_nextPos >= 0)
        maskCol = 1 << z->_nextPos;
      for (int16_t i = z->_startPos; i != z->_endPos + z->_posOffset; i += z->_posOffset)
      {
        uint8_t c = DATA_BAR(MD_MAX72XX_getColumn1(z->_MX,i) & (bBlank ? ~maskCol : maskCol));

        MD_MAX72XX_setColumn2(z->_MX,i, DATA_BAR(c));
      }

      // check if we have finished
      if (z->_nextPos < 0)
        z->_fsmState = END;

      z->_nextPos--; // for the next time around
      break;

    default:
      PRINT_STATE("O SCANV");
      z->_fsmState = END;
      break;
    }
  }
}

#endif