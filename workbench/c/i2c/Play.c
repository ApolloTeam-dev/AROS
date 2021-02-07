/*========================================================================*
 |  File: Play.c                                       Date: 22 Aug 1998  |
 *------------------------------------------------------------------------*
 |                Use the PCD3311/3312 to play melodies                   |
 |                                                                        |
 *========================================================================*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/i2c.h>

struct Library *I2C_Base;


#define PCD3312 0x48    /* may be jumpered as 0x4A, too */

/* table: names of the notes and values to produce them */
char caNotes[] = "DefFgGaAbcCdDefFgGaAbcCdD";
BYTE baCodes[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                   0x38, 0x39, 0x3a, 0x29, 0x3b, 0x3c, 0x3d, 0x0e,
                   0x3e, 0x2c, 0x3f, 0x04, 0x05, 0x25, 0x2f, 0x06,
                   0x07 };




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
    SendI2C( PCD3312, 1, "\0" );
    LibClose( &I2C_Base );
    LibClose( (struct Library **)&DOSBase );
    }


    /* Takes a note name A through G and returns a code for it. */
    /* If sharp != 0, assume it's raised by a halftone. */
    /* If far == 0, return the code for the nearest note, else for */
    /* the "other one". (We have two octaves available.) */
    /* P means "pause" and returns the appropriate value of 0. */
    /* H is accepted as an alias for B. */
    /* All other values return 0x10, which is a dial tone. :-7 */
int Translate( char c, int sharp, int far )
    {
    static int pos = 12;    /* start from the middle of the range */
    int pos0, pos1;
    char *s;
    #define ABSDIFF( a, b ) ((a) > (b) ? (a)-(b) : (b)-(a))

    if( toupper( c ) == 'P' )
        return 0;
    if( toupper( c ) == 'H' )
        c = 'B';
    c = sharp ? toupper( c ) : tolower( c );
    s = strchr( caNotes, c );
    if( s == NULL )
        return 0x10;
    pos0 = s - caNotes;
    pos1 = pos0 + 12;
    /* Two possible choices. Which one is nearer? */
    if( ABSDIFF( pos, pos0 ) < ABSDIFF( pos, pos1 ) )
        pos = far ? pos1 : pos0;
    else
        pos = far ? pos0 : pos1;

    return baCodes[ pos ];
    }



int main( int argc, char *argv[] )
    {
    int i;
    int base = 8;              /* time base for playback */
    long err;
    char *s;
    char c;

    if( argc < 2 )
        {
        printf( "Usage: \e[2m%s [timebase] <notes>\e[0m, ", argv[0] );
        printf( "e.g. %s 16 f# d e Aa pp a E f# dd\n", argv[0] );
        return 5;
        }

    DOSBase  = NULL;
    I2C_Base = NULL;
    atexit( CleanUp );          /* close libraries upon Ctrl-C, too */
    I2C_Base = LibOpen( "i2c.library", 39 );
    DOSBase  = (struct DosLibrary *)LibOpen( "dos.library", 0 );

    i=1;
    while( TRUE )
        {
        s = argv[ i ];
        if( isdigit( *s ) )     /* read a new timebase */
            {
            base = atoi( s );
            continue;
            }
        while( *s )             /* otherwise play notes (or pauses) */
            {
            c = Translate( *s, s[ 1 ] == '#', isupper( *s ) );
            if( s[ 1 ] == '#' )
                s++;
            err = SendI2C( PCD3312, 1, &c );
            if( (err & 0xff)==0 )
                {
                printf("\nI²C failure: \e[1m%s\e[0m\n", I2CErrText(err) );
                exit( 10 );
                }
            Delay( base );
            s++;
            if( !*s )
                SendI2C( PCD3312, 1, "\0" );
            printf( "\r" );
            fflush( stdout );   /* force GNU C to check for Ctrl-C :-( */
            }
        if( ++i == argc )
            break;
        }

    return 0;
    }
