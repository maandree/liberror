/* See LICENSE file for copyright and license details. */
#include "internal.h"


void
liberror_reset_error(void)
{
	liberror_free_error(liberror_get_error());
	liberror_have_error_ = 0;
}
