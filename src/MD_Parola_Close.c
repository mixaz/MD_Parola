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
 * \brief Implements closing effect
 */

#if ENA_OPNCLS

void MD_PZone_effectClose(MD_PZone_t *z,bool bLightBar, bool bIn)
// Dissolve the current message in/out
{
  if (bIn)
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
      PRINT_STATE("I CLOSE");
      z->_nextPos = 0;
      MD_PZone_zoneClear(z);
      if (bLightBar)
      {
        MD_MAX72XX_setColumn2(z->_MX,z->_limitLeft, LIGHT_BAR);
        MD_MAX72XX_setColumn2(z->_MX,z->_limitRight,LIGHT_BAR);
      }
      z->_fsmState = PUT_CHAR;
      // fall through

    case PUT_CHAR:
      PRINT_STATE("I CLOSE");
      FSMPRINT(" - offset ", _nextPos);
      MD_PZone_zoneClear(z);
      MD_PZone_commonPrint(z);
      {
        const int16_t halfWidth = (z->_limitLeft - z->_limitRight) / 2;

        if (z->_nextPos > halfWidth)
        {
          z->_fsmState = PAUSE;
        }
        else
        {
          for (int16_t i = z->_limitRight + z->_nextPos + 1; i < z->_limitLeft - z->_nextPos; i++)
            MD_MAX72XX_setColumn2(z->_MX,i, EMPTY_BAR);

          z->_nextPos++;
          if (bLightBar && (z->_nextPos <= halfWidth))
          {
            MD_MAX72XX_setColumn2(z->_MX,z->_limitLeft - z->_nextPos, LIGHT_BAR);
            MD_MAX72XX_setColumn2(z->_MX,z->_limitRight + z->_nextPos, LIGHT_BAR);
          }
        }
      }
      break;

    default:
      PRINT_STATE("I CLOSE");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting
  {
    switch (z->_fsmState)
    {
    case PAUSE:

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
      PRINT_STATE("O CLOSE");
      FSMPRINT(" - limits R:", _limitRight);
      FSMPRINT(" L:", _limitLeft);
      z->_nextPos = (z->_limitLeft - z->_limitRight) / 2;
      FSMPRINT(" O:", _nextPos);
      MD_PZone_zoneClear(z);
      MD_PZone_commonPrint(z);
      if (bLightBar)
      {
        MD_MAX72XX_setColumn2(z->_MX,z->_limitLeft - z->_nextPos, LIGHT_BAR);
        MD_MAX72XX_setColumn2(z->_MX,z->_limitRight + z->_nextPos, LIGHT_BAR);
      }
      z->_fsmState = PUT_CHAR;
      break;

    case PUT_CHAR:
      PRINT_STATE("O CLOSE");
      FSMPRINT(" - offset ", _nextPos);
      if (z->_nextPos < 0)
      {
        z->_fsmState = END;
      }
      else
      {
        MD_MAX72XX_setColumn2(z->_MX,z->_limitLeft - z->_nextPos, EMPTY_BAR);
        MD_MAX72XX_setColumn2(z->_MX,z->_limitRight + z->_nextPos, EMPTY_BAR);

        z->_nextPos--;
        if (bLightBar && (z->_nextPos >= 0))
        {
          MD_MAX72XX_setColumn2(z->_MX,z->_limitLeft - z->_nextPos, LIGHT_BAR);
          MD_MAX72XX_setColumn2(z->_MX,z->_limitRight + z->_nextPos, LIGHT_BAR);
        }
      }
      break;

    default:
      PRINT_STATE("O CLOSE");
      z->_fsmState = END;
    }
  }
}

#endif