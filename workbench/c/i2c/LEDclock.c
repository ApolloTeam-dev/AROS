/*========================================================================*
 |  Datei: LEDclock.c                                 Datum: 20 Aug 1997  |
 *------------------------------------------------------------------------*
 |      Systemzeit abfragen und mittels SAA 1064 auf LEDs anzeigen.       |
 |                                                                        |
 *========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/i2c.h>
#include <libraries/i2c.h>
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


int main(int argc, char *argv[])
{
  struct DateStamp dsJetzt;
  BOOL boDot = FALSE;
  char caBuf[5];

  DOSBase  = NULL;
  I2C_Base = NULL;
  atexit( CleanUp );            /* Librarys bei Ctrl-C mitschlieﬂen */
  I2C_Base = LibOpen( "i2c.library", 39 );
  DOSBase  = (struct DosLibrary *)LibOpen( "dos.library", 0 );
  printf( "Digitaluhr am I≤C-Bus. Abbruch mit Ctrl-C\n" );
  while( TRUE )                 /* Tja, Ctrl-C benutzen :^) */
    {
      DateStamp( &dsJetzt );
      sprintf( caBuf, boDot ? "%2ld.%02ld" : "%2ld%02ld",
          dsJetzt.ds_Minute / 60, dsJetzt.ds_Minute % 60 );
      SendLed( caBuf );
      Delay( 50 );              /* 1 sec warten */
      boDot = !boDot;
      printf( "\r" );
      fflush( stdout );         /* Ctrl-C-Abfrage erzwingen */
    }
  return 0;
}

