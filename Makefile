# Based from both usr.bin/patch/Makefile and mk/bsd.prog.mk.

PROG =	patch
SRCS = backupfile.c ed.c inp.c mkpath.c patch.c pch.c util.c
CFILES = $(filter %.c,$(SRCS))
OBJS += $(CFILES:.c=.o)

# Compatibility single-header library.
COMPAT = -include baiacu.h

# Paths
ROOT =
CCSBIN = /usr/ccs/bin
CCSMAN = /usr/ccs/share/man
UCBINST = install

all: $(PROG)
ifdef SRCS
$(PROG) : $(OBJS)
else
$(PROG) : % : %.o
endif

%.o: %.c
	@echo $(CC) $(COMPAT) -c $(CFLAGS) $(<F)
	@$(CC) $(COMPAT) -c $(CFLAGS) $(CPPFLAGS) $<

$(PROG):
	@echo $(CC) -o $(@F) $(^F) $(LDADD)
	@$(CC) -o $@ $^ $(LDPATHS) $(LDFLAGS)

install: all
	$(UCBINST) -d $(ROOT)$(CCSBIN)
	$(UCBINST) -d $(ROOT)$(CCSMAN)/man1
	$(UCBINST) -c patch $(ROOT)$(CCSBIN)/patch
	$(UCBINST) -c -m 644 patch.1 $(ROOT)$(CCSMAN)/man1/patch.1

clean:
	rm -f $(PROG) $(OBJS)
