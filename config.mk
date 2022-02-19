PREFIX    = /usr
MANPREFIX = $(PREFIX)/share/man

CC = cc -std=c11

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS   = -Wall -pedantic
LDFLAGS  = -s
