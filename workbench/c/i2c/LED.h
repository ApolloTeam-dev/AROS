/*========================================================================*
 |  File: LED.h                                        Date: 17 Aug 1997  |
 *------------------------------------------------------------------------*
 |   Subroutines for controlling an SAA 1064 LED-display on the I²C-Bus.  |
 |                                                                        |
 *========================================================================*/

ULONG SendLed( STRPTR strMsg );
ULONG LedScroll( STRPTR strMsg, WORD wTiming );
ULONG LedNumber( LONG lNumber );
                                               
