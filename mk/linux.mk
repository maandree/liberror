LIBEXT = so
LIBFLAGS = -shared -Wl,-soname,liberror.$(LIBEXT).$(LIB_MAJOR)

LIBMAJOREXT = $(LIBEXT).$(LIB_MAJOR)
LIBMINOREXT = $(LIBEXT).$(LIB_VERSION)

FIX_INSTALL_NAME = :