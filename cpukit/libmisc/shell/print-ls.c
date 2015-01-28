/*	$NetBSD: print.c,v 1.40 2004/11/17 17:00:00 mycroft Exp $	*/

/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Michael Fischbein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if 0
#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)print.c	8.5 (Berkeley) 7/28/94";
#else
__RCSID("$NetBSD: print.c,v 1.40 2004/11/17 17:00:00 mycroft Exp $");
#endif
#endif /* not lint */
#endif

#include <inttypes.h>

#include <rtems.h>
#include <rtems/libio.h>

#include <sys/param.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fts.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <tzfile.h>
#include <unistd.h>
//#include <util.h>

#include "internal.h"
#include "extern-ls.h"

#define DAYSPERNYEAR ((time_t)365)
#define SECSPERDAY   ((time_t)60 * (time_t)60 * (time_t)24)


#if RTEMS_REMOVED
extern int termwidth;
#endif

static int	printaname(rtems_shell_ls_globals* globals, FTSENT *, int, int);
static void	printlink(rtems_shell_ls_globals* globals, FTSENT *);
static void	printtime(rtems_shell_ls_globals* globals, time_t);
static int	printtype(u_int);

#if RTEMS_REMOVED
static time_t	now;
#endif

#define	IS_NOPRINT(p)	((p)->fts_number == NO_PRINT)

void
printscol(rtems_shell_ls_globals* globals, DISPLAY *dp)
{
	FTSENT *p;

	for (p = dp->list; p; p = p->fts_link) {
		if (IS_NOPRINT(p))
			continue;
		(void)printaname(globals, p, dp->s_inode, dp->s_block);
		(void)putchar('\n');
	}
}

