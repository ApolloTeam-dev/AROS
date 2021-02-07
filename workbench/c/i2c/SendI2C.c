/* SendI2C.c:
 *   Send bytes supplied in the command line arguments over the I�C bus,
 *   using i2c.library.  Usage:
 * SendI2C  <bus address>  <data>  [<data>  [...] ]
 * where <data> are an arbitrary number of hex digits, in groups of
 * 1, 2, 3 or 4 byte.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/i2c.h>
#include <libraries/i2c.h>

struct Library *I2C_Base;

int main(int argc, char *argv[])
    {
    int result=0;
    UBYTE adr, i2cdata[ 1000 ];
    UWORD i, j, bytes=0;
    ULONG l, err;

    if( argc<3 )
        {
        printf( "Usage: \e[2m%s <addr> <data>\e[0m, ", argv[0] );
        printf( "e.g. %s 70 0027 065b 4f66\n", argv[0] );
        return 5;
        }
    adr = strtoul( argv[1], NULL, 16 );
    for( i=2; i<argc; i++ )
        {
        j = strlen(argv[ i ]);
        if( j & 1 )
            {
            if( j>1 )
                printf("Warning: odd-length hex number %s\n", argv[i]);
            j++;
            }
        l = strtoul( argv[ i ], NULL, 16 );
        j *= 4;
        while( j>0 )
            {
            j -= 8;
            i2cdata[ bytes++ ] = (l >> j) & 0xff;
            }
        }

    I2C_Base = OpenLibrary( "i2c.library", 39 );
    if( !I2C_Base )
        {
        printf( "Can't open i2c.library V39+\n" );
        return 10;
        }
    err = SendI2C(adr, bytes, i2cdata);
    if( (err & 0xff)==0 )
        {
        printf("I�C failure: \e[1m%s\e[0m\n", I2CErrText(err) );
        result = 10;
        }
    CloseLibrary(I2C_Base);

    return result;
    }

