/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$
*/
#ifndef PROTO_REQTOOLS_H
#define PROTO_REQTOOLS_H

#ifndef AROS_SYSTEM_H
#include <aros/system.h>
#endif

#include <clib/reqtools_protos.h>

#if defined(_AMIGA) && defined(__GNUC__)
#ifndef NO_INLINE_STDARG
#define NO_INLINE_STDARG
#endif
#include <inline/reqtools.h>
#else
#include <defines/reqtools.h>
#endif

#endif /* PROTO_REQTOOLS_H */
