/* See LICENSE file for copyright and license details. */
#include "internal.h"


struct liberror_error *
liberror_copy_error(struct liberror_error *src)
{
	struct liberror_error *dest;
	int saved_errno;
	if (!src)
		return NULL;
	saved_errno = errno;
	dest = malloc(sizeof(*dest));
	if (dest) {
		memcpy(dest, src, sizeof(*src));
		if (dest->backtrace)
			dest->backtrace->refcount += 1;
		dest->dynamically_allocated = 1;
		if (src->cause) {
			dest->cause = liberror_copy_error(src->cause);
			if (!dest->cause) {
				if (dest->backtrace)
					dest->backtrace->refcount -= 1;
				free(dest);
				dest = NULL;
			}
		}
	}
	errno = saved_errno;
	return dest;
}
