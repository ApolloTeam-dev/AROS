/*
    Copyright © 2020, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <exec/rawfmt.h>

#include <string.h>

#include "../../../dist_config.h"

char *generate_banner()
{
#if defined(__DISTRONAME__)
	BYTE const CDistro[] = "" __DISTRONAME__ " (" __DISTROVERSION__ ", " __DISTRODATE__ ")";
#endif

    BYTE const CExtra[] = "Licensed under the AROS Public License.\n"
#if defined(REPOTYPE)
                         "Version " REPOTYPE "" REPOREVISION
#if defined(REPOID)
                         " (" REPOID ")"
#endif
                         "\n"
#endif
                         "built on " ISODATE ".\n";

    char *banner;

#if defined(__DISTRONAME__)
	IPTR CParams[4];
	CParams[0] = (IPTR)CDistro;
	CParams[1] = (IPTR)TaggedOpenLibrary(-2);
    CParams[2] = (IPTR)TaggedOpenLibrary(-3);
    CParams[3] = (IPTR)CExtra;
	banner = AllocVec(strlen((char*)CParams[0]) + strlen((char *)CParams[1]) + strlen((char *)CParams[2]) + strlen((char *)CParams[3]) + 2, MEMF_CLEAR);
    RawDoFmt("%s\n%s%s\n%s", (RAWARG)CParams, RAWFMTFUNC_STRING, banner);
#else
	IPTR CParams[3];
	CParams[0] = (IPTR)TaggedOpenLibrary(-2);
	CParams[1] = (IPTR)TaggedOpenLibrary(-3);
	CParams[2] = (IPTR)CExtra;
    banner = AllocVec(strlen((char *)CParams[0]) + strlen((char *)CParams[1]) + strlen((char *)CParams[2]) + 2, MEMF_CLEAR);
    RawDoFmt("%s%s\n%s", (RAWARG)CParams, RAWFMTFUNC_STRING, banner);
#endif
    return banner;
}
