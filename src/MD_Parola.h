#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "MD_MAX72xx.h"
/**
 * \file
 * \brief Main header file for the MD_Parola library
 */

// Granular selection of animations/functions to include in the library
// If an animation class is not used at all some memory savings can be made
// by excluding the animation code.
#ifndef ENA_MISC
#define ENA_MISC    1   ///< Enable miscellaneous animations
#endif
#ifndef ENA_WIPE
#define ENA_WIPE    1   ///< Enable wipe type animations
#endif
#ifndef ENA_SCAN
#define ENA_SCAN    1   ///< Enable scanning animations
#endif
#ifndef ENA_SCR_DIA
#define ENA_SCR_DIA 1   ///< Enable diagonal scrolling animation
#endif
#ifndef ENA_OPNCLS
#define ENA_OPNCLS  1   ///< Enable open and close scan effects
#endif
#ifndef ENA_GROW
#define ENA_GROW    1   ///< Enable grow effects
#endif
#ifndef ENA_SPRITE
#define ENA_SPRITE  1   ///< Enable sprite effects
#endif

// If function is not used at all, then some memory savings can be made
// by excluding associated code.
#ifndef ENA_GRAPHICS
#define ENA_GRAPHICS  1 ///< Enable graphics functionality
#endif

// Miscellaneous defines
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))  ///< Generic macro for obtaining number of elements of an array
#define STATIC_ZONES 1    ///< Developer testing flag for quickly flipping between static/dynamic zones

#if STATIC_ZONES
#ifndef MAX_ZONES
#define MAX_ZONES 4     ///< Maximum number of zones allowed. Change to allow more or less zones but uses RAM even if not used.
#endif
#endif

// Zone column calculations
#define ZONE_START_COL(m) ((m) * COL_SIZE)    ///< The first column of the first zone module
#define ZONE_END_COL(m)   ((((m) + 1) * COL_SIZE) - 1)///< The last column of the last zone module

//class MD_Parola;

/**
* Text alignment enumerated type specification.
*
* Used to define the display text alignment and to specify direction for
* scrolling and animations. In the situation where LEFT AND RIGHT are the only sensible
* options (eg, text scrolling direction), CENTER will behave the same as LEFT.
*/
enum textPosition_t
{
  PA_LEFT,    ///< The leftmost column for the first character will be on the left side of the display
  PA_CENTER,  ///< The text will be placed with equal number of blank display columns either side
  PA_RIGHT    ///< The rightmost column of the last character will be on the right side of the display
};

/**
* Text effects enumerated type specification.
*
* Used to define the effects to be used for the entry and exit of text in the display area.
*/
enum textEffect_t
{
  PA_NO_EFFECT,   ///< Used as a place filler, executes no operation
  PA_PRINT,       ///< Text just appears (printed)
  PA_SCROLL_UP,   ///< Text scrolls up through the display
  PA_SCROLL_DOWN, ///< Text scrolls down through the display
  PA_SCROLL_LEFT, ///< Text scrolls right to left on the display
  PA_SCROLL_RIGHT,///< Text scrolls left to right on the display
#if ENA_SPRITE
  PA_SPRITE,      ///< Text enters and exits using user defined sprite
#endif
#if ENA_MISC
  PA_SLICE,       ///< Text enters and exits a slice (column) at a time from the right
  PA_MESH,        ///< Text enters and exits in columns moving in alternate direction (U/D)
  PA_FADE,        ///< Text enters and exits by fading from/to 0 and intensity setting
  PA_DISSOLVE,    ///< Text dissolves from one display to another
  PA_BLINDS,      ///< Text is replaced behind vertical blinds
  PA_RANDOM,      ///< Text enters and exits as random dots
#endif //ENA_MISC
#if ENA_WIPE
  PA_WIPE,        ///< Text appears/disappears one column at a time, looks like it is wiped on and off
  PA_WIPE_CURSOR, ///< WIPE with a light bar ahead of the change
#endif  // ENA_WIPES
#if ENA_SCAN
  PA_SCAN_HORIZ,  ///< Scan the LED column one at a time then appears/disappear at end
  PA_SCAN_HORIZX, ///< Scan a blank column through the text one column at a time then appears/disappear at end
  PA_SCAN_VERT,   ///< Scan the LED row one at a time then appears/disappear at end
  PA_SCAN_VERTX,  ///< Scan a blank row through the text one row at a time then appears/disappear at end
#endif // ENA_SCAN
#if ENA_OPNCLS
  PA_OPENING,     ///< Appear and disappear from the center of the display, towards the ends
  PA_OPENING_CURSOR,  ///< OPENING with light bars ahead of the change
  PA_CLOSING,     ///< Appear and disappear from the ends of the display, towards the middle
  PA_CLOSING_CURSOR,  ///< CLOSING with light bars ahead of the change
#endif // ENA_OPNCLS
#if ENA_SCR_DIA
  PA_SCROLL_UP_LEFT,  ///< Text moves in/out in a diagonal path up and left (North East)
  PA_SCROLL_UP_RIGHT, ///< Text moves in/out in a diagonal path up and right (North West)
  PA_SCROLL_DOWN_LEFT,  ///< Text moves in/out in a diagonal path down and left (South East)
  PA_SCROLL_DOWN_RIGHT, ///< Text moves in/out in a diagonal path down and right (North West)
#endif // ENA_SCR_DIA
#if ENA_GROW
  PA_GROW_UP,     ///< Text grows from the bottom up and shrinks from the top down
  PA_GROW_DOWN,   ///< Text grows from the top down and and shrinks from the bottom up
#endif // ENA_GROW
};

/**
 * Zone effect enumerated type specification.
 *
 * Used to define the effects to be used for text in the zone.
 *
 * The FLIP_UD and FLIP_LR effects are specifically designed to allow rectangular shaped display
 * modules (like Parola or Generic types) to be placed in an inverted position to allow all matrices
 * to be tightly packed into a 2 line display. One of the lines must be flipped horizontally and
 * vertically to remain legible in this configuration.
 */
enum zoneEffect_t
{
  PA_FLIP_UD, ///< Flip the zone Up to Down (effectively upside down). Works with all textEffect_t values
  PA_FLIP_LR, ///< Flip the zone Left to Right (effectively mirrored). Does not work with textEffect_t types SLICE, SCROLL_LEFT, SCROLL_RIGHT
};

/***
 *  Finite State machine states enumerated type.
*/
enum fsmState_t
{
    INITIALISE,     ///< Initialize all variables
    GET_FIRST_CHAR, ///< Get the first character
    GET_NEXT_CHAR,  ///< Get the next character
    PUT_CHAR,       ///< Placing a character
    PUT_FILLER,     ///< Placing filler (blank) columns
    PAUSE,          ///< Pausing between animations
    END             ///< Display cycle has completed
};

/***
  *  Structure for list of user defined characters substitutions.
*/
typedef struct charDef_t
{
    uint16_t   code;  ///< the ASCII code for the user defined character
    uint8_t   *data;  ///< user supplied data
    struct charDef_t *next;  ///< next in the list
} charDef_t;

/**
 * Zone object for the Parola library.
 * This class contains the text to be displayed and all the attributes for the zone.
 */
typedef struct {
    MD_MAX72XX_t  *_MX;   ///< Pointer to parent's MD_MAX72xx object passed in at begin()

    // Time and speed controlling data and methods
    bool      _suspend;     // don't do anything
    uint32_t  _lastRunTime; // the millis() value for when the animation was last run
    uint16_t  _tickTimeIn;  // the time between IN animations in milliseconds
    uint16_t  _tickTimeOut; // the time between OUT animations in milliseconds
    uint16_t  _pauseTime;   // time to pause the animation between 'in' and 'out'

    // Display control data and methods
    enum fsmState_t      _fsmState;          // fsm state for all FSMs used to display text
    uint16_t        _textLen;           // length of current text in columns
    int16_t         _limitLeft;         // leftmost limit for the current display effect
    int16_t         _limitRight;        // rightmost limit for the current display effect
    bool            _limitOverflow;     // true if the text will overflow the display
    enum textPosition_t  _textAlignment;     // current text alignment
    enum textEffect_t    _effectIn;          // the effect for text entering the display
    enum textEffect_t    _effectOut;         // the effect for text exiting the display
    bool            _moveIn;            // animation is moving IN when true, OUT when false
    bool            _inverted;          // true if the display needs to be inverted
    uint16_t        _scrollDistance;    // the space in columns between the end of one message and the start of the next
    uint8_t         _zoneEffect;        // bit mapped zone effects
    uint8_t         _intensity;         // display intensity
    bool            _animationAdvanced; // true is animation advanced inthe last animation call

    // Variables used in the effects routines. These can be used by the functions as needed.
    uint8_t   _zoneStart;   // First zone module number
    uint8_t   _zoneEnd;     // Last zone module number
    int16_t   _nextPos;     // Next position for animation. Can be used in several different ways depending on the function.
    int8_t    _posOffset;   // Looping increment depends on the direction of the scan for animation
    int16_t   _startPos;    // Start position for the text LED
    int16_t   _endPos;      // End limit for the text LED.

    // Character buffer handling data and methods
    const uint8_t *_pText;             // pointer to text buffer from user call
    const uint8_t *_pCurChar;          // the current character being processed in the text
    bool       _endOfText;             // true when the end of the text string has been reached.

    // Font character handling data and methods
    struct charDef_t *_userChars;  // the root of the list of user defined characters
    uint8_t   _cBufSize;    // allocated size of the array for loading character font (cBuf)
    uint8_t   *_cBuf;       // buffer for loading character font - allocated when font is set
    uint8_t   _charSpacing; // spacing in columns between characters
    uint8_t   _charCols;    // number of columns for this character
    int16_t   _countCols;   // count of number of columns already shown
    fontType_t  *_fontDef;  // font for this zone

    /// Sprite management
#if ENA_SPRITE
    uint8_t *_spriteInData, *_spriteOutData;
    uint8_t _spriteInWidth, _spriteOutWidth;
    uint8_t _spriteInFrames, _spriteOutFrames;
#endif

} MD_PZone_t;

