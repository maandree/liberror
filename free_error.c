/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_free_error(struct liberror_error *error)
{
	if (!error)
		return;
	if (error->backtrace && !--error->backtrace->refcount)
		free(error->backtrace);
	liberror_free_error(error->cause);
	if (error->dynamically_allocated)
		free(error);
}
