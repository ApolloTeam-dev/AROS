/*========================================================================*\
 |  File: I2CScan.c                                    Date: 24 Aug 1998  |
 *------------------------------------------------------------------------*
 |  Look who's listening on the I�C bus, using i2c.library                |
 |  Usage:                                                                |
 |    I2CScan [options]                                                   |
 |  where options are:                                                    |
 |  -d<delay> : adjust i2c.library's timing parameter                     |
 |  -v : verbose, try to identify the listening chips by their addresses  |
 |  -l : lock, force i2c.library to release its hardware                  |
 |  -w : watch the statistics counters (only with lib version 40+)        |
 |  Return values:                                                        |
 |   0 = OK                                                               |
 |   5 = no listeners detected                                            |
 |  10 = scan failed due to resource allocation problems                  |
 |  15 = errors occured which might indicate a hardware problem           |
 |  20 = library not found                                                |
 |                                                                        |
\*========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>

#ifdef __AROS__
#include <aros/asmcall.h>
#include <proto/arossupport.h>
#include <aros/debug.h>
#else
#include <clib/debug_protos.h>
#endif

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/exall.h>
#include <utility/tagitem.h>

#include <string.h>
#include <proto/i2c.h>
#include <libraries/i2c.h>

struct UtilityBase *UtilityBase;

#ifndef __AROS__
#define AROS_BSTR_strlen(s) *((UBYTE *)BADDR(s))
#endif

static const char version[] __attribute__((used)) = "$VER: I2CScan 1.1 (" ADATE ")";

struct Library *I2C_Base = NULL;

/* Prototypes */
int main(int argc, char *argv[]);
void identify( UBYTE addr );
int report( ULONG code );
int scan( int verbose );
void intercept( int class );
void watcher();
void help( char *badarg );
void cleanup();

struct chip
{
	UBYTE lower_addr, upper_addr;
	STRPTR description;
};

struct chip chiptab[] =
{
	{ 0x20, 0x20, "PCF8200: speech synthesizer" },
	{ 0x20, 0x22, "SAF1135/SAA4700: VPS decoder" },
	{ 0x22, 0x22, "SAA5243/SAA5246: teletext decoder" },
	{ 0x40, 0x4E, "PCF8574: 8 bit IO expander" },
	{ 0x40, 0x4E, "TDA8444: 8 � 6 bit D/A converter" },
	{ 0x48, 0x4A, "PCD3311/PCD3312: DTMF/modem/musical tone generator" },
	{ 0x70, 0x7E, "PCF8574A: 8 bit IO expander" },
	{ 0x70, 0x76, "SAA1064: LED driver 2-4 � 8" },
	{ 0x70, 0x72, "PCF8576: LCD driver 1-4 � 40" },
	{ 0x74, 0x74, "PCF8577: LCD driver 1-2 � 32" },
	{ 0x76, 0x76, "PCF8577A: LCD driver 1-2 � 32" },
	{ 0x7C, 0x7E, "PCF8566: LCD driver 1-4 � 24" },
	{ 0x78, 0x7A, "PCF8578: LCD dot matrix driver 32�8 / 24�16" },
	{ 0x80, 0x86, "SAA1300: power output 5 � 85 mA" },
	{ 0x88, 0x88, "TDA8442: DAC & switch for color decoder" },
	{ 0x90, 0x9E, "PCF8591: 8 bit DA/AD converter" },
	{ 0x90, 0x9E, "TDA8440: AV input selector" },
	{ 0xA0, 0xA2, "PCF8583: clock/calendar and 256 byte SRAM" },
	{ 0xA0, 0xAE, "PCF8570/PCF8571: 256/128 byte SRAM" },
	{ 0xA0, 0xAE, "PCF8581/PCF8582: 256/128 byte EEPROM" },
	{ 0xB0, 0xBE, "PCF8570C: 256 byte SRAM" },
	{ 0xC0, 0xC6, "SAB3035/SAB3036: tuning and control interface" },
	{ 0xD0, 0xD6, "PCF8573: clock/calendar" }
};


void identify( UBYTE addr )
/* Tries its best to "identify" a chip address, but most I�C addresses
 * are ambiguous.
 */
{
	int i;

	Printf( ", might be:\n" );
	for( i = 0; i < (sizeof chiptab / sizeof chiptab[0]); i++ )
		if( addr >= chiptab[i].lower_addr && addr <= chiptab[i].upper_addr )
			Printf( "%s\n", chiptab[i].description );
}


int maxerror;           /* will be used to create the return value */

int report( ULONG code )
/* analyze an i2clib return code, returns TRUE if it indicates an error */
{
	static ULONG lastcode = 0;
	STRPTR s;

	if( code & 0xFF )   /* indicates OK */
		return FALSE;
	else
	{
		if( code != lastcode )
		{
			lastcode = code;
			if( (code >> 8) > I2C_NO_REPLY )
			{
				Printf( "I2C bus: error 0x%06lx, %s\n", code, I2CErrText( code ) );
				s = GetI2COpponent();
				if( s != NULL )
					Printf("\"%s\" has got our hardware\n", s);
				if( ((code >> 8) & 0xff) == I2C_HARDW_BUSY )
					maxerror = 10;
				else
					maxerror = 15;  /* serious hardware problem */
			}
		}
		return TRUE;
	}
}

