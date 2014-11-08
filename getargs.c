/* Copyright 1997 by Chris Osborn <fozztexx@fozztexx.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: getargs.c,v 1.2 2006/03/31 18:01:13 fozztexx Exp $
 *
 * $Log: getargs.c,v $
 * Revision 1.2  2006/03/31 18:01:13  fozztexx
 * Added LGPL
 *
 * Revision 1.1.1.1  1998/12/09 22:18:36  fozztexx
 * Re-imported after loss of CVS repositories.
 *
 * Revision 1.1.1.1  1998/08/28 01:07:16  fozztexx
 * Loaded into CVS.
 *
 * Revision 1.1  97/12/20  13:29:51  fozztexx
 * Initial revision
 * 
 */

/***********************************************************************

			      getargs.c
  
		  Copyright (C) 1991 by Chris Osborn
		fozztexx%nvcc.uucp@groucho.sonoma.edu
  

  This function will read the flags from the command line and fill the
  variables that are passed with the appropriate values, if the
  specified flag was on the command line.

  It will return the number of the first non-flag-related argument, or
  else -(bad flag) if a flag that wasn't in the format string was
  present, or a flag requiring an argument didn't have one.

  If an unknown flag is encountered, processing will be aborted, and
  the return will be a negative number which is equal to -(bad flag).
  I designed it this way ON PURPOSE to try to force the programmer to
  spit out some help instead of ignoring the bad flag.

  Flags may be specified seperately, as in
    -a -b H -c 25 -d Hello
  or all together, as in
    -abcd H 25 Hello
  or even mixed, like
    -abc H 25 -d Hello
   
  You need to pass it argc, argv, a format string, and all the
  pointers to variables.

  The structure of the format string is flag, type. The string
     "AbQi"
  specifies that flag "A" is a boolean, and flag "Q" is of type int.
  
  If any of the flags are not present in the passed argv, the contents
  of the corresponding variable will not be changed.

  Supported types are
     b: boolean, sets var to 1 if flag exists
     o: order, sets var to order of flag relative
          to other "order" flags on command line
     c: char, uses first char of next arg
     s: string, copies pointer to next arg
     i: int, uses atoi on next arg
     d: double, uses atof on next arg
     f: calls function with flag and arg pointer,
          function should return number of args used

***********************************************************************/

#include <stdarg.h>
#include <stdlib.h>

int getargs(int argc, char **argv, char *format, ...)
{
  va_list ap;
  int count, i, j, k;
  int order = 0;
  int **arg;
  char **p;
  double *q;
  int *r;
  char **na;
  int atoi();
#ifndef atof
  double atof();
#endif  
  int nf;
  int (*mf)();
  void *malloc();
  

  va_start(ap, format);
  count = argc;

  for (i = 0; format[i]; i++);
  arg = malloc(sizeof (void *) * (i / 2));
  for (i = 0; format[i]; i += 2)
    arg[i / 2] = va_arg(ap, void *);

  for (argc--, na = ++argv, na++; argc && **argv == '-'; argc--, argv = na, na++)
    for ((*argv)++; **argv; (*argv)++) {
      for (nf = 0, i = 0; format[i]; i += 2)
	if (format[i] == **argv) {
	  nf++;
	  switch (format[i + 1]) {
	  case 'b':
	    *(arg[i / 2]) = 1;
	    break;

	  case 'o':
	    order++;
	    *(arg[i / 2]) = order;
	    break;
	  
	  case 'c':
	    argc--;
	    if (!argc) {
	      free(arg);
	      return -(**argv);
	    }
	    r = (int *) arg[i / 2];
	    *r = **na;
	    na++;
	    break;

	  case 's':
	    argc--;
	    if (!argc) {
	      free(arg);
	      return -(**argv);
	    }
	    p = (char **) arg[i / 2];
	    *p = (char *) *na;
	    na++;
	    break;

	  case 'i':
	    argc--;
	    if (!argc) {
	      free(arg);
	      return -(**argv);
	    }
	    *(arg[i / 2]) = atoi(*na);
	    na++;
	    break;

	  case 'd':
	    argc--;
	    if (!argc) {
	      free(arg);
	      return -(**argv);
	    }
	    q = (double *) arg[i / 2];
	    *q = atof(*na);
	    na++;
	    break;

	  case 'f':
	    argc--;
	    if (!argc) {
	      free(arg);
	      return -(**argv);
	    }
	    mf = (void *) arg[i / 2];
	    j = mf((int) format[i], na);
	    for (k = 0; k < j; k++, na++);
	    break;
	  }
	}
      if (!nf) {
	free(arg);
	return -(**argv);
      }
    }

  va_end(ap);
  free(arg);

  return count - argc;
}
