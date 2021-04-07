#include <stdlib.h>
#include <string.h>

#include "MD_Parola.h"
#include "MD_Parola_lib.h"

void MD_PZone_zoneClear(MD_PZone_t *z) {
    MD_MAX72XX_clear(z->_MX,z->_zoneStart, z->_zoneEnd);
    if (z->_inverted)
        MD_MAX72XX_transform(z->_MX,z->_zoneStart, z->_zoneEnd, TINV);
}

void MD_PZone_setIntensity(MD_PZone_t *z,uint8_t intensity) {
    z->_intensity = intensity; MD_MAX72XX_control2(z->_MX,z->_zoneStart, z->_zoneEnd, INTENSITY, z->_intensity);
}

void MD_Parola_constructor2(MD_Parola_t *p,enum moduleType_t mod, uint8_t csPin, uint8_t numDevices) {
    MD_MAX72XX_constructor2(&p->_D,mod, csPin, numDevices);
    p->_numModules = numDevices;
}

void MD_Parola_begin1(MD_Parola_t *p) {
    MD_Parola_begin(p,1);
};

void MD_Parola_displayText(MD_Parola_t *p,const char *pText, enum textPosition_t align, uint16_t speed, uint16_t pause,
        enum textEffect_t effectIn, enum textEffect_t effectOut) {
    MD_Parola_displayZoneText(p,0, pText, align, speed, pause, effectIn, effectOut);
}

void MD_Parola_setSpeedInOut(MD_Parola_t *p,uint16_t speedIn, uint16_t speedOut) {
    for (uint8_t i = 0; i < p->_numZones; i++)
        MD_PZone_setSpeedInOut(&p->_Z[i],speedIn, speedOut);
}

void MD_Parola_setPause(MD_Parola_t *p,uint16_t pause) {
    for (uint8_t i = 0; i < p->_numZones; i++)
        MD_PZone_setPause(&p->_Z[i],pause);
}

void MD_Parola_setCharSpacing(MD_Parola_t *p,uint8_t cs) {
    for (uint8_t i = 0; i < p->_numZones; i++)
        MD_PZone_setCharSpacing(&p->_Z[i],cs);
}

void MD_Parola_setScrollSpacing(MD_Parola_t *p,uint16_t space) {
    for (uint8_t i = 0; i < p->_numZones; i++)
        MD_PZone_setScrollSpacing(&p->_Z[i],space);
}

void MD_Parola_setTextAlignment(MD_Parola_t *p,enum textPosition_t ta) {
    for (uint8_t i = 0; i < p->_numZones; i++)
        MD_PZone_setTextAlignment(&p->_Z[i],ta);
}

void MD_Parola_setTextEffect(MD_Parola_t *p,enum textEffect_t effectIn, enum textEffect_t effectOut) {
    for (uint8_t i = 0; i < p->_numZones; i++)
        MD_PZone_setTextEffect(&p->_Z[i],effectIn, effectOut);
}

void MD_Parola_setTextEffect2(MD_Parola_t *p,uint8_t z, enum textEffect_t effectIn, enum textEffect_t effectOut) {
    if (z < p->_numZones)
        MD_PZone_setTextEffect(&p->_Z[z],effectIn, effectOut);
}

void MD_Parola_setInvert(MD_Parola_t *p,uint8_t invert) {
    for (uint8_t i = 0; i < p->_numZones; i++)
        MD_PZone_setInvert(&p->_Z[i],invert);
}

void MD_Parola_setSpeed2(MD_Parola_t *p,uint8_t z, uint16_t speed) {
    if (z < p->_numZones)
        MD_PZone_setSpeed(&p->_Z[z],speed);
}

void MD_Parola_displayReset1(MD_Parola_t *p,uint8_t z) {
    if (z < p->_numZones)
        MD_PZone_zoneReset(&p->_Z[z]);
}