void      MD_PZone_setInitialConditions(MD_PZone_t *z);       // set up initial conditions for an effect
bool      MD_PZone_calcTextLimits(MD_PZone_t *z,const uint8_t *p); // calculate the right and left limits for the text

void MD_PZone_setInitialEffectConditions(MD_PZone_t *z); // set the initial conditions for loops in the FSM
bool MD_PZone_getFirstChar(MD_PZone_t *z,uint8_t *len);   // put the first Text char into the char buffer
bool MD_PZone_getNextChar(MD_PZone_t *z,uint8_t *len);    // put the next Text char into the char buffer
void      MD_PZone_allocateFontBuffer(MD_PZone_t *z); // allocate _cBuf based on the size of the largest font characters
uint8_t   MD_PZone_findChar(MD_PZone_t *z,uint16_t code, uint8_t size, uint8_t *cBuf); // look for user defined character
uint8_t   MD_PZone_makeChar(MD_PZone_t *z,uint16_t c, bool addBlank);  // load a character bitmap and add in trailing _charSpacing blanks if req'd
void      MD_PZone_reverseBuf(MD_PZone_t *z,uint8_t *p, uint8_t size); // reverse the elements of the buffer
void      MD_PZone_invertBuf(MD_PZone_t *z,uint8_t *p, uint8_t size);  // invert the elements of the buffer

void      MD_PZone_moveTextPointer(MD_PZone_t *z);  // move the text pointer depending on direction of buffer scan

// Debugging aid
const char *MD_PZone_state2string(MD_PZone_t *z,enum fsmState_t s);

// Effect functions
void  MD_PZone_commonPrint(MD_PZone_t *z);
void  MD_PZone_effectPrint(MD_PZone_t *z,bool bIn);
void  MD_PZone_effectVScroll(MD_PZone_t *z,bool bUp, bool bIn);
void  MD_PZone_effectHScroll(MD_PZone_t *z,bool bLeft, bool bIn);
#if ENA_MISC
    void  MD_PZone_effectSlice(MD_PZone_t *z,bool bIn);
    void  MD_PZone_effectMesh(MD_PZone_t *z,bool bIn);
    void  MD_PZone_effectFade(MD_PZone_t *z,bool bIn);
    void  MD_PZone_effectBlinds(MD_PZone_t *z,bool bIn);
    void  MD_PZone_effectDissolve(MD_PZone_t *z,bool bIn);
    void  MD_PZone_effectRandom(MD_PZone_t *z,bool bIn);
#endif // ENA_MISC
#if ENA_SPRITE
    void  MD_PZone_effectSprite(MD_PZone_t *z,bool bIn, uint8_t id);
#endif // ENA_SPRITE
#if ENA_WIPE
    void  MD_PZone_effectWipe(MD_PZone_t *z,bool bLightBar, bool bIn);
#endif // ENA_WIPE
#if ENA_OPNCLS
    void  MD_PZone_effectOpen(MD_PZone_t *z,bool bLightBar, bool bIn);
    void  MD_PZone_effectClose(MD_PZone_t *z,bool bLightBar, bool bIn);
#endif // ENA_OPNCLS
#if ENA_SCR_DIA
    void  MD_PZone_effectDiag(MD_PZone_t *z,bool bUp, bool bLeft, bool bIn);
#endif // ENA_SCR_DIA
#if ENA_SCAN
    void  MD_PZone_effectHScan(MD_PZone_t *z,bool bIn, bool bBlank);
    void  MD_PZone_effectVScan(MD_PZone_t *z,bool bIn, bool bBlank);
#endif // ENA_SCAN
#if ENA_GROW
    void  MD_PZone_effectGrow(MD_PZone_t *z,bool bUp, bool bIn);
#endif // ENA_GROW

/**
   * Class constructor.
   *
   * Instantiate a new instance of the class.
   */
  void MD_PZone_constructor(MD_PZone_t *z);

  /**
   * Initialize the object.
   *
   * Initialize the object data. This will be called to initialize
   * new data for the class that cannot be done during the object creation.
   *
   * \param p pointer to the parent's MD_MAX72xx object.
   */
  void MD_PZone_begin(MD_PZone_t *z,MD_MAX72XX_t *p);

  /**
   * Class Destructor.
   *
   * Release allocated memory and does the necessary to clean up once the object is
   * no longer required.
   */
  void MD_PZone_destructor(MD_PZone_t *z);

  //--------------------------------------------------------------
  /** \name Methods for core object control.
   * @{
   */
  /**
   * Animate the zone.
   *
   * Animate using the currently specified text and animation parameters.
   * This method is invoked from the main Parola object.
   *
   * \return bool true if the zone animation has completed, false otherwise.
   */
  bool MD_PZone_zoneAnimate(MD_PZone_t *z);

  /**
   * Get the completion status.
   *
   * Return the current completion status for the zone animation.
   *
   * See comments for the MD_Parola getZoneStatus() method.
   *
   * \return bool true if the zone animation is completed
   */
  inline bool MD_PZone_getStatus(MD_PZone_t *z) { return (z->_fsmState == END); }

  /**
   * Get the start and end parameters for a zone.
   *
   * See comments for the MD_Parola namesake method.
   *
   * \param zStart  value for the start module number placed here [0..numZones-1].
   * \param zEnd  value for the end module number placed here [0..numZones-1].
   */
  inline void MD_PZone_getZone(MD_PZone_t *z,uint8_t *zStart, uint8_t *zEnd) { *zStart = z->_zoneStart; *zEnd = z->_zoneEnd; }

  /**
  * Check if animation frame has advanced.
  *
  * Check if the last call to zoneAnimate() resulted in the animation frame advancing by
  * one or more frames in one or more zones.
  *
  * \return True if the animation frame advanced in any of the display zones.
  */
  inline bool MD_PZone_isAnimationAdvanced(MD_PZone_t *z) { return(z->_animationAdvanced); }

  /**
   * Clear the zone.
   *
   * See comments for the MD_Parola namesake method.
   *
   * \return No return value.
   */
  void MD_PZone_zoneClear(MD_PZone_t *z);

  /**
   * Reset the current zone animation to restart.
   *
   * See comments for the MD_Parola namesake method.
   *
   * \return No return value.
   */
  inline void MD_PZone_zoneReset(MD_PZone_t *z) { z->_fsmState = INITIALISE; }

  /**
  * Shutdown or resume zone hardware.
  *
  * See comments for the MD_Parola namesake method.
  *
  * \param b  boolean value to shutdown (true) or resume (false).
  * \return No return value.
  */
  inline void MD_PZone_zoneShutdown(MD_PZone_t *z,bool b) { MD_MAX72XX_control2(z->_MX,z->_zoneStart, z->_zoneEnd, SHUTDOWN, b ? ON : OFF); }

  /**
   * Suspend or resume zone updates.
   *
   * See comments for the MD_Parola namesake method.
   *
   * \param b boolean value to suspend (true) or resume (false).
   * \return No return value.
   */
  inline void MD_PZone_zoneSuspend(MD_PZone_t *z,bool b) { z->_suspend = b; }

  /**
   * Set the start and end parameters for a zone.
   *
   * See comments for the MD_Parola namesake method.
   *
   * \param zStart  the first module number for the zone [0..numZones-1].
   * \param zEnd  the last module number for the zone [0..numZones-1].
   */
  inline void MD_PZone_setZone(MD_PZone_t *z,uint8_t zStart, uint8_t zEnd) { z->_zoneStart = zStart; z->_zoneEnd = zEnd; }

  /** @} */
  //--------------------------------------------------------------
  /** \name Support methods for visually adjusting the display.
   * @{
   */

  /**
   * Get the zone inter-character spacing in columns.
   *
   * \return the current setting for the space between characters in columns.
   */
  inline uint8_t MD_PZone_getCharSpacing(MD_PZone_t *z) { return z->_charSpacing; }

  /**
  * Get the zone brightness.
  *
  * Get the intensity (brightness) of the display.
  *
  * \return The intensity setting.
  */
  inline uint8_t MD_PZone_getIntensity(MD_PZone_t *z) { return z->_intensity; }

  /**
   * Get the zone current invert state.
   *
   * See the setInvert() method.
   *
   * \return the inverted boolean value.
   */
  inline bool MD_PZone_getInvert(MD_PZone_t *z) { return z->_inverted; }

  /**
   * Get the zone pause time.
   *
   * See the setPause() method.
   *
   * \return the pause value in milliseconds.
   */
  inline uint16_t MD_PZone_getPause(MD_PZone_t *z) { return z->_pauseTime; }

  /**
     * Get the horizontal Scroll spacing.
     *
     * See the setScrollSpacing() method
     *
     * \return the space between message in columns.
     */
  inline uint16_t MD_PZone_getScrollSpacing(MD_PZone_t *z) { return z->_scrollDistance; }

