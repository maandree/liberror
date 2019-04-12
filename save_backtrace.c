/* See LICENSE file for copyright and license details. */
#include "internal.h"


WEAK int
liberror_save_backtrace(struct liberror_error *error)
{
	(void) error;
	return 0;
}
