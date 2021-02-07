/*========================================================================*
 |  File: LED.c                                        Date: 17 Aug 1997  |
 *------------------------------------------------------------------------*
 |   Subroutines for controlling an SAA 1064 LED-display on the I²C-Bus.  |
 |                                                                        |
 *========================================================================*/

#include <string.h>
#include <stdio.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/i2c.h>
#include "LED.h"


#define SAA1064 0x70    /* may be jumpered as 0x72, 0x74, 0x76, too */
#define CRAP    0x49    /* bit pattern for undefined characters */

/* bit numbers in the LED digits:
    ___
  5| 0 |1
   |___|
  4| 6 |2
   |___|.
     3   7
*/

UBYTE ubaLedCode[96] =          /* bit patterns for ' ' through '~' */
{
  0x00, 0x82, 0x22, CRAP, CRAP, CRAP, CRAP, 0x20,       /*   !"#$%&'  */
  0x39, 0x0F, CRAP, 0x70, 0x80, 0x40, 0x80, 0x52,       /*  ()*+,-./  */
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,       /*  01234567  */
  0x7F, 0x6F, CRAP, CRAP, 0x58, 0x48, 0x4C, 0x53,       /*  89:;<=>?  */
  CRAP, 0x77, 0x7F, 0x39, 0x3F, 0x79, 0x71, 0x3D,       /*  @ABCDEFG  */
  0x76, 0x06, 0x0E, 0x75, 0x38, 0x79, 0x37, 0x3F,       /*  HIJKLMNO  */
  0x73, 0x67, 0x77, 0x6D, 0x31, 0x3E, 0x3E, 0x4F,       /*  PQRSTUVW  */
  0x76, 0x72, 0x5B, 0x39, 0x64, 0x0F, 0x23, 0x08,       /*  XYZ[\]^_  */
  0x02, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F,       /*  `abcdefg  */
  0x74, 0x04, 0x0C, 0x75, 0x06, 0x79, 0x54, 0x5C,       /*  hijklmno  */
  0x73, 0x67, 0x50, 0x6D, 0x70, 0x1C, 0x1C, 0x4F,       /*  pqrstuvw  */
  0x76, 0x66, 0x5B, 0x39, 0x30, 0x0F, 0x01, CRAP        /*  xyz{|}~   */
};


ULONG SendLed( STRPTR strMsg )
/* returns 0 if OK, else the i2c.lib error code */
{
  BYTE baBuf[6] = { 0, 0x17, 0, 0, 0, 0 };
  WORD wIdx;
  ULONG ulResult;
                                                  
  for( wIdx = 2; wIdx<6 && *strMsg; wIdx++, strMsg++ )
    {
      if( *strMsg<' ' || *strMsg>'~' )
        baBuf[wIdx] = CRAP;
      else
        baBuf[wIdx] = ubaLedCode[*strMsg - ' '];
      if( strMsg[1] == '.' )    /* join '.' to the preceding character */
        {
          baBuf[wIdx] |= 0x80;
          strMsg++;
        }
    }
  ulResult = SendI2C( SAA1064, 6, baBuf );
  if( ulResult & 0x0f )
    return 0;
  else
    return ulResult;
}


ULONG LedScroll( STRPTR strMsg, WORD wTiming )
{ 
  ULONG ulResult;

  ulResult = SendLed( strMsg );
  while( strlen( strMsg ) > 4 && ulResult == 0 )
    {
      ulResult = SendLed( ++strMsg );
      Delay( wTiming );
    } 
  return ulResult;
}


ULONG LedNumber( LONG lNumber )
{                      
  char caBuf[10];

  sprintf( caBuf, "%4ld", lNumber );
  return LedScroll( caBuf, 10 );
}