void
printlong(rtems_shell_ls_globals* globals, DISPLAY *dp)
{
	struct stat *sp;
	FTSENT *p;
	NAMES *np;
	char buf[20]; //, szbuf[5];

	now = time(NULL);

	if (dp->list->fts_level != FTS_ROOTLEVEL && (f_longform || f_size)) {
#if RTEMS_REMOVED
		if (f_humanize) {
			if ((humanize_number(szbuf, sizeof(szbuf), dp->stotal,
			    "", HN_AUTOSCALE,
			    (HN_DECIMAL | HN_B | HN_NOSPACE))) == -1)
				err(exit_jump, 1, "humanize_number");
			(void)printf("total %s\n", szbuf);
		} else {
#endif
			(void)printf("total %llu\n",
			    (long long)(howmany(dp->btotal, blocksize)));
#if RTEMS_REMOVED
		}
#endif
	}

	for (p = dp->list; p; p = p->fts_link) {
		if (IS_NOPRINT(p))
			continue;
		sp = p->fts_statp;
		if (f_inode)
			(void)printf("%*lu ", dp->s_inode, sp->st_ino);
		if (f_size && !f_humanize) {
			(void)printf("%*llu ", dp->s_block,
			    (long long)howmany(sp->st_blocks, blocksize));
		}
		(void)strmode(sp->st_mode, buf);
		np = p->fts_pointer;
		(void)printf("%s %*lu ", buf, dp->s_nlink,
		    (unsigned long)sp->st_nlink);
		if (!f_grouponly)
			(void)printf("%-*s  ", dp->s_user, np->user);
		(void)printf("%-*s  ", dp->s_group, np->group);
		if (f_flags)
			(void)printf("%-*s ", dp->s_flags, np->flags);
		if (S_ISCHR(sp->st_mode) || S_ISBLK(sp->st_mode))
			(void)printf("%*"PRIu32", %*"PRIu32" ",
			    dp->s_major, major(sp->st_rdev), dp->s_minor,
			    minor(sp->st_rdev));
		else
#if RTEMS_REMOVED
			if (f_humanize) {
				if ((humanize_number(szbuf, sizeof(szbuf),
				    sp->st_size, "", HN_AUTOSCALE,
				    (HN_DECIMAL | HN_B | HN_NOSPACE))) == -1)
					err(1, "humanize_number");
				(void)printf("%*s ", dp->s_size, szbuf);
			} else {
#endif
      {
        unsigned long long size;
        if (sp->st_size < 0)
          size = sp->st_size * -1;
        else
          size = sp->st_size;
				(void)printf("%*llu ", dp->s_size, size);
  		}
		if (f_accesstime)
			printtime(globals, sp->st_atime);
		else if (f_statustime)
			printtime(globals, sp->st_ctime);
		else
			printtime(globals, sp->st_mtime);
		if (f_octal || f_octal_escape)
			(void)safe_print(globals, p->fts_name);
		else if (f_nonprint)
			(void)printescaped(globals, p->fts_name);
		else
			(void)printf("%s", p->fts_name);

		if (f_type || (f_typedir && S_ISDIR(sp->st_mode)))
			(void)printtype(sp->st_mode);
		if (S_ISLNK(sp->st_mode))
			printlink(globals, p);
		(void)putchar('\n');
	}
}

void
printcol(rtems_shell_ls_globals* globals, DISPLAY *dp)
{
	static FTSENT **array;
	static int lastentries = -1;
	FTSENT *p;
	int base, chcnt, col, colwidth, num;
	int numcols, numrows, row;
	//char szbuf[5];

	colwidth = dp->maxlen;
	if (f_inode)
		colwidth += dp->s_inode + 1;
	if (f_size) {
		if (f_humanize)
			colwidth += dp->s_size + 1;
		else
			colwidth += dp->s_block + 1;
	}
	if (f_type || f_typedir)
		colwidth += 1;

	colwidth += 1;

	if (termwidth < 2 * colwidth) {
		printscol(globals, dp);
		return;
	}

	/*
	 * Have to do random access in the linked list -- build a table
	 * of pointers.
	 */
	if (dp->entries > lastentries) {
		lastentries = dp->entries;
		if ((array =
		    realloc(array, dp->entries * sizeof(FTSENT *))) == NULL) {
			warn(NULL);
			printscol(globals, dp);
		}
	}
	for (p = dp->list, num = 0; p; p = p->fts_link)
		if (p->fts_number != NO_PRINT)
			array[num++] = p;

	numcols = termwidth / colwidth;
	colwidth = termwidth / numcols;		/* spread out if possible */
	numrows = num / numcols;
	if (num % numcols)
		++numrows;

	if (dp->list->fts_level != FTS_ROOTLEVEL && (f_longform || f_size)) {
#if RTEMS_REMOVED
		if (f_humanize) {
			if ((humanize_number(szbuf, sizeof(szbuf), dp->stotal,
			    "", HN_AUTOSCALE,
			    (HN_DECIMAL | HN_B | HN_NOSPACE))) == -1)
				err(1, "humanize_number");
			(void)printf("total %s\n", szbuf);
		} else {
#endif
			(void)printf("total %llu\n",
			    (long long)(howmany(dp->btotal, blocksize)));
#if RTEMS_REMOVED
		}
#endif
	}
	for (row = 0; row < numrows; ++row) {
		for (base = row, chcnt = col = 0; col < numcols; ++col) {
			chcnt = printaname(globals, array[base], dp->s_inode,
			    f_humanize && 0 ? dp->s_size : dp->s_block);
			if ((base += numrows) >= num)
				break;
			while (chcnt++ < colwidth)
				(void)putchar(' ');
		}
		(void)putchar('\n');
	}
}

void
printacol(rtems_shell_ls_globals* globals, DISPLAY *dp)
{
	FTSENT *p;
	int chcnt, col, colwidth;
	int numcols;
	//char szbuf[5];

	colwidth = dp->maxlen;
	if (f_inode)
		colwidth += dp->s_inode + 1;
	if (f_size) {
		if (f_humanize)
			colwidth += dp->s_size + 1;
		else
			colwidth += dp->s_block + 1;
	}
	if (f_type || f_typedir)
		colwidth += 1;

	colwidth += 1;

	if (termwidth < 2 * colwidth) {
		printscol(globals, dp);
		return;
	}

	numcols = termwidth / colwidth;
	colwidth = termwidth / numcols;		/* spread out if possible */

	if (dp->list->fts_level != FTS_ROOTLEVEL && (f_longform || f_size)) {
#if RTEMS_REMOVED
		if (f_humanize) {
			if ((humanize_number(szbuf, sizeof(szbuf), dp->stotal,
			    "", HN_AUTOSCALE,
			    (HN_DECIMAL | HN_B | HN_NOSPACE))) == -1)
				err(1, "humanize_number");
			(void)printf("total %s\n", szbuf);
		} else {
#endif
			(void)printf("total %llu\n",
			    (long long)(howmany(dp->btotal, blocksize)));
#if RTEMS_REMOVED
		}
#endif
	}
	chcnt = col = 0;
	for (p = dp->list; p; p = p->fts_link) {
		if (IS_NOPRINT(p))
			continue;
		if (col >= numcols) {
			chcnt = col = 0;
			(void)putchar('\n');
		}
		chcnt = printaname(globals, p, dp->s_inode,
		    f_humanize && 0 ? dp->s_size : dp->s_block);
		while (chcnt++ < colwidth)
			(void)putchar(' ');
		col++;
	}
	(void)putchar('\n');
}

void
printstream(rtems_shell_ls_globals* globals, DISPLAY *dp)
{
	FTSENT *p;
	int col;
	int extwidth;

	extwidth = 0;
	if (f_inode)
		extwidth += dp->s_inode + 1;
	if (f_size) {
		if (f_humanize)
			extwidth += dp->s_size + 1;
		else
			extwidth += dp->s_block + 1;
	}
	if (f_type)
		extwidth += 1;

	for (col = 0, p = dp->list; p != NULL; p = p->fts_link) {
		if (IS_NOPRINT(p))
			continue;
		if (col > 0) {
			(void)putchar(','), col++;
			if (col + 1 + extwidth + p->fts_namelen >= termwidth)
				(void)putchar('\n'), col = 0;
			else
				(void)putchar(' '), col++;
		}
		col += printaname(globals, p, dp->s_inode,
		    f_humanize && 0 ? dp->s_size : dp->s_block);
	}
	(void)putchar('\n');
}

/*
 * print [inode] [size] name
 * return # of characters printed, no trailing characters.
 */
static int
printaname(rtems_shell_ls_globals* globals,
           FTSENT *p, int inodefield, int sizefield)
{
	struct stat *sp;
	int chcnt;
	//char szbuf[5];

	sp = p->fts_statp;
	chcnt = 0;
	if (f_inode)
		chcnt += printf("%*lu ", inodefield, sp->st_ino);
	if (f_size) {
#if RTEMS_REMOVED
		if (f_humanize) {
			if ((humanize_number(szbuf, sizeof(szbuf), sp->st_size,
			    "", HN_AUTOSCALE,
			    (HN_DECIMAL | HN_B | HN_NOSPACE))) == -1)
				err(1, "humanize_number");
			chcnt += printf("%*s ", sizefield, szbuf);
		} else {
#endif
			chcnt += printf("%*llu ", sizefield,
			    (long long)howmany(sp->st_blocks, blocksize));
#if RTEMS_REMOVED
		}
#endif
	}
	if (f_octal || f_octal_escape)
		chcnt += safe_print(globals, p->fts_name);
	else if (f_nonprint)
		chcnt += printescaped(globals, p->fts_name);
	else
		chcnt += printf("%s", p->fts_name);
	if (f_type || (f_typedir && S_ISDIR(sp->st_mode)))
		chcnt += printtype(sp->st_mode);
	return (chcnt);
}

static void
printtime(rtems_shell_ls_globals* globals, time_t ftime)
{
	int i;
	char *longstring;

	longstring = ctime(&ftime);
	for (i = 4; i < 11; ++i)
		(void)putchar(longstring[i]);

#define	SIXMONTHS	((DAYSPERNYEAR / 2) * SECSPERDAY)
	if (f_sectime)
		for (i = 11; i < 24; i++)
			(void)putchar(longstring[i]);
	else if (ftime + SIXMONTHS > now && ftime - SIXMONTHS < now)
		for (i = 11; i < 16; ++i)
			(void)putchar(longstring[i]);
	else {
		(void)putchar(' ');
		for (i = 20; i < 24; ++i)
			(void)putchar(longstring[i]);
	}
	(void)putchar(' ');
}

static int
printtype(u_int mode)
{
	switch (mode & S_IFMT) {
	case S_IFDIR:
		(void)putchar('/');
		return (1);
	case S_IFIFO:
		(void)putchar('|');
		return (1);
	case S_IFLNK:
		(void)putchar('@');
		return (1);
	case S_IFSOCK:
		(void)putchar('=');
		return (1);
#if RTEMS_REMOVED
	case S_IFWHT:
		(void)putchar('%');
		return (1);
#endif
	}
	if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
		(void)putchar('*');
		return (1);
	}
	return (0);
}

static void
printlink(rtems_shell_ls_globals* globals, FTSENT *p)
{
	int lnklen;
	char name[MAXPATHLEN + 1], path[MAXPATHLEN + 1];

	if (p->fts_level == FTS_ROOTLEVEL)
		(void)snprintf(name, sizeof(name), "%s", p->fts_name);
	else
		(void)snprintf(name, sizeof(name),
		    "%s/%s", p->fts_parent->fts_accpath, p->fts_name);
	if ((lnklen = readlink(name, path, sizeof(path) - 1)) == -1) {
		(void)fprintf(stderr, "\nls: %s: %s\n", name, strerror(errno));
		return;
	}
	path[lnklen] = '\0';
	(void)printf(" -> ");
	if (f_octal || f_octal_escape)
		(void)safe_print(globals, path);
	else if (f_nonprint)
		(void)printescaped(globals, path);
	else
		(void)printf("%s", path);
}
