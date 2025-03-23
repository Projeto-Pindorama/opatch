# Based from both usr.bin/patch/Makefile and mk/bsd.prog.mk.

PROG =	patch
SRCS = backupfile.c ed.c inp.c mkpath.c patch.c pch.c util.c
CFILES = $(filter %.c,$(SRCS))
OBJS += $(CFILES:.c=.o)

all: $(PROG)

ifdef SRCS
$(PROG) : $(OBJS)
else
$(PROG) : % : %.o
endif

%.o: %.c
	@echo $(CC) -include ignorance.h -c $(CFLAGS) $(<F)
	@$(CC) -include ignorance.h -c $(CFLAGS) $(CPPFLAGS) $<

$(PROG):
	@echo $(CC) -o $(@F) $(^F) $(LDADD)
	@$(CC) -o $@ $^ $(LDPATHS) $(LDFLAGS)

clean:
	rm -f $(PROG) $(PROG).o
