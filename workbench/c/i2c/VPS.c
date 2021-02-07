/*========================================================================*
 |  Datei: VPS.c                                      Datum: 17 Aug 1997  |
 *------------------------------------------------------------------------*
 |  Überwacht das ausgestrahlte VPS-Signal mittels SAA 4700 am I²C-Bus.   |
 |  VPS-Zeit wird auch gleich mittels SAA 1064 auf LEDs angezeigt.        |
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


#define SAA4700 0x20

struct VpsInfo
    {
    BYTE bBereich;                /* 0..3 */
    BYTE bLand;                   /* 0..15 */
    BYTE bSender;                 /* 0..63 */
    STRPTR strName;
    };


/* globale Tabellen: */

STRPTR straAudio[4] =
    {
    "?", "M", "S", "2"
    };

STRPTR straFsk[4] =
    {
    "X0", "JF", "X2", "X3"
    };

#define ANZSENDER 33

struct VpsInfo viaSender[ ANZSENDER ] =
    {
    { 3, 13,  1, "ARD" },
    { 3, 13,  2, "ZDF" },
    { 3, 13,  3, "ARD+ZDF" },
    { 3, 13,  4, "ARD/N" },
    { 3, 13,  7, "3sat" },
    { 3, 13,  8, "Phoenix" },
    { 3, 13,  9, "Kinderkanal" },
    { 3, 13, 11, "Bayern 3" },
    { 3, 13, 15, "Hessen" },
    { 3, 13, 20, "Nord 3" },
    { 3, 13, 33, "Südwest BW" },
    { 3, 13, 36, "Südwest RP" },
    { 3, 13, 37, "ARD/W" },
    { 3, 13, 38, "WDR" },
    { 3, 13, 62, "MDR" },
    { 2, 13,  2, "ORB" },
    { 2, 13,  5, "arte" },
    { 2, 13, 10, "Super RTL" },
    { 2, 13, 12, "n-tv" },
    { 2, 13, 13, "DSF" },
    { 2, 13, 14, "Vox" },
    { 2, 13, 15, "RTL 2" },
    { 2, 13, 17, "Eurosport" },
    { 2, 13, 18, "Kabel 1" },
    { 2, 13, 20, "Pro 7" },
    { 2, 13, 43, "RTL" },
    { 2, 13, 44, "Premiere" },
    { 2, 13, 57, "Sat 1" },
    { 2, 13, 58, "tm 3" },
    { 2,  4,  2, "Teleclub" },
    { 1, 13,  4, "BR alpha" },
    { 1, 13, 63, "H·O·T" },
    { 1, 14, 62, "Nickelodeon" },
    };


void LedShow( ULONG ulCode )
/* VPS-Zeit bzw. Sondercodes am LED-Display anzeigen */
    {
    WORD wTag, wMonat, wStunde, wMinute;
    BYTE baBuf[5];

    if( ulCode == ULONG_MAX )
        SendLed( "----" );
    else
        {
        wTag    = (ulCode >> 25) % 32;
        wMonat  = (ulCode >> 21) % 16;
        wStunde = (ulCode >> 16) % 32;
        wMinute = (ulCode >> 10) % 64;
        switch( wStunde )
            {
            case 31:
                SendLed( "S---" );  /* Statuscode */
                break;
            case 30:
                SendLed( "L---" );  /* Leercode */
                break;
            case 29:
                SendLed( "P---" );  /* Pause */
                break;
            default:
                sprintf( baBuf, "%2d%02d", wStunde, wMinute );
                SendLed( baBuf );
            }
        }
    }


