/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 *
 *	$Id: db_write_cmd.c,v 1.9 1997/02/22 09:28:34 peter Exp $
 */

/*
 *	Author: David B. Golub,  Carnegie Mellon University
 *	Date:	7/90
 */

#include <sys/param.h>

#include <ddb/ddb.h>
#include <ddb/db_access.h>
#include <ddb/db_command.h>
#include <ddb/db_sym.h>

/*
 * Write to file.
 */
/*ARGSUSED*/
void
db_write_cmd(address, have_addr, count, modif)
	db_expr_t	address;
	boolean_t	have_addr;
	db_expr_t	count;
	char *		modif;
{
	register
	db_addr_t	addr;
	register
	db_expr_t	old_value;
	db_expr_t	new_value;
	register int	size;
	boolean_t	wrote_one = FALSE;

	addr = (db_addr_t) address;

	switch (modif[0]) {
	    case 'b':
		size = 1;
		break;
	    case 'h':
		size = 2;
		break;
	    case 'l':
	    case '\0':
		size = 4;
		break;
	    default:
		db_error("Unknown size\n");
		return;
	}

	while (db_expression(&new_value)) {
	    old_value = db_get_value(addr, size, FALSE);
	    db_printsym(addr, DB_STGY_ANY);
	    db_printf("\t\t%#8n\t=\t%#8n\n", old_value, new_value);
	    db_put_value(addr, size, new_value);
	    addr += size;

	    wrote_one = TRUE;
	}

	if (!wrote_one)
	    db_error("Nothing written.\n");

	db_next = addr;
	db_prev = addr - size;

	db_skip_to_eol();
}