/**
 * Get the zone animation IN speed.
 *
 * See the setSpeed() method.
 *
 * \return the speed value.
 */
inline uint16_t MD_PZone_getSpeedIn(MD_PZone_t *z) { return z->_tickTimeIn; }

/**
   * Get the zone animation speed.
   *
   * See the setSpeed() method.
   * This should be replaced with either getSpeedIn() or getSpeedOut()
   * unless it is known that both directions are running at the same speed.
   *
   * \return the IN speed value.
   */
  inline uint16_t MD_PZone_getSpeed(MD_PZone_t *z) { return MD_PZone_getSpeedIn(z); }

  /**
   * Get the zone animation OUT speed.
   *
   * See the setSpeed() method.
   *
   * \return the speed value.
   */
  inline uint16_t MD_PZone_getSpeedOut(MD_PZone_t *z) { return z->_tickTimeOut; }
  
  /**
  * Get the zone animation start time.
  *
  * See the setSynchTime() method
  *
  * \return the internal time reference.
  */
  inline uint32_t MD_PZone_getSynchTime(MD_PZone_t *z) { return z->_lastRunTime; }

  /**
   * Get the current text alignment specification.
   *
   * \return the current text alignment setting
   */
  inline enum textPosition_t MD_PZone_getTextAlignment(MD_PZone_t *z) { return z->_textAlignment; }

  /**
   *  Get the width of text in columns
   *
   * Calculate the width of the characters and the space between them
   * using the current font and text settings.
   *
   * \param p   pointer to a text string
   * \return the width of the string in display columns
   */
   uint16_t MD_PZone_getTextWidth(MD_PZone_t *z,const uint8_t* p);

  /**
   * Get the value of specified display effect.
   *
   * The display effect is one of the zoneEffect_t types. The returned value will be
   * true if the attribute is set, false if the attribute is not set.
   *
   * \param ze  the required text alignment.
   * \return true if the value is set, false otherwise.
   */
  boolean MD_PZone_getZoneEffect(MD_PZone_t *z,enum zoneEffect_t ze);

  /**
   * Set the zone inter-character spacing in columns.
   *
   * Set the number of blank columns between characters when they are displayed.
   *
   * \param cs  space between characters in columns.
   * \return No return value.
   */
  inline void MD_PZone_setCharSpacing(MD_PZone_t *z,uint8_t cs) { z->_charSpacing = cs; MD_PZone_allocateFontBuffer(z); }

  /**
   * Set the zone brightness.
   *
   * Set the intensity (brightness) of the display.
   *
   * \param intensity the intensity to set the display (0-15).
   * \return No return value.
   */
  void MD_PZone_setIntensity(MD_PZone_t *z,uint8_t intensity);
//  inline void setIntensity(uint8_t intensity) { _intensity = intensity; _MX->control(_zoneStart, _zoneEnd, MD_MAX72XX::INTENSITY, _intensity); }

  /**
   * Invert the zone display.
   *
   * Set the display to inverted (ON LED turns OFF and vice versa).
   *
   * \param invert  true for inverted display, false for normal display
   * \return No return value.
   */
  inline void MD_PZone_setInvert(MD_PZone_t *z,uint8_t invert) { z->_inverted = invert; }

  /**
   * Set the pause between ENTER and EXIT animations for this zone.
   *
   * Between each entry and exit, the library will pause by the number of milliseconds
   * specified to allow the viewer to read the message. For continuous scrolling displays
   * this should be set to the same value as the display speed.
   *
   * \param pause the time, in milliseconds, between animations.
   * \return No return value.
   */
  inline void MD_PZone_setPause(MD_PZone_t *z,uint16_t pause) { z->_pauseTime = pause; }

  /**
   * Set the horizontal scrolling distance between messages.
   *
   * When scrolling horizontally, the distance between the end of one message and the
   * start of the next can be set using this method. Normal operation is for the message
   * to be fully off the display before the new message starts.
   * Set to zero for default behavior.
   *
   * \param space the spacing, in columns, between messages; zero for default behaviour..
   * \return No return value.
   */
  inline void MD_PZone_setScrollSpacing(MD_PZone_t *z,uint16_t space) { z->_scrollDistance = space; }

/**
 * Set separate IN and OUT zone animation frame speed.
 *
 * The speed of the display is the 'tick' time between animation frames. The lower this time
 * the faster the animation; set it to zero to run as fast as possible.
 *
 * This method will set both the IN and OUT animations separately to the specified speed.
 *
 * \param speedIn the time, in milliseconds, between IN animation frames.
 * \param speedOut the time, in milliseconds, between OUT animation frames.
 * \return No return value.
 */
inline void MD_PZone_setSpeedInOut(MD_PZone_t *z,uint16_t speedIn, uint16_t speedOut) { z->_tickTimeIn = speedIn; z->_tickTimeOut = speedOut; }

/**
   * Set the zone animation frame speed.
   *
   * The speed of the display is the 'tick' time between animation frames. The lower this time
   * the faster the animation; set it to zero to run as fast as possible.
   *
   * This method will set the same value for both IN and OUT animations speed.
   *
   * \param speed the time, in milliseconds, between animation frames.
   * \return No return value.
   */
  inline void MD_PZone_setSpeed(MD_PZone_t *z,uint16_t speed) { MD_PZone_setSpeedInOut(z,speed, speed); }

#if ENA_SPRITE
  /**
  * Set data for user sprite effects.
  *
  * Set up the data parameters for user sprite text entry/exit effects.
  * See the comments for the namesake method in MD_Parola.
  *
  * \param inData pointer to the data table defining the entry sprite.
  * \param inWidth the width (in bytes) of each frame of the sprite.
  * \param inFrames the number of frames for the sprite.
  * \param outData pointer to the data table that is inWidth*InFrames in size.
  * \param outWidth the width (in bytes) of each frame of the sprite.
  * \param outFrames the number of frames for the sprite.
  * \return No return value.
  */
  void MD_PZone_setSpriteData(MD_PZone_t *z,const uint8_t *inData,  uint8_t inWidth,  uint8_t inFrames,
                     const uint8_t *outData, uint8_t outWidth, uint8_t outFrames);
