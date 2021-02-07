/*========================================================================*
 |  File: LEDgame.c                                    Date: 23 Aug 1997  |
 *------------------------------------------------------------------------*
 |           Fireflies dashing about in a SAA 1064 LED-display            |
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


#define SAA1064 0x70    /* may be jumpered as 0x72, 0x74, 0x76, too */
#define MAXDASHES 20    /* how many fireflies are allowed? */
#define INVISIBLE 99    /* magic value */

/* bit numbers in the LED digits:
    ___
  5| 0 |1              numbering of the 24 corners:
   |___|
  4| 6 |2              0  3   6  9   12 15   18 21
   |___|.              1  4   7 10   13 16   19 22
     3   7             2  5   8 11   14 17   20 23
*/

/* table: which of the 32 display segments connects two given corners? */
BYTE baaSegBitNo[ 24 ][ 24 ];

/* table: which are the (up to 4) neighbours of a given corner? */
struct 
{
  BYTE bCount;
  BYTE baWho[ 4 ];
} Neighbours[ 24 ];



/* construct the topology tables declared above: */
void TableSetup()
{
  /* table: which corners are connected by the first 7 segments? */
  BYTE baCorners[ 14 ] = { 0,3, 3,4, 4,5, 5,2, 2,1, 1,0, 1,4 };
  WORD wCornerA, wCornerB;
  WORD wDigit, wSegment;
  WORD wColumn, wLine;
  WORD wCount;

  for( wCornerA = 0; wCornerA < 24; wCornerA++ )
    for( wCornerB = 0; wCornerB < 24; wCornerB++ )
      baaSegBitNo[ wCornerA ][ wCornerB ] = INVISIBLE;
  for( wDigit = 0; wDigit < 4; wDigit++ )
    for( wSegment = 0; wSegment < 7; wSegment++ )
      {
        wCornerA = baCorners[ 2*wSegment ]   + 6*wDigit;
        wCornerB = baCorners[ 2*wSegment+1 ] + 6*wDigit;
        baaSegBitNo[ wCornerA ][ wCornerB ] =       /* symmetric table */
        baaSegBitNo[ wCornerB ][ wCornerA ] = wSegment + 8*wDigit;
      }
  for( wColumn = 0; wColumn < 8; wColumn++ )
    for( wLine = 0; wLine < 3; wLine++ )
      {
        wCornerA = wLine + 3*wColumn;
        wCount = 0;
        if( wColumn>0 )
          Neighbours[ wCornerA ].baWho[ wCount++ ] = wCornerA - 3;
        if( wColumn<7 )
          Neighbours[ wCornerA ].baWho[ wCount++ ] = wCornerA + 3;
        if( wLine>0 )
          Neighbours[ wCornerA ].baWho[ wCount++ ] = wCornerA - 1;
        if( wLine<2 )
          Neighbours[ wCornerA ].baWho[ wCount++ ] = wCornerA + 1;
        Neighbours[ wCornerA ].bCount = wCount;
      }                
}


void FireFlies( WORD wCount, BOOL boSkip, BOOL boChase, WORD wDelay )
/* boSkip==TRUE: skip invisible states, e. g. between corners 3 and 6 */
/* boChase==TRUE: connect them all together as a snake */
{
  WORD waHead[ MAXDASHES ], waTail[ MAXDASHES ];
  WORD wIndex, wSelect;
  WORD wCorner;
  WORD wBitNo;
  WORD wMove;
  BYTE baBuf[] = { 0, 0x27, 0,0,0,0 };
  LONG lResult;

  if( wCount > MAXDASHES )
    wCount = MAXDASHES;
  for( wIndex = 0; wIndex < wCount; wIndex++ )
    waHead[ wIndex ] = waTail[ wIndex ] = wIndex;
  wMove = boChase ? 1 : wCount;
  srand( time( NULL ) );
  while( TRUE )                 /* use Ctrl-C to exit */
    {
      /* perform chase */
      if( boChase )
        for( wIndex = wCount-1; wIndex > 0; wIndex-- )
          {
            waTail[ wIndex ] = waTail[ wIndex-1 ];
            waHead[ wIndex ] = waHead[ wIndex-1 ];
          }
      /* find a new direction for the head(s) */
      for( wIndex = 0; wIndex < wMove; wIndex++ )
        do         
          {
            wCorner = waHead[ wIndex ];
            wSelect = rand() % Neighbours[ wCorner ].bCount;
            waHead[ wIndex ] = Neighbours[ wCorner ].baWho[ wSelect ];
            if( waHead[ wIndex ] == waTail[ wIndex ] )
              {                 /* no U-turns allowed */
                wSelect = ( wSelect + 1 ) % Neighbours[ wCorner ].bCount;
                waHead[ wIndex ] = Neighbours[ wCorner ].baWho[ wSelect ];
              }
            waTail[ wIndex ] = wCorner;
          }
        while( boSkip && baaSegBitNo[ waHead[ wIndex ] ][ waTail[ wIndex ] ] == INVISIBLE );
      /* display them */
      for( wIndex = 0; wIndex < 4; wIndex++ )
        baBuf[ 2+wIndex ] = 0;
      for( wIndex = 0; wIndex < wCount; wIndex++ )
        {
          wBitNo = baaSegBitNo[ waHead[ wIndex ] ][ waTail[ wIndex ] ];
          if( wBitNo != INVISIBLE )
            baBuf[ 2+wBitNo / 8 ] |= (1 << (wBitNo % 8));
        }
      lResult = SendI2C( SAA1064, 6, baBuf );
      if( (lResult & 0xFF) == 0 )
        {
          printf( "I²C bus: error 0x%06lx, %s\n", lResult, I2CErrText( lResult ) );
          exit( 10 );
        }
      Delay( wDelay );
      printf( "\r" );
      fflush( stdout );         /* force GNU C to check for Ctrl-C :-( */
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
  SendI2C( SAA1064, 2, "\0\0" );
  LibClose( &I2C_Base );
  LibClose( (struct Library **)&DOSBase );
}


int main( int argc, char *argv[] )
{
  WORD wDelay = 4;
  WORD wCount = 1;
  BOOL boChase = FALSE;
  BOOL boSkip = TRUE;
                       
  DOSBase  = NULL;
  I2C_Base = NULL;
  atexit( CleanUp );            /* close libraries upon Ctrl-C, too */
  I2C_Base = LibOpen( "i2c.library", 39 );
  DOSBase  = (struct DosLibrary *)LibOpen( "dos.library", 0 );
  
  TableSetup();
  while( --argc )
    {
      if( strcmp( argv[ argc ], "-h" ) == 0 )
        boSkip = FALSE;
      else if( strcmp( argv[ argc ], "-c" ) == 0 )
        boChase = TRUE;
      else if( strncmp( argv[ argc ], "-d", 2 ) == 0 )
        wDelay = atoi( argv[ argc ] + 2 );
      else if( isdigit( *argv[ argc ] ) )
        wCount = atoi( argv[ argc ] );
    }
  FireFlies( wCount, boSkip, boChase, wDelay );
  return 0;
}

