/*
MD_Parola - Library for modular scrolling text and Effects

See header file for comments

Copyright (C) 2016 Marco Colli. All rights reserved.

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
 * \brief Implements FADE effect
 */

#if ENA_MISC

void MD_PZone_effectFade(MD_PZone_t *z,bool bIn)
// Fade the display in and out.
// If the overall intensity is changed while the animation is running, the
// intensity at the start of the animation will be restored at the end, overriding
// any user code changes.
{
  if (bIn) // incoming
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I FADE");
      z->_nextPos = 0;
      z->_endPos = MD_PZone_getIntensity(z);

      MD_PZone_zoneClear(z);

      z->_fsmState = GET_FIRST_CHAR;
      break;

    case GET_FIRST_CHAR:
      FSMPRINT(" I:", _nextPos);
      FSMPRINT("/", _endPos);

      MD_PZone_setIntensity(z,z->_nextPos++);
      MD_PZone_commonPrint(z);
      z->_fsmState = PUT_CHAR;
      break;

    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I FADE");
      FSMPRINT(" I:", _nextPos);
      FSMPRINT("/", _endPos);

      // check if we have finished
      if (z->_nextPos > z->_endPos)
        z->_fsmState = PAUSE;
      else
        MD_PZone_setIntensity(z,z->_nextPos++);
      break;

    default:
      PRINT_STATE("I FADE");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (z->_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O FADE");
      z->_nextPos = z->_endPos = MD_PZone_getIntensity(z);

      FSMPRINT(" I:", _nextPos);
      FSMPRINT("/", _endPos);

      MD_PZone_setIntensity(z,z->_nextPos);
      MD_PZone_commonPrint(z);

      z->_fsmState = PUT_CHAR;
      break;

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O FADE");

      FSMPRINT(" I:", _nextPos);
      FSMPRINT("/", _endPos);

      // check if we have finished
      if (z->_nextPos < 0)
      {
        MD_PZone_setIntensity(z,z->_endPos);  // set to original conditions
        MD_PZone_zoneClear(z);            // display nothing - we are currently at 0
        z->_fsmState = END;
      }
      else
        MD_PZone_setIntensity(z,z->_nextPos--);
      break;

    default:
      PRINT_STATE("O FADE");
      z->_fsmState = END;
      break;
    }
  }
}

#endif