#endif

  /**
  * Set the zone animation start time.
  *
  * Each zone animation has an associated start time. The start time
  * defaults to the time when the zone is initialized. This method allows
  * synchronization between zones by setting the same start time. Should be
  * used in conjunction with the getSynchTime() method as the return value
  * should only be treated as an internal reference and arbitrary values
  * will result in irregular behavior.
  *
  * \param zt the required start time.
  * \return No return value.
  */
  inline void MD_PZone_setSynchTime(MD_PZone_t *z,uint32_t zt) { z->_lastRunTime = zt; }

  /**
   * Set the text alignment within the zone.
   *
   * Text alignment is specified as one of the values in textPosition_t.
   *
   * \param ta  the required text alignment.
   * \return No return value.
   */
  inline void MD_PZone_setTextAlignment(MD_PZone_t *z,enum textPosition_t ta) { z->_textAlignment = ta; }

  /**
   * Set the pointer to the text buffer for this zone.
   *
   * Sets the text buffer to be a pointer to user data.
   * See the comments for the namesake method in MD_Parola.
   *
   * \param pb  pointer to the text buffer to be used.
   * \return No return value.
   */
  inline void MD_PZone_setTextBuffer(MD_PZone_t *z,const char *pb) { z->_pText = (const uint8_t *)pb; }

  /**
   * Set the entry and exit text effects for the zone.
   *
   * See the comments for the namesake method in MD_Parola.
   *
   * \param effectIn  the entry effect, one of the textEffect_t enumerated values.
   * \param effectOut the exit effect, one of the textEffect_t enumerated values.
   * \return No return value.
   */
  inline void MD_PZone_setTextEffect(MD_PZone_t *z,enum textEffect_t effectIn, enum textEffect_t effectOut) { z->_effectIn = (effectIn == PA_NO_EFFECT ? PA_PRINT : effectIn), z->_effectOut = effectOut; }

  /**
   * Set the zone display effect.
   *
   * The display effect is one of the zoneEffect_t types, and this will be set (true) or
   * reset (false) depending on the boolean value. The resulting zone display will be
   * modified as per the required effect.
   *
   * \param b set the value if true, reset the value if false
   * \param ze  the required text alignment.
   * \return No return value.
   */
  void MD_PZone_setZoneEffect(MD_PZone_t *z,boolean b, enum zoneEffect_t ze);

  /** @} */

  //--------------------------------------------------------------
  /** \name Support methods for fonts and characters.
   * @{
   */

  /**
   * Add a user defined character to the replacement list.
   *
   * Add a replacement characters to the user defined list. The character data must be
   * the same as for a single character in the font definition file. If a character is
   * specified with a code the same as an existing character the existing data will be
   * substituted for the new data. A character code of 0 is illegal as this denotes the
   * end of string character for C++ and cannot be used in an actual string.
   * The library does not copy the in the data in the data definition but only retains
   * a pointer to the data, so any changes to the data storage in the calling program will
   * be reflected in the library.
   *
   * \param code  code for the character data.
   * \param data  pointer to the character data.
   * \return true of the character was inserted in the substitution list.
   */
  bool MD_PZone_addChar(MD_PZone_t *z,uint16_t code, uint8_t *data);

  /**
   * Delete a user defined character to the replacement list.
   *
   * Delete a replacement character to the user defined list. A character code of 0 is
   * illegal as this denotes the end of string character for C++ and cannot be used in
   * an actual string.
   *
   * \param code  ASCII code for the character data.
   * \return true of the character was found in the substitution list.
   */
  bool MD_PZone_delChar(MD_PZone_t *z,uint16_t code);

  /**
   * Get the display font.
   *
   * Return the current font table pointer for this zone.
   *
   * \return Pointer to the font definition used.
   */
  inline fontType_t* MD_PZone_getZoneFont(MD_PZone_t *z) { return z->_fontDef; };

  /**
   * Set the display font.
   *
   * See comments for the namesake Parola method.
   *
   * \param fontDef Pointer to the font definition to be used.
   * \return No return value.
   */
  inline void MD_PZone_setZoneFont(MD_PZone_t *z,fontType_t *fontDef) { z->_fontDef = fontDef; MD_MAX72XX_setFont(z->_MX,z->_fontDef); MD_PZone_allocateFontBuffer(z); }

  /** @} */

#if ENA_GRAPHICS
  //--------------------------------------------------------------
  /** \name Support methods for graphics.
  * @{
  */
  /**
  * Get the start and end column numbers for the zone.
  *
  * Returns the start and end column numbers for the zone display.
  * This retains consistency between user code and library.
  *
  * \param startColumn the by-reference parameter that will hold the return value for the start column.
  * \param endColumn the by-reference parameter that will hold the return value for the end column.
  * \return The start and end columns in the by-reference parameters.
  */
  inline void MD_PZone_getZoneExtent(MD_PZone_t *z,uint16_t *startColumn, uint16_t *endColumn) { *startColumn = ZONE_START_COL(z->_zoneStart); *endColumn = ZONE_END_COL(z->_zoneEnd); }

  /**
  * Get the start and end column numbers for the text displayed.
  *
  * Returns the start and end column numbers for the text displayed in the zone.
  * This retains consistency between user code and library.
  *
  * \param startColumn the by-reference parameter that will hold the return value for the start column.
  * \param endColumn the by-reference parameter that will hold the return value for the end column.
  * \return The start and end columns in the by-reference parameters.
  */
  inline void MD_PZone_getTextExtent(MD_PZone_t *z,uint16_t *startColumn, uint16_t *endColumn) { *startColumn = z->_limitLeft; *endColumn = z->_limitRight; }

  /** @} */
#endif

/**
 * Core object for the Parola library.
 * This class contains one or more zones for display.
 */
 typedef struct {
     // The display hardware controlled by this library
     MD_MAX72XX_t  _D;         ///< Hardware library object
#if STATIC_ZONES
     MD_PZone_t    _Z[MAX_ZONES];  ///< Fixed number of zones - static zone allocation
#else
     MD_PZone_t    *_Z;            ///< Zones buffers - dynamic zone allocation
#endif
     uint8_t     _numModules;///< Number of display modules [0..numModules-1]
     uint8_t     _numZones;  ///< Max number of zones in the display [0..numZones-1]
 } MD_Parola_t;

  /**
   * Class constructor - arbitrary output pins.
   *
   * Instantiate a new instance of the class. The parameters passed are used to
   * connect the software to the hardware using the MD_MAX72XX class.
   *
   * See documentation for the MD_MAX72XX library for detailed explanation of parameters.
   *
   * \param mod       the hardware module type used in the application. One of the MD_MAX72XX::moduleType_t values.
   * \param dataPin   output on the Arduino where data gets shifted out.
   * \param clkPin    output for the clock signal.
   * \param csPin     output for selecting the device.
   * \param numDevices  number of devices connected. Default is 1 if not supplied.
   */
  inline void MD_Parola_constructor(MD_Parola_t *p,enum moduleType_t mod, uint8_t dataPin, uint8_t clkPin, uint8_t csPin, uint8_t numDevices) {
    MD_MAX72XX_constructor(&p->_D,mod, dataPin, clkPin, csPin, numDevices);
    p->_numModules = numDevices;
  }

  /**
   * Class constructor - SPI hardware interface.
   *
   * Instantiate a new instance of the class. The parameters passed are used to
   * connect the software to the hardware using the MD_MAX72XX class.
   *
   * See documentation for the MD_MAX72XX library for detailed explanation of parameters.
   *
   * \param mod       the hardware module type used in the application. One of the MD_MAX72XX::moduleType_t values.
   * \param csPin   output for selecting the device.
   * \param numDevices  number of devices connected. Default is 1 if not supplied.
   */
  void MD_Parola_constructor2(MD_Parola_t *p,enum moduleType_t mod, uint8_t csPin, uint8_t numDevices);

/**
 * Initialize the object.
 *
 * Initialize the object data. This needs to be called during setup() to initialize new
 * data for the class that cannot be done during the object creation. This form of the
 * method allows specifying the number of zones used. The module limits for the individual
 * zones are initialized separately using setZone(), which should be done immediately after
 * the invoking begin().
 *
 * \sa setZone()
 *
 * \param numZones  maximum number of zones
 */
void MD_Parola_begin(MD_Parola_t *p,uint8_t numZones);

/**
   * Initialize the object.
   *
   * Initialize the object data. This needs to be called during setup() to initialize new
   * data for the class that cannot be done during the object creation. This form of the
   * method is for backward compatibility and creates one zone for the entire display.
   */
