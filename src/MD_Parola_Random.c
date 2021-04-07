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
#include <stdlib.h>

#include <MD_Parola.h>
#include <MD_Parola_lib.h>

#define random1(X)      (random()%X)

/**
 * \file
 * \brief Implements random effect
 */

#if ENA_MISC

#define RAND_CYCLE 11  // random effect repeats every RANDOM_CYCLE columns

void MD_PZone_effectRandom(MD_PZone_t *z,bool bIn)
// Text enters as random dots
{
  static uint8_t pix[RAND_CYCLE];  // pixel data (one byte per column) to mask the display
  uint8_t c, r;   // the row and column coordinates being considered

switch (z->_fsmState)
{
  case INITIALISE:    // Entry bIn == true
  case PAUSE:         // Exit  bIn == false
    PRINT_STATE("IO RAND");
    for (uint8_t i = 0; i < RAND_CYCLE; i++)
      pix[i] = 0;

    z->_fsmState = PUT_CHAR;
    // fall through to next state

  case GET_FIRST_CHAR:
  case GET_NEXT_CHAR:
  case PUT_CHAR:
    PRINT_STATE("IO RAND");
    if (bIn) FSMPRINTS(" in"); else FSMPRINTS(" out");

    // Work out and set the next random pixel in the column mask.
    // Use a loop counter to make sure we just don't loop forever.
    z->_nextPos = 0;
    do
    {
      c = random1(RAND_CYCLE);
      r = random1(ROW_SIZE);
      z->_nextPos++;
    } while (pix[c] & (1 << r) && z->_nextPos < 5000);

    // FSMPRINT("\n [r,c]=", r); FSMPRINT(",", c); FSMPRINT(" counter ", _nextPos);

    pix[c] |= (1 << r); // set the r,c location in the mask

    // set up a new display
    MD_PZone_commonPrint(z);

    // now mask each column by the pixel mask - this repeats every RAND_CYCLE columns, but the
    // characters don't occupy every pixel so the effect looks 'random' across the whole display.
    z->_nextPos = 0;
    for (uint8_t cc = ZONE_START_COL(z->_zoneStart); cc <= ZONE_END_COL(z->_zoneEnd); cc++)
    {
      uint8_t col = MD_MAX72XX_getColumn1(z->_MX,cc);

      col &= (bIn ? pix[z->_nextPos] : ~pix[z->_nextPos]);  // set or reset the bit (depends on bIn)
      MD_MAX72XX_setColumn2(z->_MX,cc, col);

      z->_nextPos++;
      if (z->_nextPos == RAND_CYCLE)
        z->_nextPos = 0;
    }

    // check if we have finished. This is when all the columns have pixels all on (0xff)
    {
      bool bEnd = true;

      for (uint8_t i = 0; bEnd && i < COL_SIZE; i++)
        bEnd = (pix[i] == 0xff);

      if (bEnd) z->_fsmState = (bIn ? PAUSE : END);
      }
    break;

  default:
    PRINT_STATE("IO RAND");
    z->_fsmState = (bIn ? PAUSE : END);
  }
}

#endif