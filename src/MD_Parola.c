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
#include <string.h>

#include "MD_Parola.h"
#include "MD_Parola_lib.h"

/**
 * \file
 * \brief Implements core MD_Parola class methods
 */

void MD_Parola_destructor(MD_Parola_t *p)
{
#if !STATIC_ZONES
  // release the dynamically allocated zone array
  free(p->_Z);
#endif
}

void MD_Parola_begin(MD_Parola_t *p,uint8_t numZones)
{
  MD_MAX72XX_begin(&p->_D);

  // Check boundaries for the number of zones
  if (numZones == 0) numZones = 1;
#if STATIC_ZONES
  if (numZones > MAX_ZONES) numZones = MAX_ZONES;  // static zones
#endif
  p->_numZones = numZones;

#if !STATIC_ZONES
  // Create the zone objects array for dynamic zones
  _Z = malloc(sizeof(MD_PZone_t) * p->_numZones);
#endif

  for (uint8_t i = 0; i < p->_numZones; i++)
    MD_PZone_begin(&p->_Z[i],&p->_D);

  // for one zone automatically make it all modules, user will override if not intended
  if (p->_numZones == 1)
    MD_Parola_setZone(p,0, 0, p->_numModules - 1);

  // initialize zone-independent options
  MD_Parola_setSpeedInOut(p,10, 10);
  MD_Parola_setPause(p,10 * MD_Parola_getSpeed1(p));
  MD_Parola_setCharSpacing(p,1);
  MD_Parola_setScrollSpacing(p,0);
  MD_Parola_setTextAlignment(p,PA_LEFT);
  MD_Parola_setTextEffect(p,PA_PRINT, PA_NO_EFFECT);
  MD_Parola_setInvert(p,false);
}

bool MD_Parola_setZone(MD_Parola_t *p,uint8_t z, uint8_t moduleStart, uint8_t moduleEnd)
{
  if ((moduleStart <= moduleEnd) && (moduleEnd < p->_numModules) && (z < p->_numZones))
  {
    MD_PZone_setZone(&p->_Z[z],moduleStart, moduleEnd);
    return(true);
  }

  return(false);
}

void MD_Parola_displayZoneText(MD_Parola_t *p,uint8_t z, const char *pText, enum textPosition_t align, uint16_t speed, uint16_t pause, enum textEffect_t effectIn, enum textEffect_t effectOut)
{
  MD_Parola_setTextBuffer(p,z, pText);
  MD_Parola_setTextAlignment2(p,z, align);
  MD_Parola_setSpeed2(p,z, speed);
  MD_Parola_setPause2(p,z, pause);
  MD_Parola_setTextEffect2(p,z, effectIn, effectOut);

  MD_Parola_displayReset1(p,z);
}

bool MD_Parola_displayAnimate(MD_Parola_t *p)
{
  bool b = false;

  // suspend the display while we animate a frame
  MD_MAX72XX_update(&p->_D,OFF);

  for (uint8_t i = 0; i < p->_numZones; i++)
    b |= MD_PZone_zoneAnimate(&p->_Z[i]);

  // re-enable and update the display
  MD_MAX72XX_update(&p->_D,ON);

  return(b);
}


size_t MD_Parola_write1(MD_Parola_t *p,const char *str)
// .print() extension of a string
{
  MD_Parola_displayText(p,(char *)str, MD_Parola_getTextAlignment1(p), 0, 0, PA_PRINT, PA_NO_EFFECT);
  while (MD_Parola_displayAnimate(p))
    /* do nothing */;

  return(strlen(str));
}

size_t MD_Parola_write2(MD_Parola_t *p,const uint8_t *buffer, size_t size)
// .print() extension
{
  char *psz = (char *)malloc(sizeof(char) * (size + 1));

  if (psz == NULL) return(0);

  memcpy(psz, buffer, size);
  psz[size] = '\0';
  MD_Parola_write1(p,psz);
  free(psz);

  return(size);
}
