/* Core file generic interface routines for BFD.
   Copyright (C) 1990, 91, 92, 93, 94 Free Software Foundation, Inc.
   Written by Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
SECTION
	Core files

DESCRIPTION
	These are functions pertaining to core files.
*/

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

/*
FUNCTION
	bfd_core_file_failing_command

SYNOPSIS
	CONST char *bfd_core_file_failing_command(bfd *abfd);

DESCRIPTION
	Return a read-only string explaining which program was running
	when it failed and produced the core file @var{abfd}.

*/

CONST char *
bfd_core_file_failing_command (abfd)
     bfd *abfd;
{
  if (abfd->format != bfd_core) {
    bfd_set_error (bfd_error_invalid_operation);
    return NULL;
  }
  return BFD_SEND (abfd, _core_file_failing_command, (abfd));
}

/*
FUNCTION
	bfd_core_file_failing_signal

SYNOPSIS
	int bfd_core_file_failing_signal(bfd *abfd);

DESCRIPTION
	Returns the signal number which caused the core dump which
	generated the file the BFD @var{abfd} is attached to.
*/

int
bfd_core_file_failing_signal (abfd)
     bfd *abfd;
{
  if (abfd->format != bfd_core) {
    bfd_set_error (bfd_error_invalid_operation);
    return 0;
  }
  return BFD_SEND (abfd, _core_file_failing_signal, (abfd));
}

/*
FUNCTION
	core_file_matches_executable_p

SYNOPSIS
	boolean core_file_matches_executable_p
		(bfd *core_bfd, bfd *exec_bfd);

DESCRIPTION
	Return <<true>> if the core file attached to @var{core_bfd}
	was generated by a run of the executable file attached to
	@var{exec_bfd}, <<false>> otherwise.
*/
boolean
core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
    if ((core_bfd->format != bfd_core) || (exec_bfd->format != bfd_object)) {
	    bfd_set_error (bfd_error_wrong_format);
	    return false;
	}

    return BFD_SEND (core_bfd, _core_file_matches_executable_p,
		     (core_bfd, exec_bfd));
}
