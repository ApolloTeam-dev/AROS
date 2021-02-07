/*========================================================================*
 |  File: Dial.c                                       Date: 02 Aug 1998  |
 *------------------------------------------------------------------------*
 |                Use the PCD3311/3312 to produce dial tones              |
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

/* table: recognized dial keys on the command line */
/* (send code 0x10 + offset to hear them) */
char caKeys[] = "0123456789ABCD*#";



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



int main( int argc, char *argv[] )
    {
    int i;
    long err;
    char *s, *t;
    char c;

    if( argc < 2 )
        {
        printf( "Usage: \e[2m%s <keys>\e[0m, ", argv[0] );
        printf( "e.g. %s 01910 *66257# ABCD\n", argv[0] );
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
        while( *s )
            {
            t = strchr( caKeys, toupper( *s ) );
            if( t != NULL )
                {
                c = (t - caKeys) + 0x10;
                SendI2C( PCD3312, 1, &c );
                printf( "%c", *s );
                }
            else
                printf( "?" );
            Delay( 8 );
            err = SendI2C( PCD3312, 1, "\0" );
            if( (err & 0xff)==0 )
                {
                printf("\nI²C failure: \e[1m%s\e[0m\n", I2CErrText(err) );
                exit( 10 );
                }
            fflush( stdout );   /* force GNU C to check for Ctrl-C :-( */
            s++;
            }
        printf( " " );
        if( ++i == argc )
            break;
        Delay( 25 );
        }
    printf( "\n" );

    return 0;
    }
