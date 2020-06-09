.POSIX:

CONFIGFILE = config.mk
include $(CONFIGFILE)

OS = linux
# linux = Linux
# macos = Mac OS
include $(OS).mk

LIB_MAJOR = 1
LIB_MINOR = 0
LIB_VERSION = $(LIB_MAJOR).$(LIB_MINOR)


HDR =\
	liberror.h\
	internal.h

OBJ =\
	copy_error.o\
	end.o\
	free_error.o\
	get_error.o\
	internal.o\
	print_backtrace.o\
	print_error.o\
	reset_error.o\
	save_backtrace.o\
	set_error.o\
	set_error_errno.o\
	start.o

MAN0 =\
	liberror.h.0

MAN3 =\
	liberror_copy_error.3\
	liberror_end.3\
	liberror_free_error.3\
	liberror_get_error.3\
	liberror_print_backtrace.3\
	liberror_print_error.3\
	liberror_reset_error.3\
	liberror_save_backtrace.3\
	liberror_set_error.3\
	liberror_set_error_errno.3\
	liberror_start.3

MAN7 =\
	liberror.7

LOBJ = $(OBJ:.o=.lo)


all: liberror.a liberror.$(LIBEXT)
$(OBJ): $(@:.o=.c) $(HDR)
$(LOBJ): $(@:.lo=.c) $(HDR)

liberror.a: $(OBJ)
	-rm -f -- $@
	$(AR) rc $@ $(OBJ)
	$(AR) s $@

liberror.$(LIBEXT): $(LOBJ)
	$(CC) $(LIBFLAGS) -o $@ $(LOBJ) $(LDFLAGS)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.c.lo:
	$(CC) -c -o $@ $< -fPIC $(CFLAGS) $(CPPFLAGS)

install: liberror.a liberror.$(LIBEXT)
	mkdir -p -- "$(DESTDIR)$(PREFIX)/lib"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/include"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/share/licenses/liberror"
	mkdir -p -- "$(DESTDIR)$(MANPREFIX)/man0"
	mkdir -p -- "$(DESTDIR)$(MANPREFIX)/man3"
	mkdir -p -- "$(DESTDIR)$(MANPREFIX)/man7"
	cp -- liberror.a "$(DESTDIR)$(PREFIX)/lib"
	cp -- liberror.$(LIBEXT) "$(DESTDIR)$(PREFIX)/lib/liberror.$(LIBMINOREXT)"
	ln -sf -- liberror.$(LIBMINOREXT) "$(DESTDIR)$(PREFIX)/lib/liberror.$(LIBMAJOREXT)"
	ln -sf -- liberror.$(LIBMINOREXT) "$(DESTDIR)$(PREFIX)/lib/liberror.$(LIBEXT)"
	cp -- liberror.h "$(DESTDIR)$(PREFIX)/include"
	cp -- LICENSE "$(DESTDIR)$(PREFIX)/share/licenses/liberror"
	cp -- $(MAN0) "$(DESTDIR)$(MANPREFIX)/man0"
	cp -- $(MAN3) "$(DESTDIR)$(MANPREFIX)/man3"
	cp -- $(MAN7) "$(DESTDIR)$(MANPREFIX)/man7"

uninstall:
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/liberror."*
	-rm -f -- "$(DESTDIR)$(PREFIX)/include/liberror.h"
	-rm -rf -- "$(DESTDIR)$(PREFIX)/share/licenses/liberror"
	-cd -- "$(DESTDIR)$(MANPREFIX)/man0" && rm -f -- $(MAN0)
	-cd -- "$(DESTDIR)$(MANPREFIX)/man3" && rm -f -- $(MAN3)
	-cd -- "$(DESTDIR)$(MANPREFIX)/man7" && rm -f -- $(MAN7)

clean:
	-rm -f -- *.o *.lo *.a *.so *.so.* *.su *.test

.SUFFIXES:
.SUFFIXES: .c .o .lo .a

.PHONY: all install uninstall clean