void MD_Parola_begin1(MD_Parola_t *p);

  /**
   * Class Destructor.
   *
   * Release allocated memory and does the necessary to clean up once the object is
   * no longer required.
   */
  void MD_Parola_destructor(MD_Parola_t *p);

  //--------------------------------------------------------------
  /** \name Methods for core object control.
   * @{
   */
  /**
   * Animate the display.
   *
   * Animate all the zones in the display using the currently specified text and
   * animation parameters. This method needs to be invoked as often as possible
   * to ensure smooth animation. The animation is governed by a time tick that
   * is set by the setSpeed() or setSpeedInOut() methods and it will pause between 
   * entry and exit using the time set by the setPause() method.
   *
   * The calling program should monitor the return value for 'true' in order to either
   * reset the zone animation or supply another string for display. A 'true' return
   * value means that one or more zones have completed their animation.
   *
   * Not all calls to this method result in an animation, as this is governed by
   * the timing parameters set for the animation. To determine when an animation has
   * advanced during the call, the user code can call the isAnimationAdvanced() method.
   *
   * \return bool true if at least one zone animation has completed, false otherwise.
   */
  bool MD_Parola_displayAnimate(MD_Parola_t *p);

  /**
   * Get the completion status for a zone.
   *
   * This method is to determine which zone has completed when displayAnimate()
   * has returned a completion status.
   *
   * The calling program should monitor the return value for 'true' in order to either
   * reset the zone animation or supply another string for display. A 'true' return
   * value means that the zone has completed its animation cycle.
   *
   * \param z     specified zone
   * \return bool true if the zone animation has completed, false otherwise.
   */
  inline bool MD_Parola_getZoneStatus(MD_Parola_t *p,uint8_t z) { if (z < p->_numZones) return(MD_PZone_getStatus(&p->_Z[z])); else return(true); }

  /**
   * Clear the display.
   *
   * Clear all the zones in the current display.
   *
   * \return No return value.
   */
  inline void MD_Parola_displayClear(MD_Parola_t *p) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_zoneClear(&p->_Z[i]); }

  /**
   * Clear one zone in the display.
   *
   * Clear the specified zone in the current display.
   *
   * \param z   specified zone
   * \return No return value.
   */
  inline void MD_Parola_displayClear1(MD_Parola_t *p,uint8_t z) { if (z < p->_numZones) MD_PZone_zoneClear(&p->_Z[z]); }

  /**
   * Reset the current animation to restart for all zones.
   *
   * This method is used to reset all the zone animations an animation back to the start
   * of their cycle current cycle.
   * It is normally invoked after all the parameters for a display are set and the
   * animation needs to be started (or restarted).
   *
   * \return No return value.
   */
  inline void MD_Parola_displayReset(MD_Parola_t *p) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_zoneReset(&p->_Z[i]); }

  /**
   * Reset the current animation to restart for the specified zone.
   *
   * See the comments for the 'all zones' variant of this method.
   *
   * \param z specified zone
   * \return No return value.
   */
  void MD_Parola_displayReset1(MD_Parola_t *p,uint8_t z);

  /**
  * Shutdown or restart display hardware.
  *
  * Shutdown the display hardware to a low power state. The display will
  * be blank during the shutdown. Calling animate() will continue to
  * animate the display in the memory buffers but this will not be visible
  * on the display (ie, the libraries still function but the display does not).
  *
  * \param b  boolean value to shutdown (true) or resume (false).
  * \return No return value.
  */
  inline void MD_Parola_displayShutdown(MD_Parola_t *p,bool b) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_zoneShutdown(&p->_Z[i],b); }

  /**
  * Suspend or resume display updates.
  *
  * Stop the current display animation. When pausing it leaves the
  * display showing the current text. Resuming will restart the animation where
  * it left off. To reset the animation back to the beginning, use the
  * displayReset() method.
  *
  * \param b  boolean value to suspend (true) or resume (false).
  * \return No return value.
  */
  inline void MD_Parola_displaySuspend(MD_Parola_t *p,bool b) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_zoneSuspend(&p->_Z[i],b); }

  /**
  * Check if animation frame has advanced.
  *
  * Check if the last call to animate() resulted in the animation frame advancing by
  * one or more frames in one or more zones. Useful to integrate graphics into the
  * display as they may be affected by the text animation.
  *
  * For displays with more than one zone, the user code will need to interrogate each zone
  * to determine whether the animation advanced in that zone.
  *
  * \return True if the animation frame advanced in any of the display zones.
  */
  inline bool MD_Parola_isAnimationAdvanced(MD_Parola_t *p) { bool b = false; for (uint8_t i = 0; i < p->_numZones; i++) b |= MD_PZone_isAnimationAdvanced(&p->_Z[i]); return(b); }

  /**
   * Get the module limits for a zone.
   *
   * Once a zone has been defined, this method will return the 
   * start and end module that were defined for the specified zone.
   *
   * \sa setZone()
   *
   * \param z   zone number.
   * \param moduleStart returns the first module number for the zone [0..numZones-1].
   * \param moduleEnd   returns last module number for the zone [0..numZones-1].
   */
  inline void MD_Parola_getZone(MD_Parola_t *p,uint8_t z, uint8_t *moduleStart, uint8_t *moduleEnd) { if (z < p->_numZones) MD_PZone_getZone(&p->_Z[z],moduleStart, moduleEnd); }

  /**
   * Define the module limits for a zone.
   *
   * When multiple zones are defined, the library needs to know the contiguous module
   * ranges that make up the different zones. If the library has been started with only
   * one zone then it will automatically initialize the zone to be the entire range for
   * the display modules, so calling this function is not required. However, when multiple
   * zones are defined, setZone() for each zone should be should be invoked immediately 
   * after the call to begin().
   *
   * A module is a unit of 8x8 LEDs, as defined in the MD_MAX72xx library.
   * Zones should not overlap or unexpected results will occur.
   *
   * \sa begin()
   *
   * \param z   zone number.
   * \param moduleStart the first module number for the zone [0..numZones-1].
   * \param moduleEnd   the last module number for the zone [0..numZones-1].
   * \return true if set, false otherwise.
   */
  bool MD_Parola_setZone(MD_Parola_t *p,uint8_t z, uint8_t moduleStart, uint8_t moduleEnd);

  /** @} */
  //--------------------------------------------------------------
  /** \name Methods for quick start displays.
   * @{
   */
/**
  * Easy start for a non-scrolling zone text display.
  *
  * This method is a convenient way to set up a static text display within the
  * specified zone. All the data necessary for setup is passed through as
  * parameters and the display animation is started.
  *
  * \param z   zone specified.
  * \param pText parameter suitable for the setTextBuffer() method.
  * \param align parameter suitable for the the setTextAlignment() method.
  * \param speed parameter suitable for the setSpeed() method.
  * \param pause parameter suitable for the setPause() method.
  * \param effectIn  parameter suitable for the setTextEffect() method.
  * \param effectOut parameter suitable for the setTextEffect() method.
  * \return No return value.
  */
 void MD_Parola_displayZoneText(MD_Parola_t *p,uint8_t z, const char *pText, enum textPosition_t align, uint16_t speed, uint16_t pause, enum textEffect_t effectIn, enum textEffect_t effectOut);

/**
   * Easy start for a scrolling text display.
   *
   * This method is a convenient way to set up a scrolling display. All the data
   * necessary for setup is passed through as parameters and the display animation
   * is started. Assumes one zone only (zone 0).
   *
   * \param pText   parameter suitable for the setTextBuffer() method.
   * \param align   parameter suitable for the the setTextAlignment() method.
   * \param effect  parameter suitable for the the setTextEffect() method.
   * \param speed   parameter suitable for the setSpeed() method.
   * \return No return value.
   */
  inline void MD_Parola_displayScroll(MD_Parola_t *p,const char *pText, enum textPosition_t align, enum textEffect_t effect, uint16_t speed)
  {
      MD_Parola_displayZoneText(p,0, pText, align, speed, 0, effect, effect);
  }

 /**
   * Easy start for a non-scrolling text display.
   *
   * This method is a convenient way to set up a static text display. All the data
   * necessary for setup is passed through as parameters and the display animation
   * is started. Assumes one zone only (zone 0).
   *
   * \param pText parameter suitable for the setTextBuffer() method.
   * \param align parameter suitable for the the setTextAlignment() method.
   * \param speed parameter suitable for the setSpeed() method.
   * \param pause parameter suitable for the setPause() method.
   * \param effectIn  parameter suitable for the setTextEffect() method.
   * \param effectOut parameter suitable for the setTextEffect() method.
   * \return No return value.
   */
  void MD_Parola_displayText(MD_Parola_t *p,const char *pText, enum textPosition_t align, uint16_t speed, uint16_t pause, enum textEffect_t effectIn, enum textEffect_t effectOut);

  /** @} */
  //--------------------------------------------------------------
  /** \name Support methods for visually adjusting the display.
   * @{
   */

  /**
   * Get the inter-character spacing in columns for a specific zone.
   *
   * \param z   zone number.
   * \return the current setting for the space between characters in columns.
   */
  inline uint8_t MD_Parola_getCharSpacing(MD_Parola_t *p,uint8_t z) { return (z < p->_numZones ? MD_PZone_getCharSpacing(&p->_Z[z]) : 0); }

/**
 * Get the inter-character spacing in columns.
 *
 * \return the current setting for the space between characters in columns. Assumes one zone only.
 */
inline uint8_t MD_Parola_getCharSpacing1(MD_Parola_t *p) { return MD_Parola_getCharSpacing(p,0); }

/**
 * Get the current display invert state for a specific zone.
 *
 * See the setInvert() method.
 *
 * \param z   zone number.
 * \return the inverted boolean value for the specified zone.
 */
inline bool MD_Parola_getInvert(MD_Parola_t *p,uint8_t z) { return (z < p->_numZones ? MD_PZone_getInvert(&p->_Z[z]) : false); }

