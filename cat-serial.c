/* Copyright 1998-2014 by Chris Osborn <fozztexx@fozztexx.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <fozzlib/getargs.h>
#include <fozzlib/ttlock.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define HPBUF	128

int main(int argc, char *argv[])
{
  int count;
  int fd;
  struct termios state;
  int i, j;
  char buf[HPBUF];
  int enqack = 0;
  

  count = getargs(argc, argv, "eb", &enqack);

  if (count < 0 || (argc - count) < 1) {
    if (count < 0 && -count != '-')
      fprintf(stderr, "Bad flag: %c\n", -count);
    fprintf(stderr, "Usage: %s <device>\n"
	    , *argv);
    exit(1);
  }

  while (ttlock(argv[count]))
    sleep(10);
  
  fd = open(argv[count], O_RDWR);

  tcgetattr(fd, &state);
  cfmakeraw(&state);
  state.c_cflag &= ~(CRTSCTS | PARODD | CSIZE | CSTOPB);
  state.c_cflag |= CS7 | PARENB;
  cfsetispeed(&state, B2400);
  cfsetospeed(&state, B2400);
  tcsetattr(fd, TCSAFLUSH, &state);

  i = 0;
  write(fd, "OE;", 3);
  while ((j = read(0, buf, 1)) > 0) {
    write(fd, buf, j);
    i += j;
    if (i >= HPBUF && buf[0] == ';') {
      i = 0;
      write(fd, "OE;", 3);
      buf[0] = 0;
      for (j = 0; j < 2; )
	j += read(fd, buf, 1);
    }
  }

  close(fd);

  ttunlock();
  
  exit(0);
}
