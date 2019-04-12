/* See LICENSE file for copyright and license details. */
#include "liberror.h"

#include <alloca.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#if defined(__GNUC__)
# define GCC_ATTRIBUTES(...) __attribute__((__VA_ARGS__))
#else
# define GCC_ATTRIBUTES(...)
#endif

#define HIDDEN GCC_ATTRIBUTES(__visibility__("hidden"))
#define WEAK   GCC_ATTRIBUTES(__weak__)


struct liberror_backtrace {
	size_t refcount;
	size_t n;
	uintptr_t rips[];
};


HIDDEN extern _Thread_local struct liberror_error liberror_error_;
HIDDEN extern _Thread_local int liberror_have_error_;
extern _Thread_local struct liberror_backtrace *liberror_saved_backtrace;