/**
   * Get the current display invert state.
   *
   * See the setInvert() method.
   *
   * \return true if the display is inverted. Assumes one zone only.
   */
  inline bool MD_Parola_getInvert1(MD_Parola_t *p) { return MD_Parola_getInvert(p,0); }

/**
 * Get the current pause time for a specific zone.
 *
 * See the setPause() method.
 *
 * \param z   zone number.
 * \return the pause value in milliseconds for the specified zone.
 */
inline uint16_t MD_Parola_getPause(MD_Parola_t *p,uint8_t z) { return (z < p->_numZones ? MD_PZone_getPause(&p->_Z[z]) : 0); }

/**
   * Get the current pause time.
   *
   * See the setPause() method. Assumes one zone only.
   *
   * \return the pause value in milliseconds.
   */
  inline uint16_t MD_Parola_getPause1(MD_Parola_t *p) { return MD_Parola_getPause(p,0); }

  /**
   * Get the horizontal scrolling spacing.
   *
   * See the setScrollSpacing() method. Assumes one zone only
   *
   * \return the speed value.
   */
  inline uint16_t MD_Parola_getScrollSpacing(MD_Parola_t *p) { return MD_PZone_getScrollSpacing(&p->_Z[0]); }

/**
 * Get the current IN animation speed for the specified zone.
 *
 * See the setSpeed() method.
 *
 * \param z   zone number.
 * \return the speed value for the specified zone.
 */
inline uint16_t MD_Parola_getSpeed(MD_Parola_t *p,uint8_t z) { return (z < p->_numZones ? MD_PZone_getSpeed(&p->_Z[z]) : 0); }

/**
   * Get the current IN animation speed.
   *
   * See the setSpeed() method. Assumes one zone only
   *
   * \return the speed value.
   */
  inline uint16_t MD_Parola_getSpeed1(MD_Parola_t *p) { return MD_Parola_getSpeed(p,0); }

  /**
   * Get the current IN animation speed for the specified zone.
   *
   * See the setSpeed() method.
   *
   * \param z   zone number.
   * \return the IN speed value for the specified zone.
   */
  inline uint16_t MD_Parola_getSpeedIn(MD_Parola_t *p,uint8_t z) { return (z < p->_numZones ? MD_PZone_getSpeedIn(&p->_Z[z]) : 0); }

  /**
   * Get the current OUT animation speed for the specified zone.
   *
   * See the setSpeed() method.
   *
   * \param z   zone number.
   * \return the OUT speed value for the specified zone.
   */
  inline uint16_t MD_Parola_getSpeedOut(MD_Parola_t *p,uint8_t z) { return (z < p->_numZones ? MD_PZone_getSpeedOut(&p->_Z[z]) : 0); }

 /**
   * Get the current text alignment specification for the specified zone.
   *
   * \param z   zone number.
   * \return the current text alignment setting for the specified zone.
   */
  inline enum textPosition_t MD_Parola_getTextAlignment(MD_Parola_t *p,uint8_t z) { return (z < p->_numZones ? MD_PZone_getTextAlignment(&p->_Z[z]) : PA_CENTER); }

/**
  * Get the current text alignment specification.
  *
  * Assumes one zone only.
  *
  * \return the current text alignment setting.
  */
inline enum textPosition_t MD_Parola_getTextAlignment1(MD_Parola_t *p) { return MD_Parola_getTextAlignment(p,0); }

 /**
   * Get the text width in columns

   * Evaluate the width in column for the text string *p in the zone specified, as 
   * the sum of all the characters and the space between them. As each zone can  
   * display using a different font table, the result can vary between zones.
   *
   * \param z   zone number.
   * \param p   nul terminate character string to evaluate.
   * \return the number of columns used to display the text.
   */
  inline uint16_t MD_Parola_getTextColumns(MD_Parola_t *p,uint8_t z, const char *s) { return(z < p->_numZones && s != NULL ? MD_PZone_getTextWidth(&p->_Z[z],(uint8_t *)s) : 0); }

/**
   * Get the text width in columns

   * Evaluate the width in column for the text string *p as the sum of all
   * the characters and the space between them, using the currently assigned font.
   * Assumes one zone display.
   *
   * \param p   nul terminate character string to evaluate.
   * \return the number of columns used to display the text.
   */
inline uint16_t MD_Parola_getTextColumns1(MD_Parola_t *p,const char *s) { return(MD_Parola_getTextColumns(p,0, s)); }

/**
   * Get the value of specified display effect.
   *
   * The display effect is one of the zoneEffect_t types. The returned value will be
   * true if the attribute is set, false if the attribute is not set.
   *
   * \param z   zone number.
   * \param ze  the required text alignment.
   * \return true if the value is set, false otherwise.
   */
  inline boolean MD_Parola_getZoneEffect(MD_Parola_t *p,uint8_t z, enum zoneEffect_t ze) { return (z < p->_numZones ? MD_PZone_getZoneEffect(&p->_Z[z],ze) : false); }

  /**
   * Set the inter-character spacing in columns for all zones.
   *
   * Set the number of blank columns between characters when they are displayed.
   *
   * \param cs  space between characters in columns.
   * \return No return value.
   */
  void MD_Parola_setCharSpacing(MD_Parola_t *p,uint8_t cs);

  /**
   * Set the inter-character spacing in columns for the specified zone.
   *
   * See comments for the 'all zones' variant of this method.
   *
   * \param z   zone number.
   * \param cs  space between characters in columns.
   * \return No return value.
   */
  inline void MD_Parola_setCharSpacing2(MD_Parola_t *p,uint8_t z, uint8_t cs) { if (z < p->_numZones) MD_PZone_setCharSpacing(&p->_Z[z],cs); }

  /**
   * Set the display brightness for all the zones.
   *
   * Set the intensity (brightness) of the display.
   *
   * \param intensity the intensity to set the display (0-15).
   * \return No return value.
   */
  inline void MD_Parola_setIntensity(MD_Parola_t *p,uint8_t intensity) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_setIntensity(&p->_Z[i],intensity); }

  /**
   * Set the display brightness for the specified zone.
   *
   * See comments for the 'all zones' variant of this method.
   *
   * \param z   zone number.
   * \param intensity the intensity to set the display (0-15).
   * \return No return value.
   */
  inline void MD_Parola_setIntensity2(MD_Parola_t *p,uint8_t z, uint8_t intensity) { if (z < p->_numZones) MD_PZone_setIntensity(&p->_Z[z],intensity); }

  /**
   * Invert the display in all the zones.
   *
   * Set the display to inverted (ON LED turns OFF and vice versa).
   *
   * \param invert  true for inverted display, false for normal display
   * \return No return value.
   */
  void MD_Parola_setInvert(MD_Parola_t *p,uint8_t invert);

  /**
   * Invert the display in the specified zone.
   *
   * See comments for the 'all zones' variant of this method.
   *
   * \param z   zone number.
   * \param invert  true for inverted display, false for normal display
   * \return No return value.
   */
  inline void MD_Parola_setInvert2(MD_Parola_t *p,uint8_t z, uint8_t invert) { if (z < p->_numZones) MD_PZone_setInvert(&p->_Z[z],invert); }

  /**
   * Set the pause between ENTER and EXIT animations for all zones.
   *
   * Between each entry and exit, the library will pause by the number of milliseconds
   * specified to allow the viewer to read the message. For continuous scrolling displays
   * this should be set to the same value as the display speed.
   *
   * \param pause the time, in milliseconds, between animations.
   * \return No return value.
   */
  void MD_Parola_setPause(MD_Parola_t *p,uint16_t pause);

  /**
   * Set the pause between ENTER and EXIT animations for the specified zone.
   *
   * See comments for the 'all zones' variant of this method.
   *
   * \param z   zone number.
   * \param pause the time, in milliseconds, between animations.
   * \return No return value.
   */
  inline void MD_Parola_setPause2(MD_Parola_t *p,uint8_t z, uint16_t pause) { if (z < p->_numZones) MD_PZone_setPause(&p->_Z[z],pause); }

  /**
   * Set the horizontal scrolling distance between messages for all the zones.
   *
   * When scrolling horizontally, the distance between the end of one message and the
   * start of the next can be set using this method. Default behavior is for the message
   * to be fully off the display before the new message starts.
   * Set to zero for default behavior.
   *
   * \param space the spacing, in columns, between messages; zero for default behaviour..
   * \return No return value.
   */
  void MD_Parola_setScrollSpacing(MD_Parola_t *p,uint16_t space);

  /**
   * Set identical IN and OUT animation frame speed for all zones.
   *
   * The speed of the display is the 'tick' time between animation frames. The lower this time
   * the faster the animation; set it to zero to run as fast as possible.
   * 
   * This method sets the IN and OUT animation speeds to be the same.
   *
   * \param speed the time, in milliseconds, between animation frames.
   * \return No return value.
   */
  inline void MD_Parola_setSpeed(MD_Parola_t *p,uint16_t speed) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_setSpeed(&p->_Z[i],speed); }

  /**
   * Set separate IN and OUT animation frame speed for all zones.
   *
   * The speed of the display is the 'tick' time between animation frames. The lower this time
   * the faster the animation; set it to zero to run as fast as possible.
   * 
   * This method allows the IN and OUT animation speeds to be different.
   *
   * \param speedIn the time, in milliseconds, between IN animation frames.
   * \param speedOut the time, in milliseconds, between OUT animation frames.
   * \return No return value.
   */
  void MD_Parola_setSpeedInOut(MD_Parola_t *p,uint16_t speedIn, uint16_t speedOut);

  /**
   * Set the identical IN and OUT animation frame speed for the specified zone.
   *
   * See comments for the 'all zones' variant of this method.
   *
   * \param z   zone number.
   * \param speed the time, in milliseconds, between animation frames.
   * \return No return value.
   */
  void MD_Parola_setSpeed2(MD_Parola_t *p,uint8_t z, uint16_t speed);

  /**
   * Set the separate IN and OUT animation frame speed for the specified zone.
   *
   * See comments for the 'all zones' variant of this method.
   *
   * \param z   zone number.
   * \param speedIn the time, in milliseconds, between IN animation frames.
   * \param speedOut the time, in milliseconds, between OUT animation frames.
   * \return No return value.
   */
  inline void MD_Parola_setSpeedInOut2(MD_Parola_t *p,uint8_t z, uint16_t speedIn, uint16_t speedOut) { if (z < p->_numZones) MD_PZone_setSpeedInOut(&p->_Z[z],speedIn, speedOut); }

