/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: 
    Lang: English
*/


#include <proto/utility.h>
#include <proto/exec.h>

#include "camd_intern.h"


/*****************************************************************************

    NAME */

	AROS_LH2(ULONG, GetMidiAttrsA,

/*  SYNOPSIS */
	AROS_LHA(struct MidiNode *, midinode, A0),
	AROS_LHA(struct TagItem *, tags, A1),

/*  LOCATION */
	struct CamdBase *, CamdBase, 10, Camd)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
		SetMidiAttrsA

    INTERNALS

    HISTORY

	2001-01-12 ksvalast first created

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct CamdBase *,CamdBase)

	struct TagItem *tag;
	const struct TagItem *tstate=tags;
	ULONG *where;
	ULONG ret=0;

// Topic, what if camd is locked?!

	ObtainSemaphore(CB(CamdBase)->CLSemaphore);

// Topic, perhaps there should be a check inside here to see
// if the midinode is still alive if the above ObtainSemaphore
// should be done shared in some way.

// Another solution is to tell the user to lock camd if the
// caller is not the owner of the midinode, and remove
// obtain/release here.


	while((tag=NextTagItem(&tstate))){
		ret++;
		where=(ULONG *)tag->ti_Tag;
		switch(tag->ti_Tag){
			case MIDI_Name:
				*where=(ULONG)midinode->mi_Node.ln_Name;
				break;
			case MIDI_SignalTask:
				*where=(ULONG)midinode->mi_SigTask;
				break;
			case MIDI_RecvHook:
				*where=(ULONG)midinode->mi_ReceiveHook;
				break;
			case MIDI_PartHook:
				*where=(ULONG)midinode->mi_ParticipantHook;
				break;
			case MIDI_RecvSignal:
				*where=(ULONG)midinode->mi_ReceiveSigBit;
				break;
			case MIDI_PartSignal:
				*where=(ULONG)midinode->mi_ParticipantSigBit;
				break;
			case MIDI_MsgQueue:
				*where=(ULONG)midinode->mi_MsgQueueSize;
				break;
			case MIDI_SysExSize:
				*where=(ULONG)midinode->mi_SysExQueueSize;
				break;
			case MIDI_TimeStamp:
				*where=(ULONG)midinode->mi_TimeStamp;
				break;
			case MIDI_ErrFilter:
				*where=(ULONG)midinode->mi_ErrFilter;
				break;
			case MIDI_ClientType:
				*where=(ULONG)midinode->mi_ClientType;
				break;
			case MIDI_Image:
				*where=(ULONG)midinode->mi_Image;
				break;
			default:
				ret--;
				break;
		}
	}

	ReleaseSemaphore(CB(CamdBase)->CLSemaphore);

	return ret;

   AROS_LIBFUNC_EXIT
}


