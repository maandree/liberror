/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_pop_error(void)
{
	struct liberror_error *cause;
	if (liberror_have_error_) {
		cause = liberror_error_.cause;
		liberror_error_.cause = NULL;
		liberror_set_error_existing(cause);
	}
}