#if ENA_SPRITE
  /**
  * Set data for user sprite effects (single zone).
  *
  * This method is used to set up user data needed so that the library can
  * display the sprite ahead of the entry/exit of text when the PA_SPRITE
  * animation type is selected.
  *
  * A sprite is made up of a number of frames that run sequentially to make
  * make the animation on the display. Once the animation reaches the last frame
  * it restarts from the first frame.
  *
  * A sprite is defined similarly to a character in the font table. Each byte
  * is a bit pattern defining a column in the sprite. A frame is xWidth bytes
  * in size and there are xFrames in the animation.
  *
  * \param z zone number.
  * \param inData pointer to the data table defining the entry sprite.
  * \param inWidth the width (in bytes) of each frame of the sprite.
  * \param inFrames the number of frames for the sprite.
  * \param outData pointer to the data table that is inWidth*InFrames in size.
  * \param outWidth the width (in bytes) of each frame of the sprite.
  * \param outFrames the number of frames for the sprite.
  * \return No return value.
  */
  inline void MD_Parola_setSpriteData2(MD_Parola_t *p,uint8_t z, const uint8_t *inData, uint8_t inWidth, uint8_t inFrames,
                                const uint8_t *outData, uint8_t outWidth, uint8_t outFrames)
  { if (z < p->_numZones) MD_PZone_setSpriteData(&p->_Z[z],inData, inWidth, inFrames, outData, outWidth, outFrames); }

  /**
  * Set data for user sprite effect (whole display).
  *
  * See the comments for single zone variant of this method.
  *
  * \param inData pointer to the data table defining the entry sprite.
  * \param inWidth the width (in bytes) of each frame of the sprite.
  * \param inFrames the number of frames for the sprite.
  * \param outData pointer to the data table that is inWidth*InFrames in size.
  * \param outWidth the width (in bytes) of each frame of the sprite.
  * \param outFrames the number of frames for the sprite.
  * \return No return value.
  */
  inline void MD_Parola_setSpriteData(MD_Parola_t *p,const uint8_t *inData, uint8_t inWidth, uint8_t inFrames,
                     const uint8_t *outData, uint8_t outWidth, uint8_t outFrames)
  { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_setSpriteData(&p->_Z[i],inData, inWidth, inFrames, outData, outWidth, outFrames); }

#endif

  /**
   * Set the text alignment for all zones.
   *
   * Text alignment is specified as one of the values in textPosition_t.
   *
   * \param ta  the required text alignment.
   * \return No return value.
   */
  void MD_Parola_setTextAlignment(MD_Parola_t *p,enum textPosition_t ta);

  /**
   * Set the text alignment for the specified zone.
   *
   * See comments for the 'all zones' variant of this method.
   *
   * \param z zone number.
   * \param ta  the required text alignment.
   * \return No return value.
   */
  inline void MD_Parola_setTextAlignment2(MD_Parola_t *p,uint8_t z, enum textPosition_t ta) { if (z < p->_numZones) MD_PZone_setTextAlignment(&p->_Z[z],ta); }

/**
 * Set the pointer to the text buffer for the specified zone.
 *
 * See comments for the single zone version of this method.
 *
 * \param z zone number.
 * \param pb  pointer to the text buffer to be used.
 * \return No return value.
 */
inline void MD_Parola_setTextBuffer(MD_Parola_t *p,uint8_t z, const char *pb) { if (z < p->_numZones) MD_PZone_setTextBuffer(&p->_Z[z],pb); }

/**
   * Set the pointer to the text buffer.
   *
   * Sets the text buffer to be a pointer to user data. The library does not allocate
   * any memory for the text message, rather it is the calling program that supplies
   * a pointer to a buffer. This reduces memory requirements and offers the flexibility
   * to keep a single buffer or rotate buffers with different messages, all under calling
   * program control, with no library limit to the size or numbers of buffers. The text
   * placed in the buffer must be properly terminated by the NUL ('\0') character or
   * processing will overrun the end of the message.
   *
   * This form of the method assumes one zone only.
   *
   * \param pb  pointer to the text buffer to be used.
   * \return No return value.
   */
  inline void MD_Parola_setTextBuffer1(MD_Parola_t *p,const char *pb) { MD_Parola_setTextBuffer(p,0, pb); }

  /**
   * Set the entry and exit text effects for all zones.
   *
   * The 'in' and 'out' text effects are specified using the textEffect_t enumerated
   * type. If no effect is required, NO_EFFECT should be specified. NO_EFFECT
   * is most useful when no exit effect is required (e.g., when DISSOLVE is used) and
   * the entry effect is sufficient.
   *
   * \param effectIn  the entry effect, one of the textEffect_t enumerated values.
   * \param effectOut the exit effect, one of the textEffect_t enumerated values.
   * \return No return value.
   */
  void MD_Parola_setTextEffect(MD_Parola_t *p,enum textEffect_t effectIn, enum textEffect_t effectOut);

  /**
   * Set the entry and exit text effects for a specific zone.
   *
   * See comments for the 'all zones' variant of this method.
   *
   * \param z     zone number.
   * \param effectIn  the entry effect, one of the textEffect_t enumerated values.
   * \param effectOut the exit effect, one of the textEffect_t enumerated values.
   * \return No return value.
   */
  void MD_Parola_setTextEffect2(MD_Parola_t *p,uint8_t z, enum textEffect_t effectIn, enum textEffect_t effectOut);

  /**
   * Set the display effect for the specified zone.
   *
   * The display effect is one of the zoneEffect_t types, and this will be set (true) or
   * reset (false) depending on the boolean value. The resulting zone display will be
   * modified as per the required effect.
   *
   * \param z   zone number.
   * \param b   set the value if true, reset the value if false
   * \param ze  the required text alignment.
   * \return No return value.
   */
  inline void MD_Parola_setZoneEffect(MD_Parola_t *p,uint8_t z, boolean b, enum zoneEffect_t ze) { if (z < p->_numZones) MD_PZone_setZoneEffect(&p->_Z[z],b, ze); }

  /**
  * Synchronize the start of zone animations.
  *
  * When zones are set up, the animation start time will default
  * to the set-up time. If several zones need to be animated
  * in synchronized fashion (eg, they are visually stacked vertically),
  * this method will ensure that all the zones start at the same instant.
  * The method should be invoked before the call to displayAnimate().
  *
  * \return No return value.
  */
  inline void MD_Parola_synchZoneStart(MD_Parola_t *p) { for (uint8_t i = 1; i < p->_numZones; i++) MD_PZone_setSynchTime(&p->_Z[i],MD_PZone_getSynchTime(&p->_Z[0])); }