void FileShow( ULONG ulCode0, ULONG ulCode1, FILE* Datei )
    {
    WORD wTag, wMonat, wStunde, wMinute;
    WORD wBereich, wLand, wSender, wAudio, wFsk;
    WORD wLauf;
    struct DateStamp dsJetzt;

    if( !Datei )
        return;
    DateStamp( &dsJetzt );
    fprintf( Datei, "%2ld:%02ld:%02ld - ", dsJetzt.ds_Minute / 60,
          dsJetzt.ds_Minute % 60, dsJetzt.ds_Tick / TICKS_PER_SECOND );
    if( ulCode0 == ULONG_MAX )
        fprintf( Datei, "kein VPS-Signal\n" );
    else
        {
        wBereich =  ulCode0 >> 30;
        wTag     = (ulCode0 >> 25) % 32;
        wMonat   = (ulCode0 >> 21) % 16;
        wStunde  = (ulCode0 >> 16) % 32;
        wMinute  = (ulCode0 >> 10) % 64;
        wLand    = (ulCode0 >>  6) % 16;
        wSender  =  ulCode0        % 64;
        wAudio   =  ulCode1 >> 30;
        wFsk     = (ulCode1 >> 28) %  4;
        switch( wStunde )
            {
            case 31:
                fprintf( Datei, "%-20s", "Statuscode" );
                break;
            case 30:
                fprintf( Datei, "%-20s", "Leercode" );
                break;
            case 29:
                fprintf( Datei, "%-20s", "Pause" );
                break;
            default:
                fprintf( Datei, "VPS %2d:%02d am %02d.%02d. ",
                    wStunde, wMinute, wTag, wMonat );
            }
        fprintf( Datei, "%s, %s, ", straFsk[wFsk], straAudio[wAudio] );
        fprintf( Datei, "A%d/L%02d/P%02d", wBereich, wLand, wSender );
        for( wLauf = 0; wLauf < ANZSENDER; wLauf++ )
            if( viaSender[wLauf].bBereich == wBereich
             && viaSender[wLauf].bLand    == wLand
             && viaSender[wLauf].bSender  == wSender )
                break;
        if( wLauf < ANZSENDER )
            fprintf( Datei, ": %s", viaSender[wLauf].strName );
        fprintf( Datei, "\n" );
        }
    }


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
    FILE *Datei;
    STRPTR strName;
    WORD wRetry;
    ULONG laVpsData[2];
    ULONG lLastData, lLastValid;
    ULONG lError, lLastError;

    DOSBase  = NULL;
    I2C_Base = NULL;
    atexit( CleanUp );            /* Librarys bei Ctrl-C mitschließen */
    I2C_Base = LibOpen( "i2c.library", 39 );
    DOSBase  = (struct DosLibrary *)LibOpen( "dos.library", 0 );
    printf( "VPS-Signalüberwachung. Abbruch mit Ctrl-C\n" );
    if( argc<2 )
        strName = "RAM:VPSdatei";
    else
        strName = argv[1];
    printf( "Ausgabedatei: \"%s\"\n", strName );
    lLastData = 0;
    lLastValid = 0;
    lLastError = 0;
    while( TRUE )                 /* Tja, Ctrl-C benutzen :^) */
        {
        wRetry = 25;  /* erst nach 2 * 1/2 Sekunde 'kein VPS-Signal' aufgeben */
        do
            {
            lError = ReceiveI2C( SAA4700, 5, (char *)laVpsData );
            if( (lError & 0xFF) == 0 )
                laVpsData[0] = ULONG_MAX;  /* entspricht "kein VPS-Empfang" */
            Delay(1);             /* 1/50 sec warten */
            }
        while( laVpsData[0] == ULONG_MAX && wRetry-- );
        if( lError != lLastError)
            printf( "I²C-Bus: %s\n", I2CErrText( lError) );
        lLastError = lError;
        if( laVpsData[0] == lLastData && laVpsData[0] != lLastValid )
            {
            lLastValid = laVpsData[0];
            FileShow( laVpsData[0], laVpsData[1], stdout );
            Datei = fopen( strName, "a" );
            FileShow( laVpsData[0], laVpsData[1], Datei );
            fclose( Datei );
            SendLed( "    ");     /* Display kurz blinken lassen */
            Delay( 10 );
            LedShow( laVpsData[0] );
            }
        Delay( 5 );               /* 10 Meßwerte pro Sekunde reichen */
        lLastData = laVpsData[0];
        printf( "\r" );
        fflush( stdout );         /* Ctrl-C-Abfrage erzwingen */
        }

    return 0;
    }

