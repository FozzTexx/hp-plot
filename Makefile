PRODUCT=	cat-serial
# If PRODUCT= line is missing, be sure to insert one
CFILES= cat-serial.c
CFLAGS= -O -g -Wall -I$(HOME)/Unix/$(OSTYPE)/include
OTHER_LIBS= -lfozzlib
MAKEFILEDIR=/usr/local/Makefiles
MAKEFILE=single.make

-include Makefile.preamble

include $(MAKEFILEDIR)/$(MAKEFILE)

-include Makefile.postamble

-include Makefile.dependencies
