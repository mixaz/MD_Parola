/*
MD_Parola - Library for modular scrolling text and Effects

See header file for comments

Copyright (C) 2018 Marco Colli. All rights reserved.

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
 * \brief Implements various sprite effects
 */

#if ENA_SPRITE

void MD_PZone_effectSprite(MD_PZone_t *z,bool bIn, uint8_t id)
// Animated Pacman sprite leads or eats up the message.
// Print up the whole message and then remove the parts we
// don't need in order to do the animation.
{
  if (bIn)  // incoming - sprite moves left to right in the zone
  {
    switch (z->_fsmState)
    {
    case INITIALISE:
      PRINT_STATE("I SPRITE");
      MD_PZone_setInitialEffectConditions(z);
      if (z->_startPos < z->_endPos)
      {
        int16_t t = z->_startPos;
        z->_startPos = z->_endPos;
        z->_endPos = t;
      }
      if (z->_spriteInData == NULL)
      {
        z->_fsmState = END;
        break;
      }
      z->_posOffset = 0;   // current animation frame for the sprite
      z->_nextPos = ZONE_END_COL(z->_zoneEnd) + 1;
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    case PAUSE:
      PRINT_STATE("I SPRITE");

      MD_PZone_commonPrint(z);

      // move reference column and draw new graphic
      z->_nextPos--;
      for (uint8_t i = 0; i < z->_spriteInWidth; i++)
      {
        if ((z->_nextPos + i) <= ZONE_END_COL(z->_zoneEnd) && (z->_nextPos + i) >= ZONE_START_COL(z->_zoneStart))
          MD_MAX72XX_setColumn2(z->_MX,z->_nextPos + i, DATA_BAR(pgm_read_byte(z->_spriteInData + (z->_posOffset * z->_spriteInWidth) + i)));
      }

      // blank out the part of the display we don't need
      // this is the part to the right of the sprite
      for (int16_t i = z->_nextPos - 1; i >= z->_endPos; i--)
        MD_MAX72XX_setColumn2(z->_MX,i, EMPTY_BAR);

      // advance the animation frame
      z->_posOffset++;
      if (z->_posOffset >= z->_spriteInFrames)
          z->_posOffset = 0;

      // check if we have finished
      if (z->_nextPos == ZONE_START_COL(z->_zoneStart) - z->_spriteInWidth - 1)
          z->_fsmState = PAUSE;
      break;

    default:
      PRINT_STATE("I SPRITE");
      z->_fsmState = PAUSE;
    }
  }
  else  // exiting - sprite moves left to right in the zone
  {
    switch (z->_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      PRINT_STATE("O SPRITE");
      MD_PZone_setInitialEffectConditions(z);
      if (z->_startPos < z->_endPos)
      {
        int16_t t = z->_startPos;
        z->_startPos = z->_endPos;
        z->_endPos = t;
      }
      if (z->_spriteOutData == NULL)
      {
        z->_fsmState = END;
        break;
      }
      z->_nextPos = ZONE_START_COL(z->_zoneStart) - 1;
      z->_posOffset = 0;
      z->_fsmState = PUT_CHAR;
      // fall through to next state

    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      PRINT_STATE("O SPRITE");
      MD_PZone_commonPrint(z);

      // move reference column and draw new graphic
      z->_nextPos++;
      for (uint8_t i = 0; i < z->_spriteOutWidth; i++)
      {
        if ((z->_nextPos - i) <= ZONE_END_COL(z->_zoneEnd) && (z->_nextPos - i) >= ZONE_START_COL(z->_zoneStart))
          MD_MAX72XX_setColumn2(z->_MX,z->_nextPos - i, DATA_BAR(pgm_read_byte(z->_spriteOutData + (z->_posOffset * z->_spriteOutWidth) + i)));
      }

      // blank out the part of the display we don't need
      // this is the part to the right of the sprite
      for (int16_t i = z->_nextPos - z->_spriteOutWidth; i >= z->_endPos; i--)
        MD_MAX72XX_setColumn2(z->_MX,i, EMPTY_BAR);

      // advance the animation frame
      z->_posOffset++;
      if (z->_posOffset >= z->_spriteOutFrames)
          z->_posOffset = 0;

      // check if we have finished
      if (z->_nextPos == ZONE_END_COL(z->_zoneEnd) + z->_spriteOutWidth + 1)
          z->_fsmState = END;
      break;

    default:
      PRINT_STATE("O SPRITE");
      z->_fsmState = END;
      break;
    }
  }
}

#endif