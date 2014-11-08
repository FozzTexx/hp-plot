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
 * $Id: ttlock.c,v 1.4 2006/03/31 18:01:13 fozztexx Exp $
 */

#define _GNU_SOURCE
#include <stdio.h>

#include "ttlock.h"
#include "strings.h"
#include <sys/param.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#ifdef linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#else
#include <libc.h>
#endif

#define LFNAML 50
#ifdef linux
#define LOCK_DIR "/var/lock"
#else
#define LOCK_DIR "/usr/spool/uucp/LCK"
#endif

static int haslock = 0;
static char flfnam[MAXPATHLEN+1];

int ttlock(const char *ttdev)
{
  int x;
  int lockfd;
  int pid;
  int fpid;
  int tries;
  const char *device;
  char *devname;
  char lockfil[LFNAML];
  char tmpnam[LFNAML+30];
  char *lockdir = LOCK_DIR;
#ifndef NeXT
  char *buf = NULL;
#endif

  
  haslock = 0;
  *flfnam = 0;
  pid = getpid();

  device = ((devname = rindex(ttdev,'/')) != NULL ? devname + 1 : ttdev);

  sprintf(lockfil,"LCK..%s", device);

  sprintf(flfnam, "%s/%s", lockdir, lockfil);
  sprintf(tmpnam, "%s/LTMP.%05d", lockdir, (int) pid);

  lockfd = creat(tmpnam, 0444);
  if (lockfd < 0) {
    perror(lockdir);
    unlink(tmpnam);
    return -1;
  }

#ifdef NeXT
  write(lockfd, (char *) &pid, sizeof(pid));
#else
  asprintf(&buf, "%10i\n", pid);
  write(lockfd, buf, strlen(buf));
  free(buf);
#endif

  close(lockfd);
  chmod(tmpnam, 0444);
  tries = 0;
  while (!haslock && tries++ < 2) {
    haslock = (link(tmpnam, flfnam) == 0);
    if (haslock) {
      break;
    }
    else {
      if ((fpid = ttrpid(flfnam)) > -1) {
	if (fpid > 0) {
	  errno = 0;
	  x = kill(fpid, 0);
	  if (x < 0 && errno == ESRCH) {
	    printf("Removing stale lock %s (pid %d terminated)\n", flfnam, fpid);
	    x = unlink(flfnam);
	    continue;
	  }
	  else if ((x < 0 && errno == EPERM) || x == 0) {
	    unlink(tmpnam);
	    return -2;
	  }
	}
	else
	  break;
      }
      else
	break;
    }
  }
  
  unlink(tmpnam);
  
  return haslock ? 0 : -1;
}

/*  T T U N L O C K  */

int ttunlock()
{
  if (haslock && *flfnam) {
    unlink(flfnam);
    *flfnam = 0;
  }

  return 0;
}

#ifdef NeXT
int ttrpid(const char *name)
{
  int x, fd, pid;

  
  fd = open(name,O_RDONLY);
  if (fd > 0) {
    x = read(fd, (char *) &pid, sizeof(pid));
    if (x < 0)
      pid = -1;
    close(fd);
  }
  else
    pid = -1;
  
  return pid;
}
#else
int ttrpid(const char *name)
{
  int pid = -1;
  FILE *file;


  if ((file = fopen(name, "r"))) {
    fscanf(file, "%i", &pid);
    fclose(file);
  }

  return pid;
}
#endif
