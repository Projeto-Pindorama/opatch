# Based from both usr.bin/patch/Makefile and mk/bsd.prog.mk.

PROG =	patch
SRCS = backupfile.c ed.c inp.c mkpath.c patch.c pch.c util.c
CFILES = $(filter %.c,$(SRCS))
OBJS += $(CFILES:.c=.o)
# Compatibility single-header library.
COMPAT = -include ignorance.h

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

clean:
	rm -f $(PROG) $(PROG).o