/** @} */
  //--------------------------------------------------------------
  /** \name Support methods for fonts and characters.
   * @{
   */

  /**
   * Add a user defined character to the replacement list for all zones.
   *
   * Add a replacement characters to the user defined list. The character data must be
   * the same as for a single character in the font definition file. If a character is
   * specified with a code the same as an existing character the existing data will be
   * substituted for the new data. A character code of 0 ('\0') is illegal as this
   * denotes the end of string character for C++ and cannot be used in an actual string.
   *
   * The library does not copy the data definition but only retains a pointer to the data,
   * so any changes to the data storage in the calling program will be reflected into the
   * library. The data must also remain in scope while it is being used.
   *
   * \param code  code for the character data.
   * \param data  pointer to the character data.
   * \return No return value.
   */
  inline void MD_Parola_addChar(MD_Parola_t *p,uint16_t code, uint8_t *data) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_addChar(&p->_Z[i],code, data); }

  /**
   * Add a user defined character to the replacement specified zone.
   *
   * See the comments for the 'all zones' variant of this method
   *
   * \param z   zone specified
   * \param code  ASCII code for the character data.
   * \param data  pointer to the character data.
   * \return true of the character was inserted in the substitution list.
   */
  inline bool MD_Parola_addChar2(MD_Parola_t *p,uint8_t z, uint16_t code, uint8_t *data) { return(z < p->_numZones ? MD_PZone_addChar(&p->_Z[z],code, data) : false); }

  /**
   * Delete a user defined character to the replacement list for all zones.
   *
   * Delete a reference to a replacement character in the user defined list.
   *
   * \param code  ASCII code for the character data.
   * \return No return value.
   */
  inline void MD_Parola_delChar(MD_Parola_t *p,uint16_t code) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_delChar(&p->_Z[i],code); }

  /**
   * Delete a user defined character to the replacement list for the specified zone.
   *
   * See the comments for the 'all zones' variant of this method.
   *
   * \param z   zone specified
   * \param code  ASCII code for the character data.
   * \return true of the character was found in the substitution list.
   */
  inline bool MD_Parola_delChar2(MD_Parola_t *p,uint8_t z, uint16_t code) { return(z < p->_numZones ? MD_PZone_delChar(&p->_Z[z],code) : false); }

  /**
   * Get the display font for specified zone.
   *
   * Get the current font table pointer for the specified zone.
   *
   * \param z specified zone.
   * \return Pointer to the font definition used.
   */
  inline fontType_t* MD_Parola_getFont(MD_Parola_t *p,uint8_t z) { if (z < p->_numZones) return (MD_PZone_getZoneFont(&p->_Z[z])); else return(NULL); };

  /**
   * Get the display font for single zone display.
   *
   * Return the current font table pointer for single zone displays.
   *
   * \return Pointer to the font definition used.
   */
  inline fontType_t* MD_Parola_getFont1(MD_Parola_t *p) { return(MD_Parola_getFont(p,0)); };

  /**
   * Set the display font for all zones.
   *
   * Set the display font to a user defined font table. This can be created using the
   * MD_MAX72xx font builder (refer to documentation for the tool and the MD_MAX72xx library).
   * Passing nullptr resets to the library default font.
   *
   * \param fontDef Pointer to the font definition to be used.
   * \return No return value.
   */
  inline void MD_Parola_setFont(MD_Parola_t *p,fontType_t *fontDef) { for (uint8_t i = 0; i < p->_numZones; i++) MD_PZone_setZoneFont(&p->_Z[i],fontDef); }

  /**
   * Set the display font for a specific zone.
   *
   * Set the display font to a user defined font table. This can be created using the
   * MD_MAX72xx font builder (refer to documentation for the tool and the MD_MAX72xx library).
   * Passing nullptr resets to the library default font.
   *
   * \param z   specified zone.
   * \param fontDef Pointer to the font definition to be used.
   * \return No return value.
   */
  inline void MD_Parola_setFont2(MD_Parola_t *p,uint8_t z, fontType_t *fontDef) { if (z < p->_numZones) MD_PZone_setZoneFont(&p->_Z[z],fontDef); }

  /** @} */

#if ENA_GRAPHICS
  //--------------------------------------------------------------
  /** \name Support methods for graphics.
  * @{
  */
  /**
  * Get a pointer to the instantiated graphics object.
  *
  * Provides a pointer to the MD_MAX72XX object to allow access to
  * the display graphics functions.
  *
  * \return Pointer to the MD_MAX72xx object used by the library.
  */
  inline MD_MAX72XX_t *MD_Parola_getGraphicObject(MD_Parola_t *p) { return(&p->_D); }

  /**
  * Get the start and end column numbers for the whole display.
  *
  * Returns the start and end column numbers for the matrix display.
  * This retains consistency between user code and library.
  *
  * \param startColumn the by-reference parameter that will hold the return value for the start column.
  * \param endColumn the by-reference parameter that will hold the return value for the end column.
  * \return The start and end columns in the by-reference parameters.
  */
  inline void MD_Parola_getDisplayExtent(MD_Parola_t *p,uint16_t *startColumn, uint16_t *endColumn) { *startColumn = ZONE_START_COL(0); *endColumn = ZONE_END_COL(p->_numModules-1); }

  /**
  * Get the start and end column numbers for the required zone.
  *
  * Returns the start and end column numbers for the zone display.
  * This retains consistency between user code and library.
  * The by-reference parameters remain unchanged if an invalid zone
  * number is requested.
  *
  * \param z zone specified
  * \param startColumn the by-reference parameter that will hold the return value for the start column.
  * \param endColumn the by-reference parameter that will hold the return value for the end column.
  * \return The start and end columns in the by-reference parameters.
  */
  inline void MD_Parola_getDisplayExtent2(MD_Parola_t *p,uint8_t z, uint16_t *startColumn, uint16_t *endColumn) { if (z < p->_numZones) MD_PZone_getZoneExtent(&p->_Z[z],startColumn, endColumn); }

/**
* Get the start and end column numbers for the text displayed.
*
* Returns the start and end column numbers for the text displayed in the zone.
* This retains consistency between user code and library.
*
* \param z zone specified
* \param startColumn the by-reference parameter that will hold the return value for the start column.
* \param endColumn the by-reference parameter that will hold the return value for the end column.
* \return The start and end columns in the by-reference parameters.
*/
inline void MD_Parola_getTextExtent(MD_Parola_t *p,uint8_t z, uint16_t *startColumn, uint16_t *endColumn) { if (z < p->_numZones) MD_PZone_getTextExtent(&p->_Z[z],startColumn, endColumn); }

  /**
  * Get the start and end column numbers for the text displayed.
  *
  * Returns the start and end column numbers for the text displayed in zone 0.
  * This can be used for simplicity when the display is a single zone.
  *
  * \param startColumn the by-reference parameter that will hold the return value for the start column.
  * \param endColumn the by-reference parameter that will hold the return value for the end column.
  * \return The start and end columns in the by-reference parameters.
  */
  inline void MD_Parola_getTextExtent1(MD_Parola_t *p,uint16_t *startColumn, uint16_t *endColumn) { MD_Parola_getTextExtent(p,0, startColumn, endColumn); }

  /** @} */
#endif

  //--------------------------------------------------------------
  /** \name Support methods for Print class extension.
  * @{
  */

/**
* Write a nul terminated string to the output display.
*
* Display a nul terminated string when given a pointer to the char array.
* Invokes an animation using PA_PRINT with all other settings (alignment,
* speed, etc) taken from current defaults.
* This method also invokes the animation for the print and returns when that has
* finished, so it blocks while the printing is happening, which should be at least
* one iteration of the wait loop.
*
* \param str  Pointer to the nul terminated char array.
* \return the number of characters written.
*/
size_t MD_Parola_write1(MD_Parola_t *p,const char *str);

/**
  * Write a single character to the output display
  *
  * Display a character when given the ASCII code for it. The character is
  * converted to a string and the string printing function invoked.
  * The LED display is designed for string based output, so it does not make much
  * sense to do this. Creating the short string is a consistent way to way to handle
  * single the character.
  *
  * \param c  ASCII code for the character to write.
  * \return the number of characters written.
  */
inline size_t MD_Parola_write(MD_Parola_t *p,uint8_t c) { char sz[2]; sz[0] = c; sz[1] = '\0'; MD_Parola_write1(p,sz); return(1); }

  /**
  * Write a character buffer to the output display.
  *
  * Display a non-nul terminated string given a pointer to the buffer and
  * the size of the buffer. The buffer is turned into a nul terminated string
  * and the simple write() method is invoked. Memory is allocated and freed
  * in this method to copy the string.
  *
  * \param buffer Pointer to the data buffer.
  * \param size The number of bytes to write.
  * \return the number of bytes written.
  */
  size_t MD_Parola_write2(MD_Parola_t *p,const uint8_t *buffer, size_t size);

  /** @} */

