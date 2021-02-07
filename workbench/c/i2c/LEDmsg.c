/*========================================================================*
 |  File: LEDmsg.c                                     Date: 21 Aug 1997  |
 *------------------------------------------------------------------------*
 |   Demo of sending little pieces of text to the SAA 1064 LED-display    |
 |                                                                        |
 *========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/i2c.h>
#include "LED.h"


struct Library *I2C_Base;


void LibClose( struct Library **ppLib )
{
  if( *ppLib )
  {
    CloseLibrary( *ppLib );
    *ppLib = NULL;
  }
}


struct Library* LibOpen( STRPTR strName, LONG lVersion )
{
  struct Library* pLib;

  pLib = OpenLibrary( strName, lVersion );
  if( !pLib )
    {
      printf( "Can't open %s", strName );
      printf( lVersion ? " V%ld+\n" : "\n", lVersion );
      exit( 10 );
    }
  else
    return pLib;
}


void CleanUp()
{
  SendLed( "    " );
  LibClose( &I2C_Base );
  LibClose( (struct Library **)&DOSBase );
}


int main( int argc, char *argv[] )
{
  char caBuf[100];
  char* pcMsg = "";
  LONG lDelay = 10;
  LONG lResult, lCount;

  DOSBase  = NULL;
  I2C_Base = NULL;
  atexit( CleanUp );            /* Librarys bei Ctrl-C mitschließen */
  I2C_Base = LibOpen( "i2c.library", 39 );
  DOSBase  = (struct DosLibrary *)LibOpen( "dos.library", 0 );
  if( argc > 1 )
    {
      pcMsg = argv[1];
      if( argc > 2 && atol( argv[2] )>0 )
        lDelay = atol( argv[2] );
    }
  else
    {
      srand( time( NULL ) );
      if( rand() % 5 == 0 )
        for( lCount = 26; lCount > -20; lCount-- )
          {
            LedNumber( lCount );
            Delay( 5 );
          }
      else
        switch( rand() % 4 )
          {
            case 0: pcMsg = "HALLO - PAUSE - HILFE ALF";
            case 1: pcMsg = "buS Error";
            case 2: pcMsg = "dEAd AGAIN ";
            case 3: pcMsg = "HELLO JEAN-LUC . . .";
          }
    }
  printf( "sending \"%s\"\n", pcMsg );
  sprintf( caBuf, "   %s    ", pcMsg );
  lResult = LedScroll( caBuf, lDelay );
  if( lResult )
    {
      printf( "I²C bus: error 0x%06lx, %s\n", lResult, I2CErrText( lResult ) );
      return 10;
    }
  return 0;
}