int scan( int verbose )
/* Return value of scan() is the one that the program itself should return, */
/* as explained at the top of this file. */
{
	int rd=0, wr=0, i;
	char dummy;
	int count=0;

	maxerror = 0;
	for( i=0; i<128; i++ )
	{
		wr = !report( SendI2C(2*i, 0, &dummy) );
		rd = !report( ReceiveI2C(2*i+1, 1, &dummy) );
		if (rd || wr)
		{
			count++;
			if( verbose )
				Printf("\n");
			Printf( "Chip address " );
			if( rd && wr )
				Printf( "0x%02x/0x%02x: R/W", 2*i, 2*i+1 );
			else if ( rd )
				Printf( "0x%02x: R only", 2*i+1 );
			else
				Printf( "0x%02x: W only", 2*i );
			if( verbose )
				identify( 2*i );
			else
				Printf( "\n" );
		}
	}
	if( maxerror == 0 && count == 0 )
		return 5; /* indicate "no replies" */
	else
		return maxerror;
}



int ctrl_c;

void intercept( int class )
{
	ctrl_c = 1;
}

void watcher()
{
	ULONG calls;
	struct I2C_Base *ib;
	char *types[] =
	{
		"parallel port",
		"serial port",
		"floppy port",
		"expansion board",
		"controller chip",
		"5"
	};

	/* install a nicer way of handling ^C */
	ctrl_c = 0;
	//signal( SIGINT, intercept );
	ib = (struct I2C_Base *)I2C_Base;
	Printf( "Hardware implementation: %s. Hit Ctrl-C to exit.\n",
		types[ ib->HwType ] );
	Printf( "\e[0 p       %9s %9s %9s %9s %9s %9s\n\n\n\n",
		"Send", "Recv", "Lost", "Unheard", "Overflow", "Errors" );

	while( !ctrl_c )
	{
		Printf( "\e[3ACalls: %9ld %9ld %9ld %9ld %9ld %9ld\nBytes: %9ld %9ld\n",
			ib->SendCalls, ib->RecvCalls, ib->Lost, ib->Unheard,
			ib->Overflows, ib->Errors, ib->SendBytes, ib->RecvBytes );
		calls = ib->SendCalls + ib->RecvCalls;

		if( calls == 0 )
			calls = 1;

		Printf( "Average: %7ld %9ld %9ld%% %8ld%% %8ld%% %8ld%%\n",
			ib->SendCalls ? ib->SendBytes / ib->SendCalls : 0,
			ib->RecvCalls ? ib->RecvBytes / ib->RecvCalls : 0,
		(100 * ib->Lost)      / calls,
		(100 * ib->Unheard)   / calls,
		(100 * ib->Overflows) / calls,
		(100 * ib->Errors)    / calls );
	}
	Printf( "\e[ p" );          /* make cursor visible again */
	//signal( SIGINT, SIG_DFL );  /* restore default break handler */
}


void help( char *badarg )
{
	Printf( "Illegal option '%s', usage:\n", badarg );
	Printf( "  I2CScan [-d<delay>] [-v[erbose]] [-l[ock]] [-w[atch]]\n" );
}

void cleanup()
{
	if( I2C_Base )
	{
		CloseLibrary( I2C_Base );
		I2C_Base = NULL;
	}
}

int main(int argc, char *argv[])
{
	ULONG busdelay;
	int i, verbose=FALSE, lockit=FALSE, watch=FALSE;
	int result = 0;

	//atexit( cleanup );   /* make sure library will be closed upon Ctrl-C */
	I2C_Base = OpenLibrary( "i2c.library", 39 );
	if( I2C_Base == NULL )
	{
		Printf( "Can't open i2c.library V39+\n" );
		return 20;
	}
	busdelay = SetI2CDelay( I2CDELAY_READONLY );

	for( i=1; i<argc; i++ ) {
		if ( *argv[i] == '-' ) {
			switch ( argv[i][1] ) {
				case 'd':
					busdelay = atol(argv[i] + 2);
					break;
				case 'v':
					verbose = TRUE;
					break;
				case 'w':
					watch = TRUE;
					break;
				case 'l':
					lockit = TRUE;
					break;
				default:
					help(argv[i]);
					cleanup();
					Exit(10);
			}
		} else {
			help(argv[i]);
			cleanup();
			Exit(10);
		}
	}

	if( watch )
	{
		if( I2C_Base->lib_Version < 40 )
		{
			Printf( "need i2c.library v40+ for this option\n" );
			cleanup();
			Exit( 20 );
		}
		watcher();
	}
	else if( lockit )
	{
		ShutDownI2C();
		Printf( "I2C activity halted, hit <Return> to continue... " );
		//getchar();
		BringBackI2C();
		Printf("\e[A\e[49Cthanks.\n");
	}
	else
	{
		SetI2CDelay( busdelay );
		Printf( "Delay value for I2C bus timing is %ld, ", busdelay );
		Printf( "scanning for listeners ...\n" );
		result = scan( verbose );
	}
	cleanup();

	return result;
}