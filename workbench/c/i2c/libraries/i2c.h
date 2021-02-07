#ifndef LIBRARIES_I2C_H
#define LIBRARIES_I2C_H

/*
**	$VER: i2c.h 40.0 (11.08.98)
**
**	Useful definitions for i2c.library (return codes, libbase etc.)
**
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

/* If you call SetI2CDelay only to read the delay, not change it: */
#define I2CDELAY_READONLY 0xffffffff    /* V39+ ! */

/* Type of delay to pass to AllocI2C (obsolete in V39+, see docs): */
#define DELAY_TIMER 1   /* Use timer.device for SCL-delay  */
#define DELAY_LOOP  2   /* Use for/next-loop for SCL-delay */

/* Allocation Errors */
/* (as returned by AllocI2C, BringBackI2C, or found in the middle high */
/* byte of the error codes from V39's SendI2C/ReceiveI2C) */
enum {
    I2C_OK=0,               /* Hardware allocated successfully */
    I2C_PORT_BUSY,          /* \_Allocation is actually done in two steps: */
    I2C_BITS_BUSY,          /* / port & bits, and each step may fail */
    I2C_NO_MISC_RESOURCE,   /* Shouldn't occur, something's very wrong */
    I2C_ERROR_PORT,         /* Failed to create a message port */
    I2C_ACTIVE,             /* Some other I2C client has pushed us out */
    I2C_NO_TIMER            /* Failed to open the timer.device */
};

/* I/O Errors */
/* (as found in the middle low byte of the error codes from V39's */
/* SendI2C/ReceiveI2C) */
enum {
    /*I2C_OK=0,*/       /* Last send/receive was OK */
    I2C_REJECT=1,       /* Data not acknowledged (i.e. unwanted) */
    I2C_NO_REPLY,       /* Chip address apparently invalid */
    SDA_TRASHED,        /* SDA line randomly trashed. Timing problem? */
    SDA_LO,             /* SDA always LO \_wrong interface attached, */
    SDA_HI,             /* SDA always HI / or none at all? */
    SCL_TIMEOUT,        /* \_Might make sense for interfaces that can */
    SCL_HI,             /* / read the clock line, but currently none can. */
    I2C_HARDW_BUSY      /* Hardware allocation failed */
};

/* ======================================================================== */
/* === I2C_Base =========================================================== */
/* ======================================================================== */
/*
 * Starting with V40, i2c.library exposes some statistics counters, and a
 * hint what kind of hardware implementation you are dealing with, in its
 * base structure. These data weren't present in any of the previous
 * releases, so check the lib version before you try to read them.
 */

/* This structure is READ ONLY, and only present in V40 or later! */
struct I2C_Base
{
    struct Library LibNode;
    ULONG SendCalls;        /* calls to SendI2C */
    ULONG SendBytes;        /* bytes actually sent */
    ULONG RecvCalls;        /* calls to ReceiveI2C */
    ULONG RecvBytes;        /* bytes actually received */
    ULONG Lost;             /* calls rejected due to resource conflicts */
    ULONG Unheard;          /* calls to addresses that didn't reply */
    ULONG Overflows;        /* times a chip rejected some or all of our data */
    ULONG Errors;           /* errors caused by hardware/timing problems */
    UBYTE HwType;           /* implementation: 0=par, 1=ser, 2=disk, 3=card */

    /* The data beyond this point is private and is different between
     * most of the various i2c.library implementations anyway.
     */
};

#endif  /* LIBRARIES_I2C_H */
