/* See LICENSE file for copyright and license details. */
#include "internal.h"


#define STRDUP(S) ((S) ? strdup(S) : NULL)

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

		switch (dest->details_type) {
		case LIBERROR_DETAILS_USER:
			dest->details.user.data = dest->details.user.copy_data(dest->details.user.data);
			if (!dest->details.user.data)
				goto fail;
			break;

		case LIBERROR_DETAILS_ONE_FILE:
			dest->details.one_file.name = STRDUP(src->details.one_file.name);
			if (src->details.one_file.name && !dest->details.one_file.name)
				goto fail;
			break;

		case LIBERROR_DETAILS_TWO_FILES:
			dest->details.two_files.name1 = STRDUP(src->details.two_files.name1);
			dest->details.two_files.name2 = STRDUP(src->details.two_files.name2);
			if (src->details.two_files.name1 && !dest->details.two_files.name1)
				goto fail;
			if (src->details.two_files.name2 && !dest->details.two_files.name2)
				goto fail;
			break;

		case LIBERROR_DETAILS_NONE:
		default:
			break;
		}
	}
	errno = saved_errno;
	return dest;

fail:
	liberror_free_error(dest);
	errno = saved_errno;
	return NULL;
}
