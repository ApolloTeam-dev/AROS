/*
    Copyright (C) 1995-2000 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include <proto/exec.h>

/*****************************************************************************

    NAME */
#include <exec/tasks.h>
#include <proto/alib.h>

	void DeleteTask (

/*  SYNOPSIS */
	struct Task * task)

/*  FUNCTION
	Get rid of a task which was created by CreateTask().

    INPUTS
	task - The task which was created by CreateTask(). Must be
	    non-NULL.

    RESULT
	None.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	RemTask()

    INTERNALS

    HISTORY

******************************************************************************/
{
    RemTask (task);
} /* DeleteTask */

