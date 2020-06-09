/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_set_error_existing(struct liberror_error *error)
{
	liberror_reset_error();
	if (error) {
		liberror_have_error_ = 1;
		memcpy(&liberror_error_, error, sizeof(*error));
		if (error->dynamically_allocated) {
			liberror_error_.dynamically_allocated = 0;
			free(error);
		}
	}
}
